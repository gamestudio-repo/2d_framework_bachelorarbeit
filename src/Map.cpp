class Map:public Basicobject{
	int tilewidth=0;
	int tileheight=0;
	int tile_columns=0;
	int tile_rows=0;
	int tile_per_row=0;
	Basicobject* map_tileset=nullptr;

	void split_line(char* line,int* array){
		int counter=0;
		int array_counter=0;
		while(line[counter]!='\0'){
			if(line[counter]=='x'){
				array[array_counter]=stringToInt(line,counter);
				array_counter++;
				counter++;
				break;
			}
			counter++;
		}
		array[array_counter]=stringToInt(line,-1,counter);
	}
	
	void drawPart(int tile_x,int tile_y,int xpos,int ypos){
		uint8_t* maptile_pixels=map_tileset->getPixels();
		size_t maptile_bitwidth=map_tileset->getBitmapWidth();
		rect<2> maptile_rect=map_tileset->getRect();
		size_t this_bitwidth=getBitmapWidth();
		for(int i=0;i<tileheight;i++){
			for(int j=0;j<tilewidth;j++){
				for(int k=0;k<3;k++){
					pixels[this_bitwidth*(object_rect[1]-(i+1+ypos))+(j+xpos)*3+k]=maptile_pixels[maptile_bitwidth*(maptile_rect[1]-(i+1+tile_y))+(j+tile_x)*3+k];
				}
			}
		}
	}
	
	void loadTileLine(char* tile_info,int current_ypos){
		int tile_pos[2];
		int tile_id=0;
		int current_xpos=0;
		int counter=0;
		int string_header=0;
		while(tile_info[counter]!='\0' && (current_xpos<tile_columns)){
			if(tile_info[counter]==','){
				tile_id=stringToInt(tile_info,counter-string_header,string_header);
				tile_pos[0]=(tile_id%tile_per_row);
				tile_pos[1]=(tile_id/tile_per_row);
				drawPart(tile_pos[0]*tilewidth,tile_pos[1]*tileheight,current_xpos*tilewidth,current_ypos*tileheight);
				string_header=counter+1;
				current_xpos++;
			}
			counter++;
		}
		if(current_xpos==tile_columns){
			return;
		}
		tile_id=stringToInt(tile_info,-1,string_header);
		tile_pos[0]=(tile_id%tile_per_row);
		tile_pos[1]=(tile_id/tile_per_row);
		drawPart(tile_pos[0]*tilewidth,tile_pos[1]*tileheight,current_xpos*tilewidth,current_ypos*tileheight);
	}
	
	void loadTiles(PFile& file){
		int current_ypos=0;
		char* line=file.readLine();
		if(line[0]=='#'){
			line=file.readLine();
			while(line[0]!='#'){
				if(current_ypos<tile_rows){
					loadTileLine(line,current_ypos);
					current_ypos++;
				}
				line=file.readLine();
			}
			return;
		}
		string error="MAP Error: Tiles von der Map Datei '";
		error+=file.getFilename();
		error+="' konnte nicht geladen werden!";
		file.close();
		throw error;
	}

	public:

	bool load(const char* filename){
		PFile file;
		if(file.load(filename)){
			int info[2];
			split_line(file.readLine(),info);
			tilewidth=info[0];
			tileheight=info[1];
			split_line(file.readLine(),info);
			tile_columns=info[0];
			tile_rows=info[1];
			Imageloader loader;
			map_tileset=loader.load<Basicobject>(file.readLine());
			tile_per_row=(map_tileset->getWidth()/tilewidth);
			setSize(tilewidth*tile_columns,tileheight*tile_rows);
			loadTiles(file);
			delete map_tileset;
			map_tileset=nullptr;
			file.close();
			return true;
		}
		return false;
	}
	
	~Map() override {
		if(map_tileset!=nullptr){
			delete map_tileset;
			map_tileset=nullptr;
		}
	}
	
};