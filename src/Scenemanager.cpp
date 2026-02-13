struct ObjectInfo{
	int xpos=0;
	int ypos=0;
	int speed=0;
	std::string tilename;
	int mode=0;
	int tilewidth=0;
	int tileheight=0;
	PArray<PArray<int>> tiles;
	int move_type=0;
	//#########################//
	PArray<int> move_pattern;
	/*
								falls copy_bool==1											falls move_type==4			falls Tile_info==1
	Tile_info;Copy_bool;(copy_amount;)random_pos;Name_der_Image_file.type;x;y;move_type;(path_filename;)speed;(tilewidth;tileheight;{{Tile1},{Tile2}})
	*/	
	
	void addMove(int direction){
		move_pattern.add(direction);
	}
	
	std::string toString(){
		std::string info_string;
		info_string+="1";
		info_string+=";0;0;";
		info_string+=tilename;
		info_string+=";";
		info_string+=std::to_string(xpos);
		info_string+=";";
		info_string+=std::to_string(ypos);
		info_string+=";";
		info_string+=std::to_string(move_type);
		info_string+=";";
		info_string+=std::to_string(speed);
		if(mode==1){
			info_string+=";";
			info_string+=std::to_string(tilewidth);
			info_string+=";";
			info_string+=std::to_string(tileheight);
			info_string+=";{";
			for(max_uint i=0;i<tiles.elements();i++){
				info_string+="{";
				for(max_uint k=0;k<tiles[i].elements();k++){
					info_string+=std::to_string(tiles[i][k]);
					if(k!=(tiles[i].elements()-1)){
						info_string+=",";
					}
				}
				info_string+="}";
				if(i!=(tiles.elements()-1)){
					info_string+=",";
				}
			}
			info_string+="}";
		}
		info_string+=";";
		info_string+=std::to_string(move_pattern.elements());
		for(max_uint i=0;i<move_pattern.elements();i++){
			info_string+=";";
			info_string+=std::to_string(move_pattern[i]);
		}
		return info_string;
	}
	
	void append(std::ofstream& out){
		out << std::to_string(mode);
		out << ";0;0;";
		out << tilename;
		out << ";";
		out << std::to_string(xpos);
		out << ";";
		out << std::to_string(ypos);
		out << ";";
		out << std::to_string(move_type);
		out << ";";
		out << std::to_string(speed);
		if(mode==1){
			out << ";";
			out << std::to_string(tilewidth);
			out << ";";
			out << std::to_string(tileheight);
			out << ";{";
			for(max_uint i=0;i<tiles.elements();i++){
				out << "{";
				for(max_uint k=0;k<tiles[i].elements();k++){
					out << std::to_string(tiles[i][k]);
					if(k!=(tiles[i].elements()-1)){
						out << ",";
					}
				}
				out << "}";
				if(i!=(tiles.elements()-1)){
					out << ",";
				}
			}
			out << "}";
		}
		out << ";";
		out << std::to_string(move_pattern.elements());
		for(max_uint i=0;i<move_pattern.elements();i++){
			out << ";";
			out << std::to_string(move_pattern[i]);
		}
	}
	
	void clear(){
		for(int i=0;i<tiles.elements();i++){
			tiles[i].clear();
		}
		tiles.clear();
		move_pattern.clear();
	}
	
};
/*

		Werte für move_type:
		0 -> Links
		1 -> Rechts
		2 -> Oben
		3 -> Unten
		4 -> Mit eigene Custom Pfad
		5 -> Random von 0-3
		
		
		Werte für Random_pos:
		0 -> alle dieselbe x und y Position
		1 -> Random x und y Position
		2 -> nah beiander
		3 -> relativ gut verteilt
		
		Struktur für 0:
		
		Beispiel:
		0;0;Name_der_Image_file.type;x;y;5;1
		0;1;1;1;Name_der_Image_file.type;x;y;5;speed	
		
		Struktur für 1:
		1;0;Name_der_Image_file.type;x;y;move_type;speed;tilewidth;tileheight;{{1,3},{4,4}}
		1;1;1;1;Name_der_Image_file.type;x;y;move_type;speed;tilewidth;tileheight;{{1,3},{4,4}}
*/
class Scenemanager{
	
	PArray<Basicobject*> objects;
	PArray<ObjectInfo> infos;
	
	public:
	
	void add(Basicobject* object,max_uint movements){
		ObjectInfo info;
		position<int> pos=object->getPos();
		if(movements!=0){
			info.move_pattern.create(movements);
		}
		info.xpos=pos[0];
		info.ypos=pos[1];
		info.speed=object->getSpeed();
		info.tilename=object->getTileName();
		info.mode=object->getModeType();
		info.tilewidth=object->getTileWidth();
		info.tileheight=object->getTileHeight();
		info.move_type=7;
		info.tiles=object->getTileInfos();
		object->setID(objects.elements()+1);
		objects.add(object);
		infos.add(info);
	}
	
	void object_move(Basicobject* object,int direction){
		max_uint id=object->getID();
		if(id!=0){
			infos[id-1].addMove(direction);
		}
	}
	
	bool create(char* file){
		// Prüfen, ob die Datei existiert
		std::ifstream checkFile(file);
		if (checkFile.good()) {
			checkFile.close();
			// Datei existiert – löschen
			if (std::remove(file) != 0) {
				// Fehler beim Löschen
				return false;
			}
		}
		std::ofstream out(file, std::ios::app);
		if(!out){ // prüfen ob geöffnet / erstellt
			return false;
		}
		for(max_uint i=0;i<objects.elements()-1;i++){
			infos[i].append(out);
			out << '\n';
		}
		infos[objects.elements()-1].append(out);
		out.close();
		return out.good(); // true wenn kein Fehler beim Schreiben

	}
	
	void clear(){
		for(max_uint i=0;i<infos.elements();i++){
			infos[i].clear();
		}
		infos.clear();
		objects.clear();
	}
};