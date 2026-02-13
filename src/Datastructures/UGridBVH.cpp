template<typename objecttype,int struc_id=0,typename object_form=rect<2>>
class UGridBVH:public StructureInterface<objecttype,struc_id,object_form>{
	
	int tilewidth=0;
	int tileheight=0;
	int rows=0;
	int columns=0;
	BVH<objecttype,struc_id>** list_array=nullptr;

	
	rect<2> getCheckField(const object_form& objectform) {
		// Position relativ zur gemappten Grid-Startposition
		int relX = objectform.pos[0];
		int relY = objectform.pos[1];

		int minX = relX / tilewidth;
		int minY = relY / tileheight;

		int maxX = (relX + objectform[0]  + tilewidth - 1) / tilewidth;
		int maxY = (relY + objectform[1]  + tileheight - 1) / tileheight;
		minX = std::max(0, minX);
		minY = std::max(0, minY);
		maxX = std::min(columns, maxX);
		maxY = std::min(rows, maxY);
		rect<2> field;
		field.pos[0] = minX;
		field.pos[1] = minY;
		field[0] = maxX - minX;
		field[1] = maxY - minY;
		return field;
	}
	
	// Löscht Objekt von der Datenstruktur
	bool removeObjectByFrame(const objecttype& object,const object_form& old_frame,int mode=0){
		rect<2> check_rect=getCheckField(old_frame);
		if(mode!=0){
			rect<2> real_rect=getCheckField(object->getRect());
			if(real_rect!=check_rect){
				mode=0;
			}
		}
		bool removed=false;
		for(int y=0;y<check_rect[1];y++){
			for(int x=0;x<check_rect[0];x++){
				if(list_array[(y+check_rect.pos[1])*columns+(x+check_rect.pos[0])]->removeByFrame(object,old_frame,mode)){
					removed=true;
				}
			}
		}
		return removed;
	}
	
	void getAllElements(PArray<objecttype>& elements){
		if(list_array == nullptr) return;
		PArray<objecttype> placeholder;
		std::unordered_set<objecttype> seen;
		seen.reserve(256); // optional: Schätzung der erwarteten Größe

		for(int y = 0; y < rows; ++y){
			for(int x = 0; x < columns; ++x){
				int idx = y * columns + x;
				if(list_array[idx] == nullptr) continue;
				placeholder.clear();
				list_array[idx]->getAllElements(placeholder);
				for(size_t i = 0; i < placeholder.elements(); ++i){
					objecttype obj = placeholder[i];
					if(seen.insert(obj).second){ // neu
						elements.add(obj);
					}
				}
			}
		}
		return elements.elements();
	}
	
	public:
	
	void setSize(int tilewidth,int tileheight,int columns,int rows){
		clearComplete();
		if(columns==0 || rows==0){
			return;
		}
		this->tilewidth=tilewidth;
		this->tileheight=tileheight;
		this->rows=rows;
		this->columns=columns;
		list_array=new BVH<objecttype,struc_id>*[rows*columns];
		MemoryManager::registerAlloc(list_array, sizeof(BVH<objecttype,struc_id>*) * rows * columns, struc_id);
		for(int i=0;i<rows*columns;i++){
			list_array[i]=new BVH<objecttype,struc_id>();
		}
	}
	
	//Erstelle die Datenstruktur durch Objektliste
	bool build(PArray<objecttype>& elements) override {
		clear();
		bool all_insert=true;
		for(int i=0;i<elements.elements();i++){
			if(!insert(elements[i])){
				all_insert=false;
			}
		}
		return all_insert;
	}
	
	// Fügt Objekt in die Datenstruktur ein
	bool insert(const objecttype& object) override {
		rect<2> check_rect=getCheckField(object->getRect());
		if(check_rect[0]==0 || check_rect[1]==0){
			return false;
		}
		for(int y=0;y<check_rect[1];y++){
			for(int x=0;x<check_rect[0];x++){
				list_array[(y+check_rect.pos[1]) * columns + (x+check_rect.pos[0])]->insert(object);
			}
		}
		return true;
	}
	
	// Löscht Objekt von der Datenstruktur
	bool remove(const objecttype& object) override {
		return removeObjectByFrame(object,object->getRect());
	}
	
	// Speichert alle gefunden Objekte in "array" und gibt die größe zurück
	size_t query(const object_form& query_rect, PArray<objecttype>& array,objecttype object_pointer=nullptr) override {
		size_t found_elements=0;
		PArray<objecttype> placeholder;
		rect<2> check_rect=getCheckField(query_rect);
		std::unordered_set<objecttype> seen;
		// Optional: reserve to avoid Rehashing
		seen.reserve(256);
		for(int y=0;y<check_rect[1];y++){
			for(int x=0;x<check_rect[0];x++){
				if(list_array[(y+check_rect.pos[1])*columns+(x+check_rect.pos[0])]!=nullptr){
					list_array[(y+check_rect.pos[1])*columns+(x+check_rect.pos[0])]->query(query_rect,placeholder,object_pointer);
					for(size_t k = 0; k < placeholder.elements(); ++k){
						objecttype obj = placeholder[k];
						if(seen.insert(obj).second){ // true, wenn neu
							array.add(obj);
							++found_elements;
						}
					}
					placeholder.clear(); // wichtig: placeholder pro Zelle leeren
				}
			}
		}
		return found_elements;
	}

	
	// Falls Objekte sich Updaten (Position,Größe,usw.)
	bool notify(const objecttype& object,const object_form& old_frame) override {
		if(removeObjectByFrame(object,old_frame,1)){
			return insert(object);
		}
		return false;
	}
	
	// Visuelle Darstellung der Struktur
	void drawStructure(object_form& pixel_frame,int bitwidth,uint8_t* pixels) override {
		uint8_t color[3]={255,0,0};
		int draw_rows=rows;
		int draw_columns=columns;
		int start_x=0;
		int start_y=0;
		if((start_y+rows*tileheight)>pixel_frame[1]){
			draw_rows=(((rows*tileheight)-((start_y+rows*tileheight)-pixel_frame[1]))/tileheight);
		}
		if((start_x+columns*tilewidth)>pixel_frame[0]){
			draw_columns=(((columns*tilewidth)-((start_x+columns*tilewidth)-pixel_frame[0]))/tilewidth);
		}
		for(int y=0;y<draw_rows;y++){
			for(int x=0;x<draw_columns;x++){
				list_array[y*columns+x]->drawStructure(pixel_frame,bitwidth,pixels);
				for(int w=0;w<tilewidth;w++){
					pixels[bitwidth*(pixel_frame[1]-(y*tileheight+1+start_y))+(x*tilewidth+w+start_x)*3+2]=color[0];
					pixels[bitwidth*(pixel_frame[1]-(y*tileheight+1+start_y))+(x*tilewidth+w+start_x)*3+1]=color[1];
					pixels[bitwidth*(pixel_frame[1]-(y*tileheight+1+start_y))+(x*tilewidth+w+start_x)*3]=color[2];
					if(y+1<draw_rows){
						pixels[bitwidth*(pixel_frame[1]-((y+1)*tileheight+1+start_y))+(x*tilewidth+w+start_x)*3+2]=color[0];
						pixels[bitwidth*(pixel_frame[1]-((y+1)*tileheight+1+start_y))+(x*tilewidth+w+start_x)*3+1]=color[1];
						pixels[bitwidth*(pixel_frame[1]-((y+1)*tileheight+1+start_y))+(x*tilewidth+w+start_x)*3]=color[2];
					}
				}
				for(int h=0;h<tileheight;h++){
					pixels[bitwidth*(pixel_frame[1]-(y*tileheight+1+h+start_y))+(x*tilewidth+start_x)*3+2]=color[0];
					pixels[bitwidth*(pixel_frame[1]-(y*tileheight+1+h+start_y))+(x*tilewidth+start_x)*3+1]=color[1];
					pixels[bitwidth*(pixel_frame[1]-(y*tileheight+1+h+start_y))+(x*tilewidth+start_x)*3]=color[2];
					if(x+1<draw_columns){
						pixels[bitwidth*(pixel_frame[1]-(y*tileheight+1+h+start_y))+((x+1)*tilewidth+start_x)*3+2]=color[0];
						pixels[bitwidth*(pixel_frame[1]-(y*tileheight+1+h+start_y))+((x+1)*tilewidth+start_x)*3+1]=color[1];
						pixels[bitwidth*(pixel_frame[1]-(y*tileheight+1+h+start_y))+((x+1)*tilewidth+start_x)*3]=color[2];
					}
				}
			}
		}
	}	 
	
	// Allgemeine Funktion zum Updaten der Struktur->Falls nötig, z.B. Balanzieren oder Überprüfungen
	void update() override {
		
	}
	
	
	// Lösche und gebe alle Resourcen frei
	void clear() override {
		if(list_array!=nullptr){
			for(int i=0;i<rows*columns;i++){
				if(list_array[i]!=nullptr){
					list_array[i]->clearComplete();
				}
				
			}
		}
	}
	
	void clearComplete() override {
		if(list_array!=nullptr){
			for(int i=0;i<rows*columns;i++){
				if(list_array[i]!=nullptr){
					list_array[i]->clearComplete();
					delete list_array[i];
					list_array[i]=nullptr;
				}
			}
			MemoryManager::unregisterAlloc(list_array);
			delete[] list_array;
			list_array=nullptr;
		}
	}
	
	UGridBVH(int tilewidth,int tileheight,int columns,int rows){
		setSize(tilewidth,tileheight,columns,rows);
	}
	
	UGridBVH(int tilewidth,int tileheight,int width,int height,int size){
		int columns=0;
		int rows=0;
		if(tilewidth>0 && tileheight>0){
			columns=(int)(PMath::roundUp(((float)width)/((float)tilewidth)));
			rows=(int)(PMath::roundUp(((float)height)/((float)tileheight)));
		}
		setSize(tilewidth,tileheight,columns,rows);
	}
	
	UGridBVH(){
	}
	
	~UGridBVH(){
	}
	
};