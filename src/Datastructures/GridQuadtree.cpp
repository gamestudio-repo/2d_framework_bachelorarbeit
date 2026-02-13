template<typename objecttype,int struc_id=0,typename object_form=rect<2>>
class GridQuadtree:public StructureInterface<objecttype,struc_id,object_form>{
	int current_depth=0;
	int max_depth=0;
	int columns=0;
	int rows=0;
	int min_tilewidth=0;
	int min_tileheight=0;
	// Größe und Position des Quadranten
	rect<2> frame;
	//0: left_top; 1: left_bot; 2: right_top; 3: right_bot
	PArray<GridQuadtree*> boundarys={nullptr,nullptr,nullptr,nullptr};
	//Elemente in Quadranten
	bool compressed=false;
	max_uint current_elements=0;
	UGrid<objecttype,struc_id,object_form> grid;
	
	
	//################insert-Helper-Functions######################################################
	
	void initGrid(){
		if(min_tilewidth>0 && min_tileheight>0){
			columns=(int)(PMath::roundUp(((float)frame[0])/((float)min_tilewidth)));
			rows=(int)(PMath::roundUp(((float)frame[1])/((float)min_tileheight)));
			grid.setSize(min_tilewidth,min_tileheight,columns,rows);
			grid.setMapped(frame,true);
		}
	}
	
	void removeGrid(){
		grid.clearComplete();
	}
	
	bool add(const objecttype& object){
		if(grid.insert(object)){
			current_elements++;
			return true;
		}
		return false;
	}
	
	uint8_t removeElement(const objecttype& object,const object_form& objectframe,int mode=0){
		if(mode!=0){
			if(!frame.contain(object->getRect())){
				mode=0;
			}
		}
		if(grid.remove_intern(object,objectframe,mode)){
			if(mode!=0){
				if(grid.insert(object)){
					return 2;
				}
			}
			current_elements--;
			return true;
		}
		return false;
	}
	
	size_t gridQuery(const object_form& query_rect,PArray<objecttype>& query_array,objecttype object_pointer=nullptr){
		return grid.query(query_rect,query_array,object_pointer);
	}
	
	rect<2> getFittingBoundary(const object_form& obj,const object_form& boundary_frame) {
		// 1) Werte des Eltern-Frames
		int  x0 = boundary_frame.pos[0];
		int  y0 = boundary_frame.pos[1];
		int  W  = boundary_frame[0];
		int  H  = boundary_frame[1];

		// 2) Halbe Dimensionen wie in quadBoundaryRect
		int halfW = W / 2;
		int halfH = H / 2;
		int remW  = W - 2 * halfW;
		int remH  = H - 2 * halfH;
		int leftW  = halfW + remW;
		int rightW = halfW ;
		int topH   = halfH + remH;
		int botH   = halfH;

		// 3) Mittellinien
		int midX = x0 + leftW;
		int midY = y0 + topH;

		// 4) Koordinaten des Objekts
		int left   = obj.pos[0];
		int right  = obj.pos[0] + obj[0];
		int top    = obj.pos[1];
		int bottom = obj.pos[1] + obj[1];

		// 5) Initialisiere Rückgabe-Rechteck
		rect<2> result;

		// 6) Prüfe, ob das Objekt vollständig in einen Quadranten passt
		if (left >= x0 && right <= midX) {
			result.pos[0] = x0;
			result[0] = leftW;
		} else if (left >= midX && right <= x0 + W) {
			result.pos[0] = midX;
			result[0] = rightW;
		} else {
			return rect<2>(); // passt horizontal nicht
		}

		if (top >= y0 && bottom <= midY) {
			result.pos[1] = y0;
			result[1] = topH;
		} else if (top >= midY && bottom <= y0 + H) {
			result.pos[1] = midY;
			result[1] = botH;
		} else {
			return rect<2>(); // passt vertikal nicht
		}
		return result;
	}

	
	// Entscheide, ob das Objekt komplett in einen der 4 Sub-Quadranten passt.
    // Rückgabe: 0..3 Kind-Index, 4 = passt in keines
    byte fitInBoundary(const object_form& obj) {
		// 1) Werte des Eltern-Frames
		int x0 = frame.pos[0];
		int y0 = frame.pos[1];
		int W  = frame[0];
		int H  = frame[1];

		// 2) Halbe Dimensionen wie in quadBoundaryRect
		int halfW = W / 2;
		int halfH = H / 2;
		int remW  = W - 2 * halfW;
		int remH  = H - 2 * halfH;
		int leftW  = halfW + remW;
		int rightW = halfW;
		int topH    = halfH + remH;
		int botH    = halfH;

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
	
	rect<2> findMacthingRect(const object_form& form_fit,const object_form& frame,int& current_depth){
		rect<2> perfect_boundary=frame;
		rect<2> try_boundary=getFittingBoundary(form_fit,perfect_boundary);
		while(try_boundary.contain(form_fit) && (current_depth+1)<=max_depth){
			perfect_boundary=try_boundary;
			current_depth++;
			try_boundary=getFittingBoundary(form_fit,perfect_boundary);
		}
		return perfect_boundary;
	}
	
	bool compress_create(byte placing,const object_form& form_fit){
		int real_depth=current_depth;
		rect<2> perfect_boundary=findMacthingRect(form_fit,frame,real_depth);
		if(perfect_boundary!=frame){
			boundarys[placing] = new GridQuadtree(perfect_boundary,max_depth,min_tilewidth,min_tileheight,real_depth);
			if((real_depth-1)>current_depth){
				boundarys[placing]->compressed=true;
			}
			return true;
		}
		return false;
	}
	
	bool isCompressed(){
		return compressed;
	}
	

	bool insert_compressed(byte placing,const objecttype& object) {
		rect<2> object_rect=object->getRect();
		byte check = boundarys[placing]->fitInBoundary(object_rect);
		if(check!=4){
			return boundarys[placing]->intern_insert(object);
		}
		if(boundarys[placing]->frame.contain(object_rect)){
			return boundarys[placing]->add(object);
		}
		GridQuadtree* vorhandener_baum=boundarys[placing];
		boundarys[placing]=nullptr;
		GridQuadtree* object_tree=this;
		if(compress_create(placing,object_rect)){
			object_tree=boundarys[placing];
		}
		byte replacing=object_tree->fitInBoundary(vorhandener_baum->frame);
		if(replacing!=4){
			object_tree->boundarys[replacing]=vorhandener_baum;
			return object_tree->intern_insert(object);
		}
		GridQuadtree* neuerstellter_baum=object_tree;
		boundarys[placing]=nullptr;
		rect<2> vollständiger_baum_frame=vorhandener_baum->frame;
		vollständiger_baum_frame.expand(neuerstellter_baum->frame);
		GridQuadtree* complete_tree=this;
		if(compress_create(placing,vollständiger_baum_frame)){
			complete_tree=boundarys[placing];
		}
		replacing =complete_tree->fitInBoundary(vorhandener_baum->frame);
		complete_tree->boundarys[replacing]=vorhandener_baum;
		replacing = complete_tree->fitInBoundary(neuerstellter_baum->frame);
		complete_tree->boundarys[replacing]=neuerstellter_baum;
		return complete_tree->boundarys[replacing]->intern_insert(object);
		
	}
	
	// Fügt Objekt in die Datenstruktur ein
	bool intern_insert(const objecttype& object) {
		rect<2> object_rect=object->getRect();
		if (!frame.contain(object_rect)){
			return false;
		}
		if (current_depth == max_depth) {
			return add(object);
		}
		byte placing = fitInBoundary(object_rect);
		if (placing < 4) {
			// passt exakt in ein Kind
			if(boundarys[placing]==nullptr){
				compress_create(placing,object_rect);
			}
			if(boundarys[placing]->isCompressed()){
				return insert_compressed(placing,object);
			}
			return boundarys[placing]->intern_insert(object);
		}
		// => Objekt passt in kein Kind, bleibt im aktuellen Knoten
		return add(object);
	}
	//##################################################################################
	
	uint8_t removeByFrame(const objecttype& object,const object_form& objectframe,int mode=0){
		if(!frame.contain(objectframe)){
			return false;
		}
		uint8_t removed=0;
		for(byte i=0;i<boundarys.elements();i++){
			if(boundarys[i]!=nullptr){
				removed=(boundarys[i]->removeByFrame(object,objectframe,mode));
				if(removed==1){
					if(boundarys[i]->isEmpty()){
						boundarys[i]->clearComplete();
						delete boundarys[i];
						boundarys[i]=nullptr;
					}
					return true;
				}
				if(removed==2){
					return 2;
				}
			}
		}
		return removeElement(object,objectframe,mode);
	}
	
	bool isEmpty(){
		if(current_elements>0){
			return false;
		}
		for(byte i=0;i<boundarys.elements();i++){
			if(boundarys[i]!=nullptr){
				return false;
			}
		}
		return true;
	}
	
	public:
	
	void getAllElements(PArray<objecttype>& elements){
		grid.getAllElements(elements);
		for(uint8_t i=0;i<boundarys.elements();i++){
			if(boundarys[i]!=nullptr){
				boundarys[i]->getAllElements(elements);
			}
		}
	}
	
	//Erstelle die Datenstruktur durch Objektliste
	bool build(PArray<objecttype>& elements) override {
		clear();
		current_elements=0;
		bool all_insert=true;
		for(max_uint i=0;i<elements.elements();i++){
			if(!intern_insert(elements[i])){
				all_insert=false;
			}
		}
		return all_insert;
	}

	// Fügt Objekt in die Datenstruktur ein
	bool insert(const objecttype& object) override {
		return intern_insert(object); 
	}
	
	bool remove(const objecttype& object) override {
		if(removeByFrame(object,object->getRect())==1){
			return true;
		}
		return false;
	}
	
	// Speichert alle gefunden Objekte in "array" und gibt die größe zurück
	size_t query(const object_form& query_rect,PArray<objecttype>& query_array,objecttype object_pointer=nullptr) override {
		if(!frame.intersect(query_rect)){
			return 0;
		}
		size_t elements=0;
		if(current_elements>0){
			elements+=gridQuery(query_rect,query_array,object_pointer);
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
		if(removeByFrame(object,old_frame,1)==1){
			return insert(object);
		}
		return false;
	}
	
	// Allgemeine Funktion zum Updaten der Struktur->Falls nötig, z.B. Balanzieren oder Überprüfungen
	void update() override {
		
	}
	
	void drawStructure(object_form& pixel_frame,int bitwidth,uint8_t* pixels) override {
		byte color[3]={0,0,255};
		// Zeichne nur, wenn mindestens ein Sub-Quadrant existiert
		int half_width=frame[0]/2;
		int half_height=frame[1]/2;
		for(int i=0;i<frame[1];i++){
			pixels[bitwidth*(pixel_frame[1]-(i+1+frame.pos[1]))+(frame.pos[0]+half_width)*3+2]=color[0];
			pixels[bitwidth*(pixel_frame[1]-(i+1+frame.pos[1]))+(frame.pos[0]+half_width)*3+1]=color[1];
			pixels[bitwidth*(pixel_frame[1]-(i+1+frame.pos[1]))+(frame.pos[0]+half_width)*3]=color[2];
		}
		for(int i=0;i<frame[0];i++){
			pixels[bitwidth*(pixel_frame[1]-(1+frame.pos[1]+half_height))+(frame.pos[0]+i)*3+2]=color[0];
			pixels[bitwidth*(pixel_frame[1]-(1+frame.pos[1]+half_height))+(frame.pos[0]+i)*3+1]=color[1];
			pixels[bitwidth*(pixel_frame[1]-(1+frame.pos[1]+half_height))+(frame.pos[0]+i)*3]=color[2];
		}
		if(current_elements>0){
			grid.drawStructure(pixel_frame,bitwidth,pixels);
		}
		for(int i=0;i<boundarys.elements();i++){
			if(boundarys[i]!=nullptr){
				boundarys[i]->drawStructure(pixel_frame,bitwidth,pixels);
			}
		}
	
	}
	
	// Lösche und gebe alle Resourcen frei
	void clear() override {
		for(byte i=0;i<boundarys.elements();i++){
			if(boundarys[i]!=nullptr){
				boundarys[i]->clearComplete();
				delete boundarys[i];
				boundarys[i]=nullptr;
			}
		}
	}
	
	void clearComplete() override {
		removeGrid();
		clear();
	}
	
	GridQuadtree(int x,int y,int width,int height,int new_max_depth=5,int new_min_tilewidth=0,int new_min_tileheight=0,int newdepth=0){
		frame.pos[0]=x;
		frame.pos[1]=y;
		frame[0]=width;
		frame[1]=height;
		current_depth=newdepth;
		max_depth=new_max_depth;
		min_tilewidth=new_min_tilewidth;
		min_tileheight=new_min_tileheight;
		initGrid();
	}
	
	GridQuadtree(const rect<2> newframe,int new_max_depth=5,int new_min_tilewidth=0,int new_min_tileheight=0,int newdepth=0){
		frame.pos[0]=newframe.pos[0];
		frame.pos[1]=newframe.pos[1];
		frame[0]=newframe[0];
		frame[1]=newframe[1];
		current_depth=newdepth;
		max_depth=new_max_depth;
		min_tilewidth=new_min_tilewidth;
		min_tileheight=new_min_tileheight;
		initGrid();
	}
	
	~GridQuadtree(){}
	
};