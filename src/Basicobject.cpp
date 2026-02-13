class Basicobject{
	protected: 
	BITMAPINFO bitmapinfo;
	rect<2> object_rect;
	uint8_t* pixels=nullptr;
	uint8_t* trans_pixels=nullptr;
	PArray<StrucStatBase<Basicobject*>*> structures;
	int move_speed=1;
	int next_base_direction=-1;			// Für Move_type 6;
	position<int> destiny_pos={-1,-1};
	position<int> last_pos={-1,-1};
	int move_type=5;
	PArray<int> direction_path;
	max_uint path_counter=0;
	PArray<int> move_pattern;
	max_uint move_counter=0;
	max_uint object_id=0;
	PArray<PArray<int>> tileinfos;
	std::string tilename;
	byte mode_type=0;
	int tilewidth=0;
	int tileheight=0;
	
	
	virtual size_t getBitmapSize(){
		return getBitmapWidth()*getHeight();
	}
	
	virtual void createBitmapinfoSize(){
		bitmapinfo.bmiHeader.biSize=sizeof(bitmapinfo.bmiHeader);
		bitmapinfo.bmiHeader.biWidth=getWidth();
		bitmapinfo.bmiHeader.biHeight=getHeight();
		bitmapinfo.bmiHeader.biSizeImage=getBitmapSize();
	}
	
	virtual void createBitmapinfo(){
		bitmapinfo.bmiHeader.biSize=sizeof(bitmapinfo.bmiHeader);
		bitmapinfo.bmiHeader.biWidth=getWidth();
		bitmapinfo.bmiHeader.biHeight=getHeight();
		bitmapinfo.bmiHeader.biPlanes=1;
		bitmapinfo.bmiHeader.biBitCount=24;
		bitmapinfo.bmiHeader.biCompression=0;
		bitmapinfo.bmiHeader.biSizeImage=getBitmapSize();
		bitmapinfo.bmiHeader.biXPelsPerMeter=0;
		bitmapinfo.bmiHeader.biYPelsPerMeter=0;
		bitmapinfo.bmiHeader.biClrUsed=0;
		bitmapinfo.bmiHeader.biClrImportant=0;
	}
	
	void notify(const rect<2>& old_frame){
		for(int i=0;i<structures.elements();i++){
			if(structures[i]!=nullptr){
				structures[i]->notify(this,old_frame,1);
			}
		}
	}
	
	public:
	
	void setTiles(int tilewidth_new,int tileheight_new){
		tilewidth=tilewidth_new;
		tileheight=tileheight_new;
	}
	
	int getTileWidth(){
		return tilewidth;
	}
	
	int getTileHeight(){
		return tileheight;
	}
	
	void setMode(byte mode){
		mode_type=mode;
	}
	
	byte getModeType(){
		return mode_type;
	}
	
	// Neue API: akzeptiert std::vector<std::vector<int>>
void setTileInfos(const std::vector<std::vector<int>>& infos) {
    tileinfos.clear(); // vorhandene Inhalte entfernen (falls nötig)
    for (const auto& vec : infos) {
        PArray<int> current;
        for (int v : vec) {
            current.add(v);
        }
        tileinfos.add(current);
    }
}
	
	void setTileInfos(LinkedList<LinkedList<int>*>*& infos){
		PArray<int> current;
		LinkedList<LinkedList<int>*>* current_list=infos;
		LinkedList<int>* inside_list=nullptr;
		while(current_list!=nullptr){
			inside_list=current_list->element();
			while(inside_list!=nullptr){
				current.add(inside_list->element());
				inside_list=inside_list->next();
			}
			tileinfos.add(current);
			current_list=current_list->next();
			current.clear();
		}
	}
	
	PArray<PArray<int>>& getTileInfos(){
		return tileinfos;
	}
	
	virtual void setID(max_uint new_id){
		object_id=new_id;
	}
	
	virtual max_uint getID(){
		return object_id;
	}
	
	virtual void setMovePattern(int* new_pattern,max_uint length){
		if(new_pattern!=nullptr && length>0){
			move_pattern.create(length);
			for(max_uint i=0;i<length;i++){
				move_pattern.add(new_pattern[i]);
			}
		}
	}
	
	virtual int getNextPattern(){
		if(move_counter>=move_pattern.elements() || move_pattern.elements()==0){
			throw "Error: Basicobject macht mehr Bewegungen als es sollte!";
		}
		int direction=move_pattern[move_counter];
		move_counter++;
		return direction;
	}
	
	virtual position<int> getDestinyPosition(){
		return destiny_pos;
	}
	
	virtual void setDestinyPosition(position<int> new_pos){
		destiny_pos=new_pos;
	}
	
	virtual void setLastPosition(position<int> new_pos){
		last_pos=new_pos;
	}
	
	virtual position<int> getLastPosition(){
		return last_pos;
	}
	
	virtual void setNextDirection(int next_direction){
		next_base_direction=next_direction;
	}
	
	virtual int getNextDirection(){
		return next_base_direction;
	}
	
	template<typename struc_type>
	void setStructure(struc_type*& newstruc){
		if(newstruc!=nullptr){
			structures.add(newstruc);
		}
	}
	
	virtual BITMAPINFO* getBitmapinfo(){
		return &bitmapinfo;
	}
	
	virtual uint8_t* getPixels(){
		return pixels;
	}
	
	virtual uint8_t* getTransparency(){
		return trans_pixels;
	}
	
	virtual position<int> getPos() const {
		return object_rect.pos;
	}
	
	virtual rect<2> getRect() const {
		return object_rect;
	}
	
	virtual int getWidth(){
		return object_rect[0];
	}
	
	virtual int getHeight(){
		return object_rect[1];
	}
	
	virtual size_t getBitmapWidth(){
		return ((getWidth()*3+3)/4)*4;
	}
	
	virtual void setPixels(uint8_t* newpixels){
		for(size_t i=0;i<getBitmapSize();i++){
			pixels[i]=newpixels[i];
		}
	}
	
	virtual void setTileName(const char* newname){
		tilename=newname;
	}
	
	virtual std::string getTileName(){
		return tilename;
	}
	
	virtual void setTransPixels(uint8_t* newtranspixels){
		for(size_t i=0;i<object_rect[0]*object_rect[1];i++){
			trans_pixels[i]=newtranspixels[i];
		}
	}
	
	virtual void setSize(int width,int height){
		rect<2> old_frame=object_rect;
		object_rect[0]=width;
		object_rect[1]=height;
		notify(old_frame);
		if(pixels!=nullptr){
			delete[] pixels;
		}
		if(trans_pixels!=nullptr){
			delete[] trans_pixels;
		}
		size_t size=getBitmapSize();
		pixels=new uint8_t[size];
		trans_pixels=new uint8_t[object_rect[0]*object_rect[1]];
		for(int i=0;i<object_rect[0]*object_rect[1];i++){
			trans_pixels[i]=255;
		}
		for(size_t i=0;i<size;i++){
			pixels[i]=0;
		}
		createBitmapinfo();
	}
	
	virtual void setPos(int xpos,int ypos){
		rect<2> old_frame=object_rect;
		object_rect.pos[0]=xpos;
		object_rect.pos[1]=ypos;
		notify(old_frame);
	}
	
	bool inRange(int value,int range,int limit){
		if((limit-range)<0 || (limit+range)>255){
			return false;
		}
		if((value>=(limit-range)) && (value<=(limit+range)) && (value>=0)){
			return true;
		}
		return false;
	}
	
	void setSpeed(int newspeed){
		move_speed=newspeed;
	}
	
	int getSpeed(){
		return move_speed;
	}
	
	void setMoveType(int newtype){
		move_type=newtype;
	}
	
	int getMoveType(){
		return move_type;
	}
	
	int getCurrentPathDirection(){
		if((path_counter)<(direction_path.elements()-1)){
			path_counter++;
			return direction_path[path_counter-1];
		}
		path_counter=0;
		return direction_path[direction_path.elements()-1];
	}
	
	int getLastPathDirection(){
		if(path_counter<1){
			return direction_path[direction_path.elements()-1];
		}
		return direction_path[path_counter-1];
	}
	
	void changePathPattern(int new_direction){
		if((path_counter)<=(direction_path.elements()-1) && (direction_path.elements()>0)){
			if((path_counter)<1){
				direction_path[direction_path.elements()-1]=new_direction;
				return;
			}
			direction_path[path_counter-1]=new_direction;
		}
	}
	
	void setPathDirectionMove(size_t index,int direction){
		if(index<direction_path.elements()){
			direction_path[index]=direction;
		}
	}
	
	void addPathDirection(int direction){
		if(direction<=3 && direction>=0){
			direction_path.add(direction);
		}
	}
	
	rect<2> move_rect(int direction){
		rect<2> moving;
		if(direction<0 && direction>3){
			return moving;
		}
		moving[0]=object_rect[0];
		moving[1]=object_rect[1];
		moving.pos[0]=object_rect.pos[0];
		moving.pos[1]=object_rect.pos[1];
		switch(direction){
			case 0:
				//Links
				moving.pos[0]-=move_speed;
				break;
			case 1:
				//Rechts
				moving.pos[0]+=move_speed;
				break;
			case 2:
				//Oben
				moving.pos[1]-=move_speed;
				break;
			case 3:
				//Unten
				moving.pos[1]+=move_speed;
				break;
		}
		return moving;
	}
	
	bool canMove(){
		if(move_speed>0){
			return true;
		}
		return false;
	}
	
	void move(int direction){
		rect<2> old_frame=object_rect;
		switch(direction){
			case 0:
				//Links
				object_rect.pos[0]-=move_speed;
				break;
			case 1:
				//Rechts
				object_rect.pos[0]+=move_speed;
				break;
			case 2:
				//Oben
				object_rect.pos[1]-=move_speed;
				break;
			case 3:
				//Unten
				object_rect.pos[1]+=move_speed;
				break;
		}
		notify(old_frame);
	}
	
	void filterPixels(int r,int g,int b,int range){
		int bitwidth=getBitmapWidth();
		for(int i=0;i<getHeight();i++){
			for(int j=0;j<getWidth();j++){
				if(inRange(pixels[bitwidth*(object_rect[1]-(i+1))+j*3+2],range,b) &&
					inRange(pixels[bitwidth*(object_rect[1]-(i+1))+j*3+1],range,g) &&
					inRange(pixels[bitwidth*(object_rect[1]-(i+1))+j*3],range,r)){
						trans_pixels[object_rect[0]*(object_rect[1]-(i+1))+j]=0;
					}
			}
		}
	}
	
	void setPixelsNull(){
		if(pixels!=nullptr){
			for(size_t i=0;i<getBitmapSize();i++){
				pixels[i]=0;
			}
		}
		if(trans_pixels!=nullptr){
			for(size_t i=0;i<getHeight()*getWidth();i++){
				trans_pixels[i]=0;
			}
		}
	}
	
	virtual void loadPixels(uint8_t* newpixels,int newwidth,int newheight,uint8_t* transparent=nullptr){
		setSize(newwidth,newheight);
		if(transparent!=nullptr){
			for(int i=0;i<object_rect[0]*object_rect[1];i++){
				trans_pixels[i]=transparent[i];
			}
		}
		int bitmapwidth=getBitmapWidth();
		for(int i=0;i<object_rect[1];i++){
			for(int j=0;j<object_rect[0];j++){
				for(byte k=0;k<3;k++){
					pixels[bitmapwidth*i+j*3+k]=newpixels[object_rect[0]*3*i+j*3+k];
				}
			}
		}
	}
	
	virtual void paint(HDC& hdc){
		StretchDIBits(hdc,object_rect.pos[0],object_rect.pos[1],object_rect[0],object_rect[1],0,0,object_rect[0],object_rect[1],pixels,&bitmapinfo,DIB_RGB_COLORS,SRCCOPY);
	}
	
	void clearPixelsMode(){
		if(pixels!=nullptr){
			delete pixels;
			pixels=nullptr;
		}
		if(trans_pixels!=nullptr){
			delete trans_pixels;
			trans_pixels=nullptr;
		}
	}
	
	Basicobject(){}
	
	virtual ~Basicobject(){
		if(pixels!=nullptr){
			delete[] pixels;
		}
		if(trans_pixels!=nullptr){
			delete[] trans_pixels;
		}
		direction_path.clear();
		move_pattern.clear();
		structures.clear();
	}
};