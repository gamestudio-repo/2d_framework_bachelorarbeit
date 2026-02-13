template<typename objecttype,int struc_id=0,typename object_form=rect<2>>
class Quadtree:public StructureInterface<objecttype,struc_id,object_form>{
	Quadtree* root=nullptr;
	int current_depth=0;
	int max_depth=0;
	// Größe und Position des Quadranten
	rect<2> frame;
	//0: left_top; 1: left_bot; 2: right_top; 3: right_bot
	PArray<Quadtree*> boundarys={nullptr,nullptr,nullptr,nullptr};
	//Elemente in Quadranten
	PArray<objecttype> array;
	
	//################insert-Helper-Functions######################################################
	
	void add(const objecttype& object){
		array.add(object);
	}
	
	rect<2> quadBoundaryRect(byte placing) {
		int rightW = frame[0] / 2;
		int leftW  = frame[0] - rightW;
		int botH    = frame[1] / 2;
		int topH    = frame[1] - botH;
		bool toBottom = (placing & 1) != 0;
		bool toRight  = (placing & 2) != 0;
		rect<2> child;
		child[0]      = toRight  ? rightW  : leftW;   // Breite
		child[1]      = toBottom ? botH     : topH;    // Höhe
		child.pos[0] = frame.pos[0] + (toRight  ? leftW  : 0);
		child.pos[1] = frame.pos[1] + (toBottom ? topH   : 0);
		return child;
	}

	
	// Entscheide, ob das Objekt komplett in einen der 4 Sub-Quadranten passt.
    // Rückgabe: 0..3 Kind-Index, 4 = passt in keines
    byte fitInBoundary(const objecttype& objPtr) {
		const rect<2>& obj = objPtr->getRect();
		// 1) Werte des Eltern-Frames
		int x0 = frame.pos[0];
		int y0 = frame.pos[1];
		int W  = frame[0];
		int H  = frame[1];
		// 2) Halbe Dimensionen wie in quadBoundaryRect
		int rightW = W / 2;
		int leftW  = W - rightW;
		int botH    = H / 2;
		int topH    = H -botH;
		// 3) Mittellinien (exklusive Rechts-/Unterränder)
		int midX = x0 + leftW;
		int midY = y0 + topH;

		// 4) Koordinaten des Objekts
		int left   = obj.pos[0];
		int right  = obj.pos[0] + obj[0];
		int top    = obj.pos[1];
		int bottom = obj.pos[1] + obj[1];

		// 5) In welchem Quadranten liegt es *vollständig*?
		bool inLeft  = (left  >= x0   && right  <= midX);
		bool inRight = (left  >= midX && right  <= x0 + W);
		bool inTop   = (top   >= y0   && bottom <= midY);
		bool inBot   = (top   >= midY && bottom <= y0 + H);

		if (inLeft  && inTop)  return 0;  // left_top
		if (inLeft  && inBot)  return 1;  // left_bot
		if (inRight && inTop)  return 2;  // right_top
		if (inRight && inBot)  return 3;  // right_bot
		return 4;                          // passt in keinen Sub-Quadranten
	}

	
	// Fügt Objekt in die Datenstruktur ein
	bool intern_insert(const objecttype& object, Quadtree* rootpointer) {
		if (!frame.contain(object->getRect())){
			return false;
		}
		if (current_depth == max_depth) {
			add(object);
			return true;
		}
		byte placing = fitInBoundary(object);
		if (placing < 4) {
			// passt exakt in ein Kind
			if (boundarys[placing]==nullptr) {
				boundarys[placing] = new Quadtree(quadBoundaryRect(placing),max_depth,current_depth + 1);
				boundarys[placing]->root   = rootpointer;
			}
			return boundarys[placing]->intern_insert(object, rootpointer);
		}
		// => Objekt passt in kein Kind, bleibt im aktuellen Knoten
		add(object);
		return true;
	}
	
	bool removeByFrame(const objecttype& object,const object_form& objectframe,int mode=0){
		if(!frame.contain(objectframe)){
			return false;
		}
		for(byte i=0;i<boundarys.elements();i++){
			if(boundarys[i]!=nullptr){
				if((boundarys[i]->removeByFrame(object,objectframe,mode))){
					if(boundarys[i]->isEmpty()){
						(boundarys[i]->array).clear();
						delete boundarys[i];
						boundarys[i]=nullptr;
					}
					return true;
				}
			}
		}
		for(int i=0;i<array.elements();i++){
			if(array[i]==object){
				if(mode!=0){
					if(frame.contain(object->getRect())){
						return false;
					}
				}
				array.remove(i);
				array[array.elements()]=nullptr;
				return true;
			}
		}
		return false;
	}
	
	bool isEmpty(){
		for(byte i=0;i<boundarys.elements();i++){
			if(boundarys[i]!=nullptr){
				return false;
			}
		}
		if(array.elements()>0){
			return false;
		}
		return true;
	}
	
	public:
	
	//Erstelle die Datenstruktur durch Objektliste
	bool build(PArray<objecttype>& elements) override {
		clear();
		root=this;
		bool all_insert=true;
		for(max_uint i=0;i<elements.elements();i++){
			if(!intern_insert(elements[i],root)){
				all_insert=false;
			}
		}
		return all_insert;
	}

	// Fügt Objekt in die Datenstruktur ein
	bool insert(const objecttype& object) override {
		if(root==nullptr){
			root=this;
		}
		return intern_insert(object,root); 
	}
	
	bool remove(const objecttype& object) override {
		return removeByFrame(object,object->getRect());
	}
	
	// Speichert alle gefunden Objekte in "array" und gibt die größe zurück
	size_t query(const object_form& query_rect,PArray<objecttype>& query_array,objecttype object_pointer=nullptr) override {
		if(!frame.intersect(query_rect)){
			return 0;
		}
		size_t elements=0;
		for(size_t i=0;i<array.elements();i++){
			if(array[i]!=object_pointer){
				if(query_rect.intersect(array[i]->getRect())){
					query_array.add(array[i]);
					elements++;
				}
			}
		}
		for(byte i=0;i<boundarys.elements();i++){
			if(boundarys[i]!=nullptr){
				elements+=boundarys[i]->query(query_rect,query_array,object_pointer);
			}
		}
		return elements;
	}
	
	// Falls Objekte sich Updaten (Position,Größe,usw.)
	bool notify(const objecttype& object,const object_form& old_frame) override {
		if(removeByFrame(object,old_frame,1)){
			return insert(object);
		}
		return false;
	}
	
	// Allgemeine Funktion zum Updaten der Struktur->Falls nötig, z.B. Balanzieren oder Überprüfungen
	void update() override {
		
	}
	
	void drawStructure(object_form& pixel_frame,int bitwidth,uint8_t* pixels) override {
		// Zeichne nur, wenn mindestens ein Sub-Quadrant existiert
		bool hasChildren = false;
		for (int i = 0; i < boundarys.elements(); i++) {
			if (boundarys[i] != nullptr) {
				hasChildren = true;
				break;
			}
		}
		if(hasChildren){
			int half_width=frame[0]/2;
			int half_height=frame[1]/2;
			for(int i=0;i<frame[1];i++){
				pixels[bitwidth*(pixel_frame[1]-(i+1+frame.pos[1]))+(frame.pos[0]+half_width)*3+2]=255;
				pixels[bitwidth*(pixel_frame[1]-(i+1+frame.pos[1]))+(frame.pos[0]+half_width)*3+1]=0;
				pixels[bitwidth*(pixel_frame[1]-(i+1+frame.pos[1]))+(frame.pos[0]+half_width)*3]=0;
			}
			for(int i=0;i<frame[0];i++){
				pixels[bitwidth*(pixel_frame[1]-(1+frame.pos[1]+half_height))+(frame.pos[0]+i)*3+2]=255;
				pixels[bitwidth*(pixel_frame[1]-(1+frame.pos[1]+half_height))+(frame.pos[0]+i)*3+1]=0;
				pixels[bitwidth*(pixel_frame[1]-(1+frame.pos[1]+half_height))+(frame.pos[0]+i)*3]=0;
			}
			for(int i=0;i<boundarys.elements();i++){
				if(boundarys[i]!=nullptr){
					boundarys[i]->drawStructure(pixel_frame,bitwidth,pixels);
				}
			}
		}
	}
	
	// Lösche und gebe alle Resourcen frei
	void clear() override {
		array.clear();
		for(byte i=0;i<boundarys.elements();i++){
			if(boundarys[i]!=nullptr){
				boundarys[i]->clear();
				delete boundarys[i];
				boundarys[i]=nullptr;
			}
		}
	}
	
	void clearComplete() override {
		clear();
	}
	
	Quadtree(int x,int y,int width,int height,int new_max_depth=5,int newdepth=0){
		frame.pos[0]=x;
		frame.pos[1]=y;
		frame[0]=width;
		frame[1]=height;
		current_depth=newdepth;
		max_depth=new_max_depth;
	}
	
	Quadtree(const rect<2> newframe,int new_max_depth=5,int newdepth=0){
		frame.pos[0]=newframe.pos[0];
		frame.pos[1]=newframe.pos[1];
		frame[0]=newframe[0];
		frame[1]=newframe[1];
		current_depth=newdepth;
		max_depth=new_max_depth;
	}
	
	~Quadtree(){}
	
};