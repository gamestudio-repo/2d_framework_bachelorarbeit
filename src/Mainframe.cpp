#include <random>
#include <limits>
#include <queue>
#include <cstdint>
// Hilfsstruktur für Paar-Hashing
struct PosHash {
    std::size_t operator()(const std::pair<int,int>& p) const noexcept {
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
    }
};

struct Command{
	max_uint time=0;
	max_uint command_id=0;
	string variable;
};

//operation: 0=Nix/Error; 1=Insert; 2=Remove; 3=Random
//Mode (Remove): 0=random; 1=erstes Element; 2=Letzes Element; 3= Fix definiertes
//Mode (Insert): 0=Random; 1= Fix definiertes
//id nur bei Remove operation
//width,height,x,y nur bei insert operation
struct Operation{
	uint8_t operation=0;
	uint8_t mode=0;
	int width=0;
	int height=0;
	int x=0;
	int y=0;
	max_uint id=0;
};

class Mainframe{
	max_uint operation_amount=0;
	std::string scene_name="";
	bool phase_2=false;
	//####################################
	bool isReady=false;
	//####################################
	PArray<Operation> operation_array;
	uint8_t set_insert=0;
	//####################################
	uint8_t gui_mode=1;
	PArray<Basicobject*> backup_array;
	Scenemanager manager;
	//###########################
	bool collision_check_set=false;
	max_uint check_collisions=0;
	//###########################
	unsigned long long check_updates=0;
	unsigned long long current_updates=0;
	bool update_check_set=false;
	//###########################
	Timer timer;
	bool timer_set=false;
	max_uint timer_set_time=0;
	string time_name="Sekunden";
	max_uint timer_ms_converter=1000;			//es ist in ms...für sekunden muss man nicht geteilt sondern malen
	max_uint timer_ms_stepper=1000;				
	double timer_start_value=1000;
	double current_timer_ms=timer_start_value;	// für microsecond z.B. muss man 0.001 machen;
	double timer_adder=1;
	//###########################
	string current_scene="";
	string single_object_path="";
	string second_bench_path="";
	//###########################
	bool hasCommands=false;
	max_uint next_command_time=0;
	LinkedList<Command> command_list;
	LinkedList<Command>* current_command_list=nullptr;
	//###########################
	int list_start=0;
	Map* map=nullptr;
	bool map_visible=false;
	bool collision_visible=false;
	//############################
	max_uint currentobjects=0;
	PArray<Basicobject*> list;  //Bruteforce
	PArray<StrucStatBase<Basicobject*>*> structures;
	bool draw_struc=false;
	//########################//
	rect<2> frame;
	uint8_t* pixels=nullptr;
	int pixelwidth=0;
	BITMAPINFO bitmapinfo;
	size_t pixel_size=0;
	//#######Für /Compare################//
	Semaphore sema_paint{1};
	//#######Für /Compare################//
	Semaphore sema_compare{1};
	//###################################//
		bool drawing=true;
	//###################################//
	max_uint list_element_size=2000000;
	max_uint operation_element_size=1000000;
	//###################################//
	PArray<Benchmark> benchmarks={
		{"BruteForce","Query"},
		{"BruteForce","Insert"},
		{"BruteForce","Remove"},
		{"BruteForce","Build"},
		{"BruteForce","Update"},
		{"BruteForce","Notify"},
	};
	int max_strucs_list = 0;
	int struct_queue_counter=0;
	PArray<StrucStatBase<Basicobject*>*> struct_queue;
	
	void initOperationArray(){
		operation_array.create(operation_element_size);
	}
	
	void initList(){
		list.create(list_element_size);
	}
	
	virtual void createBitmapinfo(){
		bitmapinfo.bmiHeader.biSize=sizeof(bitmapinfo.bmiHeader);
		bitmapinfo.bmiHeader.biWidth=frame[0];
		bitmapinfo.bmiHeader.biHeight=frame[1];
		bitmapinfo.bmiHeader.biPlanes=1;
		bitmapinfo.bmiHeader.biBitCount=24;
		bitmapinfo.bmiHeader.biCompression=0;
		bitmapinfo.bmiHeader.biSizeImage=pixel_size;
		bitmapinfo.bmiHeader.biXPelsPerMeter=0;
		bitmapinfo.bmiHeader.biYPelsPerMeter=0;
		bitmapinfo.bmiHeader.biClrUsed=0;
		bitmapinfo.bmiHeader.biClrImportant=0;
	}
	
	void createPixels(){
		if(pixels!=nullptr){
			delete[] pixels;
		}
		pixel_size=((frame[0]*3+3)/4)*4*frame[1];
		pixels=new uint8_t[pixel_size];
	}
	
	virtual size_t getBitmapWidth(){
		return ((frame[0]*3+3)/4)*4;
	}
	
	void inFrame_fix(rect<2>& rect){
		if(rect.pos[0]<=(frame.pos[0]+frame[0])&&
			rect.pos[1]<=(frame.pos[1]+frame[1])){
			if(rect.pos[0]<0){
				rect[0]=rect[0]+rect.pos[0];
				rect.pos[0]=0;
			}
			if(rect.pos[1]<0){
				rect[1]=rect[1]+rect.pos[1];
				rect.pos[1]=0;
			}
			if(rect.pos[0]+rect[0]>(frame.pos[0]+frame[0])){
				rect[0]=(frame.pos[0]+frame[0])-rect.pos[0];
			}
			if(rect.pos[1]+rect[1]>(frame.pos[1]+frame[1])){
				rect[1]=(frame.pos[1]+frame[1])-rect.pos[1];
			}
		}
	}
	
	void drawObject(Basicobject* object,size_t pixelwidth){
		byte* drawpixels=object->getPixels();
		size_t drawbitwidth=object->getBitmapWidth();
		rect<2> object_rect=object->getRect();
		inFrame_fix(object_rect);
		byte* transpixels=object->getTransparency();
		int transvalue=0;
		for(int i=0;i<object_rect[1];i++){
			for(int j=0;j<object_rect[0];j++){
				transvalue=transpixels[object_rect[0]*(object_rect[1]-(1+i))+j];
				for(byte k=0;k<3;k++){
					pixels[pixelwidth*(frame[1]-(1+i+object_rect.pos[1]))+(object_rect.pos[0]+j)*3+k]=((transvalue*drawpixels[drawbitwidth*(object_rect[1]-(1+i))+j*3+k])/255)
																									+((pixels[pixelwidth*(frame[1]-(1+i+object_rect.pos[1]))+(object_rect.pos[0]+j)*3+k]
																									*(255-transvalue))/255);
				}
			}
		}
	}
	
	bool directoryExists(const char* path) {
		DWORD attribs = GetFileAttributesA(path);
		return (attribs != INVALID_FILE_ATTRIBUTES && (attribs & FILE_ATTRIBUTE_DIRECTORY));
	}
	
	bool fileExists(const char* path){
		DWORD attribs = GetFileAttributesA(path);
		return (attribs != INVALID_FILE_ATTRIBUTES && !(attribs & FILE_ATTRIBUTE_DIRECTORY));
	}
	
	void drawPixelPart(uint8_t* object_pixels,uint8_t* object_transparent,int object_width,int object_height,int object_bitwidth,Basicobject* tile_object,int tile_x,int tile_y,int xpos,int ypos,int tilewidth,int tileheight){
		tile_x=tile_x*tilewidth;
		tile_y=tile_y*tileheight;
		xpos=xpos*tilewidth;
		ypos=ypos*tileheight;
		uint8_t* maptile_pixels=tile_object->getPixels();
		size_t maptile_bitwidth=tile_object->getBitmapWidth();
		rect<2> maptile_rect=tile_object->getRect();
		uint8_t* maptile_transparent=tile_object->getTransparency();
		for(int i=0;i<tileheight;i++){
			for(int j=0;j<tilewidth;j++){
				for(int k=0;k<3;k++){
					object_pixels[object_bitwidth*(object_height-(i+1+ypos))+(j+xpos)*3+k]=maptile_pixels[maptile_bitwidth*(maptile_rect[1]-(i+1+tile_y))+(j+tile_x)*3+k];
				}
				object_transparent[object_width*(object_height-(i+1+ypos))+(j+xpos)]=maptile_transparent[maptile_rect[0]*(maptile_rect[1]-(i+1+tile_y))+(j+tile_x)];
			}
		}
	}
	
	// Abstand zwischen zwei Punkten
	int distance(position<int> a_pos,position<int> b_pos) {
		int dx = a_pos[0] - b_pos[0];
		int dy = a_pos[1] - b_pos[1];
		return sqrt(dx * dx + dy * dy);
	}
	
	// Abstand zwischen zwei Punkten
	int distance(rect<2> a,rect<2> b) {
		position<int> a_pos=a.pos;
		position<int> b_pos=b.pos;
		int dx = a_pos[0] - b_pos[0];
		int dy = a_pos[1] - b_pos[1];
		return sqrt(dx * dx + dy * dy);
	}
	
	// Platzierungsfunktion mit adaptiver Mindestdistanz
	// Signatur wie gewünscht beibehalten
	bool getPlaceOptimized(position<int>& pos,Basicobject*& object,
						int step_on = true,
						int step = 100)
	{
		// 1) Vorbereitungen
		// ------------------
		// Rechteck & Frame-Daten
		rect<2> object_rect = object->getRect();
		int w     = object_rect[0];
		int h     = object_rect[1];
		int frameW = frame[0];
		int frameH = frame[1];

		// Aktuelle Startposition merken
		position<int> origPos = object->getPos();

		// Alle bereits eingefügten Objekte im Quadtree abfragen
		PArray<Basicobject*> existing;
		structures[0]->query(frame, existing);
		int countExisting = existing.elements() - list_start;

		// Wenn noch keine Objekte (ab list_start), direkt (0,0) setzen
		if (countExisting <= 0) {
			pos[0]=0;
			pos[1]=0;
			return true;
		}

		// 2) Random Sampling über den gesamten Frame
		//    mit "step" als Anzahl der Samples
		// ----------------------------------------
		std::mt19937                     rng{ std::random_device{}() };
		std::uniform_int_distribution<> dx(0, frameW - w);
		std::uniform_int_distribution<> dy(0, frameH - h);

		int bestMinDist = -1;
		position<int> bestPos = origPos;

		for (int i = 0; i < step; ++i) {
			// Zufällige Kandidaten-Position
			int x = dx(rng);
			int y = dy(rng);

			object_rect.pos[0] = x;
			object_rect.pos[1] = y;

			// 3) Nur lokale Objekte abfragen:
			// erweitertes Rechteck um bestMinDist, um nur wirklich nahe Objekte zu holen
			int margin = (bestMinDist >= 0)
						 ? bestMinDist
						 : std::max(frameW, frameH);

			rect<2> queryRect;
			queryRect.pos[0] = std::max(0, x - margin);
			queryRect.pos[1] = std::max(0, y - margin);
			queryRect[0]     = std::min(frameW - queryRect.pos[0], w + 2*margin);
			queryRect[1]     = std::min(frameH - queryRect.pos[1], h + 2*margin);

			PArray<Basicobject*> nearby;
			structures[0]->query(queryRect, nearby);

			// 4) Minimalabstand zu den Nachbarn bestimmen
			int minDist = std::numeric_limits<int>::max();
			for (int j = list_start; j < nearby.elements(); ++j) {
				if (nearby[j] == object) continue;
				int d = distance(object_rect, nearby[j]->getRect());
				if (d < minDist) {
					minDist = d;
					// Early-Exit, wenn schon schlechter als aktueller Bestwert
					if (minDist <= bestMinDist) 
						break;
				}
			}

			// 5) Neuen besten Platz merken
			if (minDist > bestMinDist) {
				bestMinDist = minDist;
				bestPos[0]  = x;
				bestPos[1]  = y;
			}
		}

		// 6) Ergebnis übernehmen
		pos[0]=bestPos[0];
		pos[1]=bestPos[1];

		// 7) Rückgabe: true, wenn Position verändert wurde
		return !(bestPos[0] == origPos[0] && bestPos[1] == origPos[1]);
	}

	// Platzierungsfunktion mit adaptiver Mindestdistanz
	// Signatur wie gewünscht beibehalten
	bool placeOptimized(Basicobject*& object,
						int step_on = true,
						int step = 100)
	{
		// 1) Vorbereitungen
		// ------------------
		// Rechteck & Frame-Daten
		rect<2> object_rect = object->getRect();
		int w     = object_rect[0];
		int h     = object_rect[1];
		int frameW = frame[0];
		int frameH = frame[1];

		// Aktuelle Startposition merken
		position<int> origPos = object->getPos();

		// Alle bereits eingefügten Objekte im Quadtree abfragen
		PArray<Basicobject*> existing;
		structures[0]->query(frame, existing);
		int countExisting = existing.elements() - list_start;

		// Wenn noch keine Objekte (ab list_start), direkt (0,0) setzen
		if (countExisting <= 0) {
			object->setPos(0, 0);
			return true;
		}

		// 2) Random Sampling über den gesamten Frame
		//    mit "step" als Anzahl der Samples
		// ----------------------------------------
		std::mt19937                     rng{ std::random_device{}() };
		std::uniform_int_distribution<> dx(0, frameW - w);
		std::uniform_int_distribution<> dy(0, frameH - h);

		int bestMinDist = -1;
		position<int> bestPos = origPos;

		for (int i = 0; i < step; ++i) {
			// Zufällige Kandidaten-Position
			int x = dx(rng);
			int y = dy(rng);

			object_rect.pos[0] = x;
			object_rect.pos[1] = y;

			// 3) Nur lokale Objekte abfragen:
			// erweitertes Rechteck um bestMinDist, um nur wirklich nahe Objekte zu holen
			int margin = (bestMinDist >= 0)
						 ? bestMinDist
						 : std::max(frameW, frameH);

			rect<2> queryRect;
			queryRect.pos[0] = std::max(0, x - margin);
			queryRect.pos[1] = std::max(0, y - margin);
			queryRect[0]     = std::min(frameW - queryRect.pos[0], w + 2*margin);
			queryRect[1]     = std::min(frameH - queryRect.pos[1], h + 2*margin);

			PArray<Basicobject*> nearby;
			structures[0]->query(queryRect, nearby);

			// 4) Minimalabstand zu den Nachbarn bestimmen
			int minDist = std::numeric_limits<int>::max();
			for (int j = list_start; j < nearby.elements(); ++j) {
				if (nearby[j] == object) continue;
				int d = distance(object_rect, nearby[j]->getRect());
				if (d < minDist) {
					minDist = d;
					// Early-Exit, wenn schon schlechter als aktueller Bestwert
					if (minDist <= bestMinDist) 
						break;
				}
			}

			// 5) Neuen besten Platz merken
			if (minDist > bestMinDist) {
				bestMinDist = minDist;
				bestPos[0]  = x;
				bestPos[1]  = y;
			}
		}

		// 6) Ergebnis übernehmen
		object->setPos(bestPos[0], bestPos[1]);

		// 7) Rückgabe: true, wenn Position verändert wurde
		return !(bestPos[0] == origPos[0] && bestPos[1] == origPos[1]);
	}


	bool getPlaceStacked(position<int>& pos,Basicobject*& object,
					  bool step_on = true,
					  int step = 10)
	{
		// 1) Rechteck, Ursprung und Schrittweite ermitteln
		rect<2> objRect    = object->getRect();
		int w              = objRect[0];
		int h              = objRect[1];
		position<int> startPos = objRect.pos;

		int frameW = frame[0];
		int frameH = frame[1];

		int stepX = step_on ? step : w + 1;
		int stepY = step_on ? step : h + 1;

		// 2) Queue für BFS initialisieren
		std::queue<std::pair<int,int>>   q;
		std::unordered_set<std::pair<int,int>, PosHash> visited;
		q.emplace(startPos[0], startPos[1]);
		visited.emplace(startPos[0], startPos[1]);

		// 3) BFS-Schleife: ringförmig immer näher ans Startzentrum
		static const int dirs[4][2] = {
			{ 1,  0},
			{-1,  0},
			{ 0,  1},
			{ 0, -1}
		};

		while (!q.empty()) {
			auto [x, y] = q.front();
			q.pop();

			// A) In-Bounds prüfen
			if (x < 0 || y < 0 || x + w > frameW || y + h > frameH)
				continue;

			// B) Test-Rechteck setzen und lokale Objekte abfragen
			rect<2> testRect;
			testRect.pos[0] = x;
			testRect.pos[1] = y;
			testRect[0]      = w;
			testRect[1]      = h;

			PArray<Basicobject*> local;
			structures[0]->query(testRect, local);

			// C) Intersektion prüfen
			bool collision = false;
			for (int i = list_start; i < local.elements(); ++i) {
				if (local[i] == object) 
					continue;
				if (testRect.intersect(local[i]->getRect())) {
					collision = true;
					break;
				}
			}

			// D) Wenn kein Treffer: Platz gefunden
			if (!collision) {
				pos[0]=x;
				pos[1]=y;
				return true;
			}

			// E) Nachbarn in vier Richtungen hinzufügen
			for (auto& d : dirs) {
				int nx = x + d[0] * stepX;
				int ny = y + d[1] * stepY;
				auto key = std::make_pair(nx, ny);
				if (visited.insert(key).second) {
					q.emplace(nx, ny);
				}
			}
		}

		// Kein freier Platz gefunden
		return false;
	}
	
	bool placeStacked(Basicobject*& object,
					  bool step_on = true,
					  int step = 10)
	{
		// 1) Rechteck, Ursprung und Schrittweite ermitteln
		rect<2> objRect    = object->getRect();
		int w              = objRect[0];
		int h              = objRect[1];
		position<int> startPos = objRect.pos;

		int frameW = frame[0];
		int frameH = frame[1];

		int stepX = step_on ? step : w + 1;
		int stepY = step_on ? step : h + 1;

		// 2) Queue für BFS initialisieren
		std::queue<std::pair<int,int>>   q;
		std::unordered_set<std::pair<int,int>, PosHash> visited;
		q.emplace(startPos[0], startPos[1]);
		visited.emplace(startPos[0], startPos[1]);

		// 3) BFS-Schleife: ringförmig immer näher ans Startzentrum
		static const int dirs[4][2] = {
			{ 1,  0},
			{-1,  0},
			{ 0,  1},
			{ 0, -1}
		};

		while (!q.empty()) {
			auto [x, y] = q.front();
			q.pop();

			// A) In-Bounds prüfen
			if (x < 0 || y < 0 || x + w > frameW || y + h > frameH)
				continue;

			// B) Test-Rechteck setzen und lokale Objekte abfragen
			rect<2> testRect;
			testRect.pos[0] = x;
			testRect.pos[1] = y;
			testRect[0]      = w;
			testRect[1]      = h;

			PArray<Basicobject*> local;
			structures[0]->query(testRect, local);

			// C) Intersektion prüfen
			bool collision = false;
			for (int i = list_start; i < local.elements(); ++i) {
				if (local[i] == object) 
					continue;
				if (testRect.intersect(local[i]->getRect())) {
					collision = true;
					break;
				}
			}

			// D) Wenn kein Treffer: Platz gefunden
			if (!collision) {
				object->setPos(x, y);
				return true;
			}

			// E) Nachbarn in vier Richtungen hinzufügen
			for (auto& d : dirs) {
				int nx = x + d[0] * stepX;
				int ny = y + d[1] * stepY;
				auto key = std::make_pair(nx, ny);
				if (visited.insert(key).second) {
					q.emplace(nx, ny);
				}
			}
		}

		// Kein freier Platz gefunden
		return false;
	}
	
	bool getPlacePosition(position<int>& pos,Basicobject* placeholder_object,byte random_pos){
		/*
		0 -> alle dieselbe x und y Position
		1 -> Random x und y Position
		2 -> nah beiander
		3 -> mäglichst weit weg
		*/
		switch(random_pos){
			case 0:
				return false;
			case 1:
				return getRandomPosition(pos,placeholder_object);
			case 2:
				return getPlaceStacked(pos,placeholder_object,false);
			case 3:
				return getPlaceOptimized(pos,placeholder_object,false);
		}
		return false;
	}
	
	bool placePosition(Basicobject* placeholder_object,byte random_pos){
		/*
		0 -> alle dieselbe x und y Position
		1 -> Random x und y Position
		2 -> nah beiander
		3 -> mäglichst weit weg
		*/
		switch(random_pos){
			case 0:
				return add(placeholder_object);
			case 1:
				if(randomPosition(placeholder_object)){
					return add(placeholder_object);
				}
				else{
					delete placeholder_object;
				}
				return false;
			case 2:
				if(placeStacked(placeholder_object,false)){
					return add(placeholder_object);
				}
				else{
					delete placeholder_object;
				}
				return false;
			case 3:
				if(placeOptimized(placeholder_object,false)){
					return add(placeholder_object);
				}
				else{
					delete placeholder_object;
				}
				return false;
		}
		return false;
	}
	
	bool getRandomPosition(position<int>& pos,Basicobject* object){
		size_t tries = 10000;
		rect<2> object_rect = object->getRect();
		int x = 0, y = 0;
		PArray<Basicobject*> collisions; // nur einmal anlegen

		// Zufalls-Engine initialisieren
		static std::mt19937 rng(std::random_device{}());
		std::uniform_int_distribution<int> distX(0, frame[0] - 1);
		std::uniform_int_distribution<int> distY(0, frame[1] - 1);

		while (tries-- > 0) {
			collisions.clear(); // Liste zurücksetzen

			// Zufallswerte erzeugen
			x = distX(rng);
			y = distY(rng);

			object_rect.pos[0] = x;
			object_rect.pos[1] = y;

			if (!out_of_range(object_rect)) {
				structures[0]->query(object_rect, collisions);
				if (collisions.elements() == 0) {
					pos[0]=x;
					pos[1]=y;
					return true;
				}
			}
		}
		return false;
	}
	
	bool randomPosition(Basicobject* object) {
		size_t tries = 10000;
		rect<2> object_rect = object->getRect();
		int x = 0, y = 0;
		PArray<Basicobject*> collisions; // nur einmal anlegen

		// Zufalls-Engine initialisieren
		static std::mt19937 rng(std::random_device{}());
		std::uniform_int_distribution<int> distX(0, frame[0] - 1);
		std::uniform_int_distribution<int> distY(0, frame[1] - 1);

		while (tries-- > 0) {
			collisions.clear(); // Liste zurücksetzen

			// Zufallswerte erzeugen
			x = distX(rng);
			y = distY(rng);

			object_rect.pos[0] = x;
			object_rect.pos[1] = y;

			if (!out_of_range(object_rect)) {
				structures[0]->query(object_rect, collisions);
				if (collisions.elements() == 0) {
					object->setPos(x, y);
					return true;
				}
			}
		}
		return false;
	}
	
	void loadPathFile(Basicobject* object,std::string& path_file,const char* path){
		if((object->getMoveType())!=4 && object->getMoveType()!=8){
			return;
		}
		PFile reader;
		std::string real_path=path;
		real_path+="/move_path/";
		real_path+=path_file;
		if(reader.load(real_path.c_str())){
			string directions=reader.readLine();
			size_t counter=0;
			size_t string_length=directions.len();
			while(counter<string_length){
				if(directions[counter]!=';'){
					object->addPathDirection(((directions[counter])-'0'));
				}
				counter++;
			}
		}
		reader.close();
	}
	
	Basicobject* loadFileObject(char* filename,uint8_t mode=0){
		string full_path=single_object_path;
		full_path+=filename;
		PFile file_loader;
		char* line=nullptr;
		if(file_loader.load(full_path)){
			if(!file_loader.isEnd()){
				line=file_loader.readLine();
			}
		}
		else{
			return nullptr;
		}
		file_loader.close();
		if(line==nullptr){
			return nullptr;
		}
		Imageloader loader;
		int counter=0;
		int currentpos=counter;
		LinkedList<string> informations;
		string placeholder_string;
		while(line[counter]!='\0'){
			if(line[counter]==';'){
				placeholder_string.copy(line,currentpos,counter-currentpos);
				informations.add(placeholder_string);
				currentpos=counter+1;
			}
			counter++;
		}
		if(counter!=currentpos){
			placeholder_string.copy(line,currentpos,counter-currentpos);
			informations.add(placeholder_string);
		}
		Basicobject* placeholder_object=nullptr;
		/* 
		Alle mit () gekennzeichnete sind mit Bedingungen angeknüpft
							falls copy_bool==1											(falls move_type==(4|8) | falls move_type==6)  falls Tile_info==1							                      falls move_type==7
		Tile_info;Copy_bool;(copy_amount;random_pos;)Name_der_Image_file.type;x;y;move_type;(((path_filename)|(xpos;ypos;));)speed(;tilewidth;tileheight;{{Tile1},{Tile2}})(Anzahl der Bewegungen;Bewegungsrichtung(0-3);Bewegugnsrichtung;...)
		
		Werte für move_type:
		0 -> Links
		1 -> Rechts
		2 -> Oben
		3 -> Unten
		4 -> Mit eigene Custom Pfad aber ohne Berücksichtigung für die Entgegengesetze Richtung bei Kollision
		5 -> Random von 0-3
		6 -> Versucht zur Zielkordinate zu laufen
		7 -> Läuft Move-Pattern ab (Liste von Bewegungsrichtung (0-3))
		8 -> Läuft Custom Pfad aber mit Berücksichtigung für die Entgegengesetze Richtung bei Kollision
		
		
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
		1;1;1;1;Name_der_Image_file.type;x;y;4;"Test.txt";speed;tilewidth;tileheight;{{1,3},{4,4}}
		1;1;1;1;Name_der_Image_file.type;x;y;7;speed;tilewidth;tileheight;{{1,3},{4,4}};5;0;0;2;1;2
		*/
		LinkedList<string>* current_info=&informations;
		string tile_info=current_info->element();
		current_info=current_info->next();
		byte copy_bool=stringToInt(current_info->element());
		current_info=current_info->next();
		size_t copy_amount=1;
		byte random_pos=0;
		if(copy_bool==1){
			copy_amount=stringToInt(current_info->element());
			current_info=current_info->next();
		}
		random_pos=stringToInt(current_info->element());
		current_info=current_info->next();
		position<int> rand_pos(2);
		string tilename=current_info->element();
		Basicobject* tile_placeholder=loader.load<Basicobject>(tilename);
		current_info=current_info->next();
		int x=stringToInt(current_info->element());
		current_info=current_info->next();
		int y=stringToInt(current_info->element());
		current_info=current_info->next();
		int move_type=stringToInt(current_info->element());
		current_info=current_info->next();
		position<int> destiny={-1,-1};
		std::string path_file;
		if(move_type==4 || move_type==8){
			path_file=current_info->element();
			current_info=current_info->next();
		}
		if(move_type==6){
			destiny[0]=stringToInt(current_info->element());
			current_info=current_info->next();
			destiny[1]=stringToInt(current_info->element());
			current_info=current_info->next();
		}
		int speed=stringToInt(current_info->element());
		current_info=current_info->next();
		int* move_pattern=nullptr;
		max_uint move_pattern_length=0;
		if(tile_info=='0'){
			if(move_type==7){
				move_pattern_length=stringToInt(current_info->element());
				current_info=current_info->next();
				move_pattern = new int[move_pattern_length];
				for(max_uint i=0;i<move_pattern_length;i++){
					move_pattern[i]=stringToInt(current_info->element());
					current_info=current_info->next();
				}
			}
			placeholder_object=new Basicobject();
			placeholder_object->setMode(stringToInt(tile_info));
			placeholder_object->setTileName(tilename);
			placeholder_object->setSize(tile_placeholder->getWidth(),tile_placeholder->getHeight());
			placeholder_object->setPixels(tile_placeholder->getPixels());
			placeholder_object->setTransPixels(tile_placeholder->getTransparency());
			placeholder_object->setMoveType(move_type);
			loadPathFile(placeholder_object,path_file,single_object_path);
			placeholder_object->setDestinyPosition(destiny);
			placeholder_object->setSpeed(speed);
			placeholder_object->filterPixels(255,255,255,0);
			placeholder_object->setPos(x,y);
			placeholder_object->setMovePattern(move_pattern,move_pattern_length);
			if(random_pos>0){
				position<int> rand_pos={};
				if(getPlacePosition(rand_pos,placeholder_object,random_pos)){
					placeholder_object->setPos(rand_pos[0],rand_pos[1]);
				}
			}
			if(move_pattern!=nullptr){
				delete[] move_pattern;
			}
			delete tile_placeholder;
			informations.clear();
			return placeholder_object;
		}
		int tile_size_width=stringToInt(current_info->element());
		current_info=current_info->next();
		int tile_size_height=stringToInt(current_info->element());
		current_info=current_info->next();
		//####################################################//
		string lineinfo=current_info->element();
		LinkedList<string> object_tiles;
		counter=2;
		currentpos=counter;
		int tile_row_counter=0;
		int max_tile_column_counter=0;
		int tile_column_counter=0;
		LinkedList<LinkedList<int>*>* full_tiles=new LinkedList<LinkedList<int>*>;
		LinkedList<int>* placeholder_tile_value=nullptr;
		placeholder_string=' ';
		while(counter<lineinfo.len()){
			if(lineinfo[counter]=='}'){
				placeholder_string.copy(lineinfo,currentpos,counter-currentpos);
				if(placeholder_tile_value==nullptr){
					placeholder_tile_value=new LinkedList<int>();
				}
				placeholder_tile_value->add(stringToInt(placeholder_string));
				currentpos=counter+3;
				if(tile_column_counter+1>max_tile_column_counter){
					max_tile_column_counter=tile_column_counter+1;
				}
				tile_column_counter=0;
				tile_row_counter++;
				counter=currentpos;
				full_tiles->add(placeholder_tile_value);
				placeholder_tile_value=nullptr;
			}
			else if(lineinfo[counter]==','){
				if(placeholder_tile_value==nullptr){
					placeholder_tile_value=new LinkedList<int>();
				}
				placeholder_string.copy(lineinfo,currentpos,counter-currentpos);
				placeholder_tile_value->add(stringToInt(placeholder_string));
				currentpos=counter+1;
				tile_column_counter++;
			}
			counter++;
		}
		LinkedList<LinkedList<int>*>* current_full_list=full_tiles;
		LinkedList<int>* current_tile_list=nullptr;
		int pixel_width=max_tile_column_counter*tile_size_width;
		int pixel_height=tile_row_counter*tile_size_height;
		int pixel_bitwidth=((pixel_width*3+3)/4)*4;
		uint8_t* pixel_part=new uint8_t[pixel_bitwidth*(pixel_height)];
		uint8_t* pixel_trans_part=new uint8_t[pixel_bitwidth*(pixel_height)];
		if(tile_row_counter>0){
			for(size_t i=0;i<copy_amount;i++){
				tile_row_counter=0;
				int tile_pos[2];
				int tile_id=0;
				int tile_per_row=(tile_placeholder->getWidth()/tile_size_width);
				int _counter=0;
				while(current_full_list!=nullptr){
					tile_column_counter=0;
					current_tile_list=current_full_list->element();
					while(current_tile_list!=nullptr){
						tile_id=current_tile_list->element();
						tile_pos[0]=(tile_id%tile_per_row);
						tile_pos[1]=(tile_id/tile_per_row);
						drawPixelPart(pixel_part,pixel_trans_part,pixel_width,pixel_height,pixel_bitwidth,tile_placeholder,tile_pos[0],tile_pos[1],tile_column_counter,tile_row_counter,tile_size_width,tile_size_height);
						tile_column_counter++;
						current_tile_list=current_tile_list->next();
					}
					tile_row_counter++;
					current_full_list=current_full_list->next();
				}
			}
		}
		//####################################################//
		current_info=current_info->next();
		if(move_type==7){
			move_pattern_length=stringToInt(current_info->element());
			current_info=current_info->next();
			move_pattern = new int[move_pattern_length];
			for(max_uint i=0;i<move_pattern_length;i++){
				move_pattern[i]=stringToInt(current_info->element());
				current_info=current_info->next();
			}
		}
		current_full_list=full_tiles;
		placeholder_object=new Basicobject();
		placeholder_object->setTiles(tile_size_width,tile_size_height);
		placeholder_object->setMode(stringToInt(tile_info));
		placeholder_object->setTileName(tilename);
		placeholder_object->setSize(pixel_width,pixel_height);
		placeholder_object->setPixels(pixel_part);
		placeholder_object->setTransPixels(pixel_trans_part);
		placeholder_object->setMoveType(move_type);
		loadPathFile(placeholder_object,path_file,single_object_path);
		placeholder_object->setDestinyPosition(destiny);
		placeholder_object->setSpeed(speed);
		placeholder_object->filterPixels(255,255,255,0);
		placeholder_object->setPos(x,y);
		placeholder_object->setMovePattern(move_pattern,move_pattern_length);
		placeholder_object->setTileInfos(current_full_list);
		if(random_pos>0){
			position<int> rand_pos={};
			if(getPlacePosition(rand_pos,placeholder_object,random_pos)){
				placeholder_object->setPos(rand_pos[0],rand_pos[1]);
			}
		}
		if(move_pattern!=nullptr){
			delete[] move_pattern;
		}
		delete[] pixel_part;
		delete[] pixel_trans_part;
		while(current_full_list!=nullptr){
			current_tile_list=current_full_list->element();
			current_tile_list->clear();
			delete current_tile_list;
			current_full_list=current_full_list->next();
		}
		full_tiles->clear();
		delete full_tiles;
		informations.clear();
		object_tiles.clear();
		delete tile_placeholder;
		return placeholder_object;
	}
	
	void loadSingleObject(char* line, Imageloader& loader, const char* path, uint8_t mode = 0) {
    std::vector<std::string> parts;
    std::string placeholder_string;
    size_t i = 0;
    size_t partStart = 0;
    while (line[i] != '\0') {
        if (line[i] == ';') {
            parts.emplace_back(line + partStart, line + i);
            partStart = i + 1;
        }
        i++;
    }
    if (i != partStart) parts.emplace_back(line + partStart, line + i);

    if (parts.empty()) return;

    auto getPart = [&](size_t idx) -> const std::string& {
        static const std::string empty = "";
        return idx < parts.size() ? parts[idx] : empty;
    };

    size_t idx = 0;
    const std::string tile_info = getPart(idx++);
    if (tile_info.empty()) return;

    if (idx >= parts.size()) return;
    byte copy_bool = static_cast<byte>(stringToInt(getPart(idx++)));

    size_t copy_amount = 1;
    if (copy_bool == 1) {
        if (idx >= parts.size()) return;
        copy_amount = static_cast<size_t>(stringToInt(getPart(idx++)));
    }

    if (idx >= parts.size()) return;
    byte random_pos = static_cast<byte>(stringToInt(getPart(idx++)));

    if (idx >= parts.size()) return;
    std::string tilename = getPart(idx++);

    Basicobject* tile_placeholder = loader.load<Basicobject>(tilename.c_str());
    if (tile_placeholder == nullptr) return;

    if (idx + 2 >= parts.size()) { delete tile_placeholder; return; }
    int x = stringToInt(getPart(idx++));
    int y = stringToInt(getPart(idx++));
    int move_type = stringToInt(getPart(idx++));
    position<int> destiny = {-1, -1};
    std::string path_file;

    if (move_type == 4) {
        if (idx >= parts.size()) { delete tile_placeholder; return; }
        path_file = getPart(idx++);
    }
    if (move_type == 6) {
        if (idx + 1 >= parts.size()) { delete tile_placeholder; return; }
        destiny[0] = stringToInt(getPart(idx++));
        destiny[1] = stringToInt(getPart(idx++));
    }
	
	if (move_type == 8) {
        if (idx >= parts.size()) { delete tile_placeholder; return; }
        path_file = getPart(idx++);
    }

    if (idx >= parts.size()) { delete tile_placeholder; return; }
    int speed = stringToInt(getPart(idx++));
    int* move_pattern = nullptr;
    size_t move_pattern_length = 0;

	size_t object_counter=0;
    if (tile_info == "0") {
        if (move_type == 7) {
            if (idx >= parts.size()) { delete tile_placeholder; return; }
            move_pattern_length = static_cast<size_t>(stringToInt(getPart(idx++)));
            move_pattern = new int[move_pattern_length];
            for (size_t m = 0; m < move_pattern_length; ++m) {
                if (idx >= parts.size()) { delete[] move_pattern; delete tile_placeholder; return; }
                move_pattern[m] = stringToInt(getPart(idx++));
            }
        }
        std::cout << "Objekte eingefuegt: ";
        for (size_t c = 0; c < copy_amount; ++c) {
            Basicobject* placeholder_object = new Basicobject();
            placeholder_object->setMode(stringToInt(tile_info));
            placeholder_object->setTileName(tilename.c_str());
            placeholder_object->setSize(tile_placeholder->getWidth(), tile_placeholder->getHeight());
            placeholder_object->setPixels(tile_placeholder->getPixels());
            placeholder_object->setTransPixels(tile_placeholder->getTransparency());
            placeholder_object->setMoveType(move_type);
            loadPathFile(placeholder_object, path_file, path);
            placeholder_object->setDestinyPosition(destiny);
            placeholder_object->setSpeed(speed);
            placeholder_object->filterPixels(255,255,255,0);
            placeholder_object->setPos(x,y);
            placeholder_object->setMovePattern(move_pattern, move_pattern_length);
			//placeholder_object->clearPixelsMode();
            if (mode == 0){
				if(placePosition(placeholder_object, random_pos)){
					std::cout << (object_counter + 1) << ",";
					object_counter++;
				}
			}
            else{
				list.add(placeholder_object);
				std::cout << (object_counter + 1) << ",";
				object_counter++;
			}
        }
        std::cout << "\n";
        if (move_pattern) { delete[] move_pattern; move_pattern = nullptr; }
        delete tile_placeholder;
        return;
    }
    if (idx + 1 >= parts.size()) { delete tile_placeholder; return; }
    int tile_size_width  = stringToInt(getPart(idx++));
    int tile_size_height = stringToInt(getPart(idx++));
    if (idx >= parts.size()) { delete tile_placeholder; return; }
    const std::string lineinfo = getPart(idx++);
    std::vector<std::vector<int>> full_tiles;
    std::vector<int>* curRow = nullptr;
    size_t pos = 0;
    size_t len = lineinfo.length();
    while (pos < len) {
        char c = lineinfo[pos];
        if (c == '{') {
            pos++;
            continue;
        } else if (c == '}' ) {
            if (curRow != nullptr) {
                full_tiles.emplace_back(std::move(*curRow));
                delete curRow;
                curRow = nullptr;
            }
            ++pos;
            continue;
        } else if ( (c >= '0' && c <= '9') || c == '-' ) {
            size_t start = pos;
            if (lineinfo[pos] == '-') ++pos;
            while (pos < len && (lineinfo[pos] >= '0' && lineinfo[pos] <= '9')) ++pos;
            std::string token = lineinfo.substr(start, pos - start);
            if (!token.empty()) {
                int val = stringToInt(token);
                if (curRow == nullptr) curRow = new std::vector<int>();
                curRow->push_back(val);
            }
            continue;
        } else {
            pos++;
            continue;
        }
    }
    if (curRow != nullptr) {
        full_tiles.emplace_back(std::move(*curRow));
        delete curRow;
        curRow = nullptr;
    }
    int tile_row_count = static_cast<int>(full_tiles.size());
    int max_tile_columns = 0;
    for (const auto& row : full_tiles) if ((int)row.size() > max_tile_columns) max_tile_columns = static_cast<int>(row.size());

    int pixel_width  = max_tile_columns * tile_size_width;
    int pixel_height = tile_row_count * tile_size_height;
    int pixel_bitwidth = ((pixel_width * 3 + 3) / 4) * 4;

    uint8_t* pixel_part = nullptr;
    uint8_t* pixel_trans_part = nullptr;
    if (pixel_width > 0 && pixel_height > 0) {
        pixel_part = new uint8_t[pixel_bitwidth * pixel_height];
        pixel_trans_part = new uint8_t[pixel_bitwidth * pixel_height];
        std::fill_n(pixel_part, pixel_bitwidth * pixel_height, 0);
        std::fill_n(pixel_trans_part, pixel_bitwidth * pixel_height, 0);
    }
    if (tile_row_count > 0 && pixel_part != nullptr) {
        for (size_t copy = 0; copy < copy_amount; ++copy) {
            for (int row = 0; row < tile_row_count; ++row) {
                const std::vector<int>& rowVec = full_tiles[row];
                int dest_col = 0;
                for (size_t col = 0; col < rowVec.size(); ++col) {
                    int tile_id = rowVec[col];
                    int tile_per_row = tile_placeholder->getWidth() / tile_size_width;
                    int tile_pos_x = tile_id % tile_per_row;
                    int tile_pos_y = tile_id / tile_per_row;
                    drawPixelPart(pixel_part, pixel_trans_part, pixel_width, pixel_height, pixel_bitwidth,
                                  tile_placeholder, tile_pos_x, tile_pos_y,
                                  dest_col, row, tile_size_width, tile_size_height);
                    dest_col++;
                }
            }
        }
    }
    if (move_type == 7) {
        if (idx < parts.size()) {
            move_pattern_length = static_cast<size_t>(stringToInt(getPart(idx++)));
            move_pattern = new int[move_pattern_length];
            for (size_t m = 0; m < move_pattern_length; ++m) {
                if (idx >= parts.size()) { delete[] move_pattern; move_pattern = nullptr; break; }
                move_pattern[m] = stringToInt(getPart(idx++));
            }
        }
    }
    std::cout << "Objekte eingefuegt: ";
    for (size_t c = 0; c < copy_amount; ++c) {
        Basicobject* placeholder_object = new Basicobject();
        placeholder_object->setTiles(tile_size_width, tile_size_height);
        placeholder_object->setMode(stringToInt(tile_info));
        placeholder_object->setTileName(tilename.c_str());
        placeholder_object->setSize(pixel_width, pixel_height);
        placeholder_object->setPixels(pixel_part);
        placeholder_object->setTransPixels(pixel_trans_part);
        placeholder_object->setMoveType(move_type);
        loadPathFile(placeholder_object, path_file, path);
        placeholder_object->setDestinyPosition(destiny);
        placeholder_object->setSpeed(speed);
        placeholder_object->filterPixels(255,255,255,0);
        placeholder_object->setPos(x,y);
        placeholder_object->setMovePattern(move_pattern, move_pattern_length);
        placeholder_object->setTileInfos(full_tiles);
		//placeholder_object->clearPixelsMode();
        if (mode == 0){
			if(placePosition(placeholder_object, random_pos)){
				std::cout << (object_counter + 1) << ",";
				object_counter++;
			}
		}
        else{
			std::cout << (object_counter + 1) << ",";
					object_counter++;
			list.add(placeholder_object);
		}
    }
    std::cout << "\n";
    if (move_pattern) { delete[] move_pattern; move_pattern = nullptr; }
    if (pixel_part) { delete[] pixel_part; pixel_part = nullptr; }
    if (pixel_trans_part) { delete[] pixel_trans_part; pixel_trans_part = nullptr; }
    delete tile_placeholder;
}



	
	void loadObjects(const char* path,Imageloader& loader,PFile& filereader,const char* folder_path,PArray<Basicobject*>& object_list,uint8_t mode=0){
		if(filereader.load(path)){
			int counter=0;
			char* line=nullptr;
			while(!filereader.isEnd()){
				line=filereader.readLine();
				std::cout<<"Zeile "<<counter<<"->";
				loadSingleObject(line,loader,folder_path,mode);
				counter++;
			}
			if(mode==0){
				structures[0]->query(frame,object_list);
			}
		}
	}
	
	void createDataStruc(int datastruc,LinkedList<string>*& infos){
		string data_struc_name=infos->element();
		infos=infos->next();
		switch(datastruc){
			case 0:{
				Quadtree<Basicobject*,1>* struc=new Quadtree<Basicobject*,1>(0,0,frame[0],frame[1],stringToInt(infos->element()));
				addStruc(struc,data_struc_name);
				break;
			}
			case 1:{
				BVH<Basicobject*,2>* struc=new BVH<Basicobject*,2>();
				addStruc(struc,data_struc_name);
				break;
			}
			case 2:
			{
				int tilewidth=stringToInt(infos->element());
				infos=infos->next();
				int tileheight=stringToInt(infos->element());
				UGrid<Basicobject*,3>* struc=new UGrid<Basicobject*,3>(tilewidth,tileheight,frame[0],frame[1],1);
				addStruc(struc,data_struc_name);
				break;
			}
			case 3:
			{
				int depth=stringToInt(infos->element());
				infos=infos->next();
				int min_tilewidth=stringToInt(infos->element());
				infos=infos->next();
				int min_tileheight=stringToInt(infos->element());
				GridQuadtree<Basicobject*,4>* struc=new GridQuadtree<Basicobject*,4>(0,0,frame[0],frame[1],depth,min_tilewidth,min_tileheight);
				addStruc(struc,data_struc_name);
				break;
			}
			case 4:
			{
				int depth=stringToInt(infos->element());
				infos=infos->next();
				BVHQuadtree<Basicobject*,5>* struc=new BVHQuadtree<Basicobject*,5>(0,0,frame[0],frame[1],depth);
				addStruc(struc,data_struc_name);
				break;
			}
			case 5:
			{
				int tilewidth=stringToInt(infos->element());
				infos=infos->next();
				int tileheight=stringToInt(infos->element());
				infos=infos->next();
				UGridBVH<Basicobject*,6>* struc=new UGridBVH<Basicobject*,6>(tilewidth,tileheight,frame[0],frame[1],1);
				addStruc(struc,data_struc_name);
				break;
			}
			case 6:
			{
				CompressedQuadtree<Basicobject*,7>* struc=new CompressedQuadtree<Basicobject*,7>(0,0,frame[0],frame[1],stringToInt(infos->element()));
				addStruc(struc,data_struc_name);
			}
		}
	}
	
	void loadSingleDatastruc_rebuild(char* line){
		int counter=0;
		int currentpos=counter;
		LinkedList<string> informations;
		string placeholder_string;
		while(line[counter]!='\0'){
			if(line[counter]==';'){
				placeholder_string.copy(line,currentpos,counter-currentpos);
				informations.add(placeholder_string);
				currentpos=counter+1;
			}
			counter++;
		}
		if(counter!=currentpos){
			placeholder_string.copy(line,currentpos,counter-currentpos);
			informations.add(placeholder_string);
		}
		LinkedList<string>* current_info=&informations;
		int structure=stringToInt(current_info->element());
		current_info=current_info->next();
		createDataStruc(structure,current_info);
		informations.clear();
	}
	
	void loadSingleDatastruc(char* line){
		int counter=0;
		int currentpos=counter;
		LinkedList<string> informations;
		string placeholder_string;
		while(line[counter]!='\0'){
			if(line[counter]==';'){
				placeholder_string.copy(line,currentpos,counter-currentpos);
				informations.add(placeholder_string);
				currentpos=counter+1;
			}
			counter++;
		}
		if(counter!=currentpos){
			placeholder_string.copy(line,currentpos,counter-currentpos);
			informations.add(placeholder_string);
		}
		LinkedList<string>* current_info=&informations;
		int structure=stringToInt(current_info->element());
		current_info=current_info->next();
		createDataStruc(structure,current_info);
		informations.clear();
	}
	
	void loadDatastructures_rebuild_start(const char* path){
		PFile filereader;
		if(filereader.load(path)){
			max_strucs_list=filereader.maxLines();
			if(struct_queue_counter<max_strucs_list){
				string line=filereader.getLine(struct_queue_counter);
				loadSingleDatastruc_rebuild(line);
				std::cout<<"Datenstruktur in Zeile "<<struct_queue_counter<<" erfolgreich geladen!\n";
			}
			filereader.close();
		}
	}
	
	void loadDatastructures_rebuild(const char* path){
		if(struct_queue_counter<max_strucs_list){
			PFile filereader;
			if(filereader.load(path)){
				string line=filereader.getLine(struct_queue_counter);
				loadSingleDatastruc_rebuild(line);
				std::cout<<"Datenstruktur in Zeile "<<struct_queue_counter<<" erfolgreich geladen!\n";
				filereader.close();
			}
		}
	}
	
	void loadDatastructures(const char* path){
		PFile filereader;
		if(filereader.load(path)){
			char* line=nullptr;
			while(!filereader.isEnd()){
				line=filereader.readLine();
				loadSingleDatastruc(line);
			}
			filereader.close();
		}
	}
	
	void loadSingleCommand(char* line){
		int counter=0;
		int currentpos=counter;
		LinkedList<string> informations;
		string placeholder_string;
		while(line[counter]!='\0'){
			if(line[counter]==';'){
				placeholder_string.copy(line,currentpos,counter-currentpos);
				informations.add(placeholder_string);
				currentpos=counter+1;
			}
			counter++;
		}
		if(counter!=currentpos){
			placeholder_string.copy(line,currentpos,counter-currentpos);
			informations.add(placeholder_string);
		}
		LinkedList<string>* current_info=&informations;
		Command new_command;
		new_command.time=stringToInt(current_info->element());
		current_info=current_info->next();
		new_command.command_id=stringToInt(current_info->element());
		current_info=current_info->next();
		new_command.variable=(current_info->element());
		command_list.add(new_command);
		informations.clear();
	}
	
	void loadCommands(const char* path){
		PFile filereader;
		if(filereader.load(path)){
			int counter=0;
			char* line=nullptr;
			while(!filereader.isEnd()){
				line=filereader.readLine();
				loadSingleCommand(line);
				counter++;
			}
			if(counter>0){
				hasCommands=true;
				next_command_time=(command_list.element()).time;
				current_command_list=&command_list;
			}
			filereader.close();
		}
	}
	
	bool out_of_range(rect<2>& frame){
		rect<2> range;
	//	if(list_start==1){
	//		range=list[0]->getRect();
	//	}
	//	else{
			range=this->frame;
	//	}
		if((frame.pos[0]<0) || (frame.pos[0]> range[0]) || (frame.pos[1]<0) || (frame.pos[1]>range[1])
			|| ((frame.pos[0]+frame[0])> range[0]) || ((frame.pos[1]+frame[1])>range[1])){
			return true;
		}
		return false;
	}
	
	void drawCollision(rect<2> rect,int r=255,int g=0,int b=0){
		int bitwidth=getBitmapWidth();
		for(int i=0;i<rect[1];i++){
			pixels[bitwidth*(frame[1]-(i+1+rect.pos[1]))+(rect.pos[0])*3+2]=r;
			pixels[bitwidth*(frame[1]-(i+1+rect.pos[1]))+(rect.pos[0])*3+1]=g;
			pixels[bitwidth*(frame[1]-(i+1+rect.pos[1]))+(rect.pos[0])*3]=b;
			pixels[bitwidth*(frame[1]-(i+1+rect.pos[1]))+(rect.pos[0]+rect[0])*3+2]=r;
			pixels[bitwidth*(frame[1]-(i+1+rect.pos[1]))+(rect.pos[0]+rect[0])*3+1]=g;
			pixels[bitwidth*(frame[1]-(i+1+rect.pos[1]))+(rect.pos[0]+rect[0])*3]=b;
		}
		for(int j=0;j<rect[0];j++){
			pixels[bitwidth*(frame[1]-(1+rect.pos[1]))+(j+rect.pos[0])*3+2]=r;
			pixels[bitwidth*(frame[1]-(1+rect.pos[1]))+(j+rect.pos[0])*3+1]=g;
			pixels[bitwidth*(frame[1]-(1+rect.pos[1]))+(j+rect.pos[0])*3]=b;
			pixels[bitwidth*(frame[1]-(1+rect.pos[1]+rect[1]))+(j+rect.pos[0])*3+2]=r;
			pixels[bitwidth*(frame[1]-(1+rect.pos[1]+rect[1]))+(j+rect.pos[0])*3+1]=g;
			pixels[bitwidth*(frame[1]-(1+rect.pos[1]+rect[1]))+(j+rect.pos[0])*3]=b;
		}
	}
	
	void find_all_Collisions(rect<2> object_rect,Basicobject*& object,PArray<Basicobject*>& array){
		max_uint counter=0;
		benchmarks[QUERY].start();
		for(max_uint i=list_start;i<list.elements();i++){
			if(list[i]!=object){
				if((list[i]->getRect()).intersect(object_rect)){
					array.add(list[i]);
					counter++;
				}
			}
		}
		benchmarks[QUERY].stop();
		benchmarks[QUERY].counter+=counter;
	}
	
	//##########Direction ist hier gleich move_type
	void setOppositeDirection(Basicobject* object,int direction){
		if(direction==8){
			int path_direction=object->getLastPathDirection();
			int newpath_direction=0;
			switch(path_direction){
				case 0:
					newpath_direction=1;
					break;
				case 1:
					newpath_direction=0;
					break;
				case 2:
					newpath_direction=3;
					break;
				case 3:
					newpath_direction=2;
					break;
			}
			object->changePathPattern(newpath_direction);
			return;
		}
		if(direction>=4){
			return;
		}
		int newdirection=4;
		switch(direction){
			case 0:
				newdirection=1;
				break;
			case 1:
				newdirection=0;
				break;
			case 2:
				newdirection=3;
				break;
			case 3:
				newdirection=2;
				break;
		}
		object->setMoveType(newdirection);
	}
	
	void make_move(Basicobject* object, int direction,int move_type){
		if(direction>=0 && direction<=3){
			object->move(direction);
		}
	}
	
	int getMoveDirection(Basicobject* object,int move_type){
		if(move_type==8){
			return object->getCurrentPathDirection();
		}
		if(move_type==7){
			return object->getNextPattern();
		}
		if(move_type==6){
			return object->getNextDirection();
		}
		if(move_type==5){
			return random(3);
		}
		if(move_type==4){
			return object->getCurrentPathDirection();
		}
		return move_type;
	}
	
	void updateCollision(){
		if(structures.elements()>0){
			if(structures[0]!=nullptr){
				PArray<Basicobject*> all_real_collisions;
				PArray<Basicobject*> collisions;
				PArray<Basicobject*> real_collisions_others;
				int direction=0;
				int move_type=0;
				max_uint real_found=0;
				rect<2> check_frame;
				//Query nur um die Objekte zu bewegen (Nicht relevant)
				//Bewegt alle Objekte im Raum außer die Map
				//Query nur um die Objekte zu malen
				max_uint end = list.elements();
				for(max_uint i=list_start;i<end;i++){
					if(list[i]->canMove()){
						move_type=list[i]->getMoveType();
						direction=getMoveDirection(list[i],move_type);
						check_frame=list[i]->move_rect(direction);
						//Prüfe Kollisionen für den Move-Bereich
						if(!out_of_range(check_frame) && (direction!=-1)){
							structures[0]->query(check_frame,collisions,1,list[i]);
							structures[0]->setCounter(QUERY,structures[0]->getCounter(QUERY)+collisions.elements());
							find_all_Collisions(check_frame,list[i],all_real_collisions);
							for(int s=1;s<structures.elements();s++){
								structures[s]->query(check_frame,real_collisions_others,1,list[i]);
								structures[s]->setCounter(QUERY,structures[s]->getCounter(QUERY)+real_collisions_others.elements());
								real_collisions_others.clear();
							}
							if(set==0){
								manager.object_move(list[i],direction);
							}
							if(collisions.elements()>0){
								if(collision_visible && drawing){
									drawCollision(check_frame);
								}
								setOppositeDirection(list[i],move_type);
								
							}
							else{
								make_move(list[i],direction,move_type);
							}
							collisions.clear();
							all_real_collisions.clear();
						}
						else{
							if(set==0){
								manager.object_move(list[i],direction);
							}
							setOppositeDirection(list[i],move_type);
						}
					}
					else{
						if(set==0){
							manager.object_move(list[i],direction);
						}
						check_frame=list[i]->getRect();
						//Prüfe Kollisionen für den Move-Bereich
						if(!out_of_range(check_frame)){
							structures[0]->query(check_frame,collisions,1,list[i]);
							structures[0]->setCounter(QUERY,structures[0]->getCounter(QUERY)+collisions.elements());
							find_all_Collisions(check_frame,list[i],all_real_collisions);
							for(int s=1;s<structures.elements();s++){
								structures[s]->query(check_frame,real_collisions_others,1,list[i]);
								structures[s]->setCounter(QUERY,structures[s]->getCounter(QUERY)+real_collisions_others.elements());
								real_collisions_others.clear();
							}
							if(collisions.elements()>0){
								if(collision_visible && drawing){
									drawCollision(check_frame);
								}
							}
							collisions.clear();
							all_real_collisions.clear();
						}
					}
				}
			}
		}
	}
	
	string getSceneLog(){
		string log_file=current_scene;
		log_file+="Comparing.txt";
		return log_file;
	}
	
	void createLogAll(){
		string log_file=getSceneLog();
		Logger::log(log_file,"=========================Neue Messung=========================");
		for(max_uint j=0;j<benchmarks.elements();j++){
			benchmarks[j].logPrint(log_file);
		}
		for(max_uint i=0;i<structures.elements();i++){
			for(max_uint j=0;j<(structures[i]->getBenchmarks()).elements();j++){
				(structures[i]->getBenchmark(j)).logPrint(log_file);
			}
		}
		for(max_uint i=0;i<structures.elements();i++){
			for(max_uint j=0;j<benchmarks.elements();j++){
				(structures[i]->getBenchmark(j)).logCompare(log_file,benchmarks[j]);
			}
		}
		for(max_uint i=0;i<structures.elements();i++){
			for(max_uint k=i+1;k<structures.elements();k++){
				for(max_uint j=0;j<(structures[i]->getBenchmarks()).elements();j++){
					(structures[i]->getBenchmark(j)).logCompare(log_file,structures[k]->getBenchmark(j));
				}
			}
		}
	}
	
	void clearLog(){
		if(current_scene!=""){
			string log_file=getSceneLog();
			if(fileExists(log_file)){
				std::remove(log_file);
			}
		}
	}
	
	void handleCommand(Command& command){
		switch(command.command_id){
			case 0:{
				Basicobject* object=loadFileObject(command.variable);
				if(object!=nullptr){
					insert_command(object);
					return;
				}
				std::cout<<"Insert-Fehler: Objekt konnte nicht mit der Datei \""<<command.variable<<"\" erstellt werden!\n";
				break;
			}
			case 1:{
				if(remove_command(stringToInt(command.variable))){
					return;
				}
				std::cout<<"Remove-Fehler: Objekt mit der ID \""<<command.variable<<"\" konnte nicht gelöscht oder gefunden werden!\n";
				break;
			}
			case 2:
				std::cout<<"STD::COUT: "<<command.variable<<"\n";
				break;
		}
	}
	
	void logBenchSeconds(double time){
		if(second_bench_path!=""){
			Logger::log(second_bench_path,"<------------------------/##################################\\------------------------>");
			string placeholder = "--->Szene mit Objektanzahl: ";
			placeholder+=list.elements();
			Logger::log(second_bench_path,placeholder);
			placeholder="Messung bei : ";
			placeholder+=time;
			placeholder+=" ";
			placeholder+=time_name;
			Logger::log(second_bench_path,placeholder);
			Logger::log(second_bench_path,"|-|-|-| Speicherverwaltung: ");
			MemoryManager::logReport(second_bench_path);
			Logger::log(second_bench_path,"|-|-|-| --------------------");
			for(max_uint i=0;i<benchmarks.elements();i++){
				benchmarks[i].logPrint(second_bench_path);
			}
			for(max_uint i=0;i<structures.elements();i++){
				for(max_uint j=0;j<(structures[i]->getBenchmarks()).elements();j++){
					(structures[i]->getBenchmark(j)).logPrint(second_bench_path);
				}
			}
		}
	}
	
	public:
	
	void checkTimer(){
		if(timer_set){
			timer.stop();
			double time_in_ms=timer.duration_ms();
			double time_in_s=time_in_ms/1000;
			if(hasCommands){
				if(next_command_time<=time_in_ms){
					if(current_command_list!=nullptr && current_command_list->hasElement()){
						handleCommand(current_command_list->element());
						current_command_list=current_command_list->next();
						if(current_command_list!=nullptr && current_command_list->hasElement()){
							next_command_time=(current_command_list->element()).time;
						}
						else{
							hasCommands=false;
							next_command_time=0;
						}
					}
				}
			}
			if(time_in_ms>=current_timer_ms){
				logBenchSeconds(current_timer_ms/timer_ms_converter);
				current_timer_ms+=(timer_adder*timer_ms_stepper);
			}
			if(timer_set_time<=time_in_s){
				createLogAll();
				clear();
				return;
			}
		}
	}
	
	void checkLimitUpdate(){
		if(hasCommands){
			if(next_command_time<=current_updates){
				if(current_command_list!=nullptr && current_command_list->hasElement()){
					handleCommand(current_command_list->element());
					current_command_list=current_command_list->next();
					if(current_command_list!=nullptr && current_command_list->hasElement()){
						next_command_time=(current_command_list->element()).time;
					}
					else{
						hasCommands=false;
						next_command_time=0;
					}
				}
			}
		}
		if(update_check_set){
			if(phase_2==true){
				if(current_updates>=operation_amount){
					createLogAll();
					clear();
					return;
				}
			}
			else{
				if(current_updates>=check_updates){
					createLogAll();
					clear();
					return;
				}
			}
			current_updates++;
		}
	}
	
	void checkLimitCollision(){
		if(collision_check_set){
			if(structures.elements()>0){
				if(structures[0]!=nullptr){
					if(structures[0]->getCounter(QUERY)>=check_collisions){
						createLogAll();
						clear();
						return;
					}
				}
			}
		}
	}
	
	void update_set_function(unsigned long long update_value,string scenename){
		loadScene(scenename,update_value);
		sema_paint.P();
		check_updates=update_value;
		current_updates=0;
		update_check_set=true;
		sema_paint.V();
	}
	
	void collision_set_function(max_uint collision_value,string scenename){
		loadScene(scenename);
		check_collisions=collision_value;
		collision_check_set=true;
	}
	
	void timer_function(max_uint timer_value,string scenename){
		loadScene(scenename);
		timer_set_time=timer_value;
		timer_set=true;
		timer.start();
	}
	
	void timer_update(){
		if(timer_set){
			timer.stop();
			double time_in_ms=timer.duration_ms();
			double time_in_s=time_in_ms/1000;
			if(time_in_ms>=current_timer_ms){
				logBenchSeconds(current_timer_ms/timer_ms_converter);
				current_timer_ms+=(timer_adder*timer_ms_stepper);
			}
		}
	}
	
	void timer_update_end(){
		if(timer_set){
			timer.stop();
			double time_in_ms=timer.duration_ms();
			double time_in_s=time_in_ms/1000;
			logBenchSeconds(time_in_s);
		}
	}
	
	void timerStart(){
		timer_set=true;
		current_timer_ms=timer_start_value;
		timer.reset();
		timer.start();
	}
	
	void timerStop(){
		timer_set=false;
		timer.stop();
		current_timer_ms=timer_start_value;
		timer.reset();
	}
	
	Basicobject* createRandomObject(uint8_t random_mode=0,int width=0,int height=0,int x=0,int y=0){
		Basicobject* object = new Basicobject();
		object->setSpeed(0);
		object->setMoveType(0);
		object->setMode(0);
		if(random_mode==0){
			static std::random_device rd;
			static std::mt19937 gen(rd()); 
			
			std::uniform_int_distribution<> rnd_w(1, frame[0]);
			std::uniform_int_distribution<> rnd_h(1, frame[1]);

			int rnd_w_value = rnd_w(gen);
			int rnd_h_value = rnd_h(gen);
			
			std::uniform_int_distribution<> rnd_x(0, (frame[0]-rnd_w_value));
			std::uniform_int_distribution<> rnd_y(0, (frame[1]-rnd_h_value));
			
			int rnd_x_value=rnd_x(gen);
			int rnd_y_value=rnd_y(gen);
		
			object->setSize(rnd_w_value,rnd_h_value);
			object->setPos(rnd_x_value,rnd_y_value);
		}
		else{
			object->setSize(width,height);
			object->setPos(x,y);
		}
		//object->clearPixelsMode();
		return object;
	}
	
	max_uint random_list_element(uint8_t type=0){
		if(type==0){
			static std::random_device rd;
			static std::mt19937 gen(rd());
			std::uniform_int_distribution<> rnd_element(0, list.elements()-1);
			return rnd_element(gen);
		}
		//type=0 : random /type=1 :letzes element / type=2 :erstes Element / type=3 : vordefiniertes
		if(type==1){
			if(list.elements()>0){
				return (list.elements()-1);
			}
		}
		return 0;
	}
	
	void list_insert(Basicobject*& new_object){
		benchmarks[INSERT].start();
		list.add(new_object);
		benchmarks[INSERT].stop();
		benchmarks[INSERT].counter+=1;
	}
	
	void list_remove(max_uint id){
		benchmarks[REMOVE].start();
		list.remove(id);
		if(list.elements()<list.size()){
			list[list.elements()]=nullptr;
		}
		benchmarks[REMOVE].stop();
		benchmarks[REMOVE].counter+=1;
	}
	
	void insert_operation(){
		if (structures.elements() > 0 && structures[0] != nullptr) {
			Basicobject* add_object=createRandomObject(operation_array[current_updates].mode,operation_array[current_updates].width,operation_array[current_updates].height,operation_array[current_updates].x,operation_array[current_updates].y);
			if(set_insert==0){
				operation_array[current_updates].width=add_object->getWidth();
				operation_array[current_updates].height=add_object->getHeight();
				operation_array[current_updates].x=(add_object->getPos())[0];
				operation_array[current_updates].y=(add_object->getPos())[1];
				operation_array[current_updates].operation=1;
				operation_array[current_updates].mode=1;
				
			}
			list_insert(add_object);
			structures[0]->insert(add_object, 1);
		}
	}
	
	void remove_operation(){
		if (structures.elements() > 0 && structures[0] != nullptr && (list.elements()>0)) {
			max_uint remove_id=operation_array[current_updates].id;
			if(set_insert==0){
				if(operation_array[current_updates].mode!=3){
					remove_id=random_list_element(operation_array[current_updates].mode);
				}
				operation_array[current_updates].width=0;
				operation_array[current_updates].height=0;
				operation_array[current_updates].x=0;
				operation_array[current_updates].y=0;
				operation_array[current_updates].operation=2;
				operation_array[current_updates].mode=3;
				operation_array[current_updates].id=remove_id;
			}
			Basicobject* object_removed=list[remove_id];
			structures[0]->remove(list[remove_id], 1);
			list_remove(remove_id);
			delete object_removed;
		}
	}
	
	uint8_t random_ins_rem(){
		static std::random_device rd;
		static std::mt19937 gen(rd());
		std::uniform_int_distribution<> rnd_type(1, 2);
		return rnd_type(gen);
	}
	
	void update_Insert_Remove(){
		switch(operation_array[current_updates].operation){
			case 0:
				break;
			case 1:
				insert_operation();
				break;
			case 2:
				remove_operation();
				break;
			case 3:{
				uint8_t operation_rnd = random_ins_rem();
				if(operation_rnd==2){
					if(list.elements()<=0){
						operation_rnd=1;
					}
				}
				if(operation_rnd==1){
					insert_operation();
				}
				else{
					remove_operation();
				}
				break;
			}
		}
	}
	
	void update(){
		if(update_check_set && isReady){
			if(phase_2==false){
				updateCollision();
			}
			else{
				update_Insert_Remove();
			}
			checkLimitUpdate();
			checkLimitCollision();
			timer_update();
		}
	}
	
	void setSize(int width,int height){
		frame[0]=width;
		frame[1]=height;
		createBitmapinfo();
		createPixels();
		pixelwidth=getBitmapWidth();
	}
	
	void drawStructure(bool boolean){
		draw_struc=boolean;
	}
	
	template<typename objecttype>
	void list_build(PArray<objecttype*> objectlist,int mode=0){
		if(objectlist.elements()>0){
			if(set==0){
				for(max_uint i=0;i<objectlist.elements();i++){
					manager.add(objectlist[i],check_collisions);
				}
			}
			if(mode==0){
				for(max_uint i=0;i<objectlist.elements();i++){
					list.add(objectlist[i]);
				}
				return;
			}
			benchmarks[BUILD].start();
			for(max_uint i=0;i<objectlist.elements();i++){
				list.add(objectlist[i]);
			}
			benchmarks[BUILD].stop();
			benchmarks[BUILD].counter+=1;
		}
	}
	
	template<typename objecttype>
	void list_add(objecttype*& object,int mode=0){
		if(set==0){
			manager.add(object,check_collisions);
		}
		if(mode==0){
			list.add(object);
			return;
		}
		benchmarks[INSERT].start();
		list.add(object);
		benchmarks[INSERT].stop();
		benchmarks[INSERT].counter+=1;
	}
	
	template<typename objecttype>
	void list_remove_command(objecttype*& object){
		bool removed=false;
		benchmarks[REMOVE].start();
		for(max_uint i=0;i<list.elements();i++){
			if(list[i]==object){
				list[i]->setID(0);
				list.remove(i);
				if(list.elements()<list.size()){
					list[list.elements()]=nullptr;
				}
				removed=true;
				break;
			}
		}
		benchmarks[REMOVE].stop();
		if(removed){
			benchmarks[REMOVE].counter+=1;
		}
	}
	
	template<typename objecttype>
	bool add(objecttype*& object, int mode = 0) {
		if (structures.elements() > 0 && structures[0] != nullptr) {
			bool success = (mode == 0)
				? structures[0]->insert(object)
				: structures[0]->insert(object, 1);

			if (success) {
				currentobjects++;
				return true;
			}
			return false;
		}
		return false;
	}
	
	void paint(HDC& hdc){
		if(structures.elements()>0){
			sema_paint.P();
			if(structures[0]!=nullptr){
				if(drawing){
					if(map!=nullptr && map_visible){
						drawObject(map,pixelwidth);
					}
					for(max_uint i=0;i<list.elements();i++){
						if(list[i]!=nullptr){
							drawObject(list[i],pixelwidth);
						}
					}
				}
				//Zum Bewegen der Objekte und zum Malen der Kollisionen
				update();
				if(drawing){
					if(draw_struc){
						structures[0]->drawStructure(frame,getBitmapWidth(),pixels);
					}
					StretchDIBits(hdc,frame.pos[0],frame.pos[1],frame[0],frame[1],0,0,frame[0],frame[1],pixels,&bitmapinfo,DIB_RGB_COLORS,SRCCOPY);
					memset(pixels, 0, pixel_size);
				}
			}
			sema_paint.V();
		}
	}
	
	void paintNoGui(){
		if(structures.elements()>0){
			sema_paint.P();
			if(structures[0]!=nullptr){
				update();
			}
			sema_paint.V();
		}
	}
	
	void compare(){
		sema_compare.P();
		(structures[0]->getBenchmark(QUERY)).compare(benchmarks[QUERY]);
		sema_compare.V();
	}
	
	template<typename objecttype>
	void insert_command(objecttype*& new_object){
		list_add(new_object,1);
		add(new_object,1);
		backup_array.add(new_object);
	}
	
	bool remove_command(max_uint id){
		if(id>=0 && id<list.elements()){
			if(structures.elements()>0){
				bool removed=false;
				for(max_uint i=0;i<structures.elements();i++){
					if(structures[i]->remove(list[id],1)){
						currentobjects-=1;
						removed=true;
					}
				}
				if(!removed){
					return false;
				}
			}
			list_remove_command(list[id]);
			return true;
		}
		return false;
	}
	
	template<typename structype>
	void addStruc(structype*& new_struc,string& data_struc_name){
		if(new_struc!=nullptr){
			StrucStat<structype*,Basicobject*>* struc_stat=new StrucStat<structype*,Basicobject*>(new_struc,data_struc_name);
			struct_queue.add(struc_stat);
		}
	}
	
	void clearNames(){
		current_scene="";
		single_object_path="";
		second_bench_path="";
		real_path="Scene/";
		objects_pattern="";
		operation_pattern="";
	}
	
	void showStructures(){
		std::cout<<"#############Structures-ID#############\n";
		for(int i=0;i<structures.elements();i++){
			std::cout<<structures[i]->getName()<<" mit StructureID "<<(structures[i]->getID()-1)<<"			: "<<i<<"\n";
		}
		std::cout<<"##########################\n";
	}
	
	void setFocusStruc(int newid){
		if(structures.elements()>1 && newid<structures.elements()){
			sema_paint.P();
			StrucStatBase<Basicobject*>* placeholder=structures[0];
			structures[0]=structures[newid];
			structures[newid]=placeholder;
			sema_paint.V();
		}
	}
	
	void turnMapOn(bool boolean){
		map_visible=boolean;
	}
	
	void showCollisions(bool boolean){
		collision_visible=boolean;
	}
	
	void setDrawing(bool boolean){
		drawing=boolean;
	}
	
	unsigned long long getUpdates(){
		return current_updates;
	}
	
	void operation_append(std::ofstream& out,max_uint id){
		out << "1;1;";
		uint8_t op_type=operation_array[id].operation-1;
		out << std::to_string(op_type);
		out << ";";
		if(op_type==1){
			out << std::to_string(operation_array[id].id);
		}
		else{
			out << std::to_string(operation_array[id].width);
			out << ";";
			out << std::to_string(operation_array[id].height);
			out << ";";
			out << std::to_string(operation_array[id].x);
			out << ";";
			out << std::to_string(operation_array[id].y);
		}
	}
	
	bool createOperationFile(char* file){
		if(operation_init_counter>0){
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
			for(max_uint i=0;i<operation_init_counter-1;i++){
				operation_append(out,i);
				out << '\n';
			}
			operation_append(out,operation_init_counter-1);
			out.close();
			return true;
		}
		return false;
	}
	
	int set=0;
	string real_path="Scene/";
	
	void reload_scene(){
		string data_struc_path=real_path;
		data_struc_path+="Datastruc.txt";
		try{
			loadDatastructures_rebuild(data_struc_path);
		}
		catch(string& exceptions){
			std::cout<<exceptions<<"\n";
		}
		catch(const char* exceptions){
			std::cout<<exceptions<<"\n";
		}
		structures.add(struct_queue[0]);
		second_bench_path=real_path;
		second_bench_path+="benchmarks/";
		second_bench_path+=struct_queue[0]->getName();
		second_bench_path+=".txt";
		if(structures.elements()>0){
			string scene_log_file=getSceneLog();
			string file_name_finished="objects_start_pattern";
			if(check_updates>0 && update_check_set){
				file_name_finished+="_";
				file_name_finished+=check_updates;
			}
			string object_path=real_path;
			object_path+=file_name_finished;
			object_path+=".txt";
			string operation_pattern=real_path;
			operation_pattern+="operation_start_pattern";
			if(check_updates>0){
				operation_pattern+="_";
				operation_pattern+=check_updates;
			}
			operation_pattern+=".txt";
			Imageloader loader;
			PFile filereader;
			PArray<Basicobject*> object_list;
			if(phase_2==true){
				if(set_insert==0){
					createOperationFile(operation_pattern);
					set_insert=1;
				}
			}
			else{
				if(set==0){
					manager.create(object_path);
					set=1;
				}
			}
			loadObjects(object_path,loader,filereader,real_path,object_list,set);
			if(set!=0){
				for(max_uint i=0;i<list.elements();i++){
					object_list.add(list[i]);
				}
				list.clear();
			}
			else{
				structures[0]->clear();
			}
			for(max_uint i=0;i<object_list.elements();i++){
				for(max_uint n=0;n<structures.elements();n++){
					object_list[i]->setStructure(structures[n]);
				}
				
			}
			for(max_uint n=0;n<structures.elements();n++){
				structures[n]->build(object_list,1);
			}
			initList();
			list_build(object_list);
			//#####################################
			for(max_uint i=0;i<object_list.elements();i++){
				backup_array.add(object_list[i]);
			}
			//#####################################
			Logger::log(scene_log_file,"======================Speicherverwaltung-Anfang======================");
			MemoryManager::logReport(scene_log_file);
			object_list.clear();
			filereader.close();
			timerStart();
			struct_queue_counter+=1;
		}
	}
	
	string objects_pattern;
	string operation_pattern;
	
	void setGUI(uint8_t new_mode){
		gui_mode=new_mode;
	}
	
	void start_pattern_append(std::ofstream& out,Basicobject* object){
		uint8_t mode=object->getModeType();
		rect<2> object_rect=object->getRect();
		position<int> object_pos=object_rect.pos;
		PArray<PArray<int>>& tiles=object->getTileInfos();
		out << std::to_string(mode);
		out << ";0;0;";
		out << object->getTileName();
		out << ";";
		out << std::to_string(object_pos[0]);
		out << ";";
		out << std::to_string(object_pos[1]);
		out << ";";
		out << std::to_string(0);
		out << ";";
		out << std::to_string(0);
		if(mode==1){
			out << ";";
			out << std::to_string(object->getTileWidth());
			out << ";";
			out << std::to_string(object->getTileHeight());
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
	}
	
	bool save_start_pattern(PArray<Basicobject*>& object_list,char* file){
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
		for(max_uint i=0;i<object_list.elements()-1;i++){
			start_pattern_append(out,object_list[i]);
			out << '\n';
		}
		start_pattern_append(out,object_list[object_list.elements()-1]);
		out.close();
		return true;
	}
	
	max_uint operation_init_counter=0;
	
	void loadSingleOperation(char* line){
		/*
		Format:
		#		Amount|Type_mode|Type_mode_extra	#
		   * Amount: Anzahl der Operationen
		   * Type_mode: 0=Nix/Error ; 1=Fix vordefinierte Operation; 2=Random; 3=Random Inserts; 4=Remove; 5;Insert-Remove-Abwechselnd
		   * Type_Mode_extra: Nur bei Remove und Fix vordefinierte Operation
		   
		   
		   Type Mode 1:
		   *Type_Mode_extra: 0:Insert; 1:Remove
				*danach kommt noch die Extra Infos für Type_mode_extra für 0 oder 1:
					Bei Type_Mode_extra 0 (Insert):
						dann die Objekt infos also:  ;Width;Height;x;y
					Bei Type_Mode_extra 1 (Remove):
						dann das Objekt welches gelöscht werden soll:  ;ObjektID
		   
		   Type Mode 4:
		   *Type_Mode_extra: 0:random; 1: letzes Element; 2: erstes Element; 3: Alternierend letzes elemnt->erstes element; 4: alternierend random->letzes->erstes
		   
		   
		   Beispiel für Fix Operation:
		   
			   Insert einfügen eines 20x20 großes objekt an stelle 0,4:
			   1;1;0;20;20;0;4
			   
			   Remove von Objekt mit ID 200:
			   1;1;1;200
			   
			Beispiel für Random:
				100 Random Operationen:
				100;2
				
			Beispiel für Random Insert (Random Größe und Position):
				100 Random Inserts:
				100;3
				
			Beispiel für Remove:
				100 Random Remove:
				100;4;0
				
				100 Remove Letzes Element:
				100;4;1
				
				100 Remove Alternierend Letzes->erstes->letzes...:
				100;4;3
				
				100 Remove Alternierend random->Letzes->erstes->random->letzes...:
				100;4;4
				
			Beispiel für Alternierend Insert-Remove (Insert->Remove(Random)->Insert->Remove(letztes Element)->Insert->Remove(erstes Element)->insert...):
				100 Remove Alternierend random->Letzes->erstes->random->letzes...:
				100;5
				
		   
		*/
		std::vector<std::string> result;
		std::stringstream ss(line);
		std::string item;
		while (std::getline(ss, item, ';')){ 
			result.push_back(item); 
		}
		max_uint amount=std::stoul(result[0]);
		operation_amount+=amount;
		uint8_t operation=std::stoul(result[1]);
		uint8_t operation_extra=0;
		if(result.size()>2){
			operation_extra=std::stoul(result[2]);
		}
		//0: error; 1:fixes; 2:random; 3:insert; 4:remove; 5:insert->remove
		switch(operation){
			case 0:
				throw "Error: Fehlerhafte Operation wurde geladen!\nPrüfe die Datei noch einmal!";
				break;
			case 1:{
				if(result.size()<=2) throw "Error: Zu wenig Parameter für Operation-Typ für Fixierte Operation in der Datei!";
				if(operation_extra>=2) throw "Error: Unbekannter Operation-Typ für Fixierte Operation in der Datei!";
				if(operation_extra==0){
					if(result.size()!=7) throw "Error: Zu wenig/viele Parameter für Operation-Typ für Fixierte Operation bei Insert in der Datei!";
					for(max_uint i=0;i<amount;i++){
						operation_array[operation_init_counter+i].width=std::stoul(result[3]);
						operation_array[operation_init_counter+i].height=std::stoul(result[4]);
						operation_array[operation_init_counter+i].x=std::stoul(result[5]);
						operation_array[operation_init_counter+i].y=std::stoul(result[6]);
						operation_array[operation_init_counter+i].operation=1;
						operation_array[operation_init_counter+i].mode=1;
						operation_array[operation_init_counter+i].id=0;
					}
				}
				else{
					if(result.size()!=4) throw "Error: Zu wenig/viele Parameter für Operation-Typ für Fixierte Operation bei Remove in der Datei!";
					for(max_uint i=0;i<amount;i++){
						operation_array[operation_init_counter+i].width=0;
						operation_array[operation_init_counter+i].height=0;
						operation_array[operation_init_counter+i].x=0;
						operation_array[operation_init_counter+i].y=0;
						operation_array[operation_init_counter+i].operation=2;
						operation_array[operation_init_counter+i].mode=3;
						operation_array[operation_init_counter+i].id=std::stoul(result[3]);
					}
				}
				break;
			}
			case 2:{
				for(max_uint i=0;i<amount;i++){
					operation_array[operation_init_counter+i].operation=3;
					operation_array[operation_init_counter+i].mode=0;
				}
				break;
			}
			case 3:{
				for(max_uint i=0;i<amount;i++){
					operation_array[operation_init_counter+i].operation=1;
					operation_array[operation_init_counter+i].mode=0;
				}
				break;
			}
			case 4:{
				uint8_t counter=0;
				if(operation_extra==3){
					counter=1;
				}
				for(max_uint i=0;i<amount;i++){
					operation_array[operation_init_counter+i].operation=2;
					if(operation_extra<3){
						operation_array[operation_init_counter+i].mode=operation_extra;
					}
					else{
						operation_array[operation_init_counter+i].mode=counter;
						counter++;
						if(counter>=3){
							if(operation_extra==3){
								counter=1;
							}
							else{
								counter=0;
							}
						}
					}
				}
				break;
			}
			case 5:{
				uint8_t counter=0;
				for(max_uint i=0;i<amount;i+=2){
					operation_array[operation_init_counter+i].operation=3;		// insert
					operation_array[operation_init_counter+i].mode=0;
					if(i+1<amount){
						operation_array[operation_init_counter+i+1].operation=2;		//remove
						operation_array[operation_init_counter+i+1].mode=counter;
						counter++;
						if(counter>=3){
							counter=0;
						}
					}
				}
				break;
			}
		}
		operation_init_counter+=amount;
	}
	
	void initOperationArray(char* operation_path){
		PFile filereader;
		if(!filereader.load(operation_path)){
			throw "Error: Operation Datei konnte nicht geladen werden!\n";
		}
		max_uint counter=0;
		char* line=nullptr;
		std::cout<<"###################\n";
		std::cout<<"Lade Operations:\n";
		while(!filereader.isEnd()){
			line=filereader.readLine();
			std::cout<<"Zeile "<<counter<<"\n";
			loadSingleOperation(line);
			counter++;
		}
		std::cout<<"###################\n";
	}
	
	void loadScene(const char* scenename,unsigned long long updates=0){
		sema_paint.P();
		clearNames();
		real_path+=scenename;
		objects_pattern=real_path;
		objects_pattern+="/objects_start_pattern";
		if(updates>0){
			objects_pattern+="_";
			objects_pattern+=updates;
		}
		objects_pattern+=".txt";
		operation_pattern=real_path;
		operation_pattern+="/operation_start_pattern";
		if(updates>0){
			operation_pattern+="_";
			operation_pattern+=updates;
		}
		operation_pattern+=".txt";
		if(!directoryExists(real_path)){
			std::cout<<"Szene: "<<scenename<<" kann nicht gefunden werden!\n";
			clearNames();
			sema_paint.V();
			return;
		}
		real_path+="/";
		single_object_path=real_path;
		single_object_path+="objects";
		if(!directoryExists(single_object_path)){
			std::cout<<"In der Szene \""<<scenename<<"\" ist der Ordner \"objects\" nicht vorhanden!\n";
			clearNames();
			sema_paint.V();
			return;
		}
		single_object_path+="/";
		string map_path=real_path;
		map_path+="Map.txt";
		string object_path=real_path;
		object_path+="Objects.txt";
		string operation_path=real_path;
		operation_path+="Operations.txt";
		string data_struc_path=real_path;
		data_struc_path+="Datastruc.txt";
		string command_path=real_path;
		command_path+="Commands.txt";
		if(gui_mode==1){
			if(!fileExists(map_path)){
				std::cout<<"In der Szene \""<<scenename<<"\" existiert keine \"Map.txt\" Datei!\n";
				clearNames();
				sema_paint.V();
				return;
			}	
		}
		if(!fileExists(object_path)){
			std::cout<<"In der Szene \""<<scenename<<"\" existiert keine \"Objects.txt\" Datei!\n";
			clearNames();
			sema_paint.V();
			return;
		}
		if(!fileExists(data_struc_path)){
			std::cout<<"In der Szene \""<<scenename<<"\" existiert keine \"Datastruc.txt\" Datei!\n";
			clearNames();
			sema_paint.V();
			return;
		}
		if(!fileExists(command_path)){
			std::cout<<"In der Szene \""<<scenename<<"\" existiert keine \"Commands.txt\" Datei!\n";
			clearNames();
			sema_paint.V();
			return;
		}
		if(!fileExists(operation_path)){
			std::cout<<"In der Szene \""<<scenename<<"\" existiert keine \"Operations.txt\" Datei!\n";
			clearNames();
			sema_paint.V();
			return;
		}
		set=0;
		set_insert=0;
		if(fileExists(objects_pattern)){
			object_path=objects_pattern;
			set=1;
		}
		if(fileExists(operation_pattern)){
			operation_path=operation_pattern;
			set_insert=1;
		}
		full_clear(1,1);
		try{
			loadDatastructures_rebuild_start(data_struc_path);
		}
		catch(string& exceptions){
			std::cout<<exceptions<<"\n";
			exit(0);
		}
		catch(const char* exceptions){
			std::cout<<exceptions<<"\n";
			exit(0);
		}
		second_bench_path=real_path;
		second_bench_path+="benchmarks/";
		second_bench_path+=struct_queue[0]->getName();
		second_bench_path+=".txt";
		structures.add(struct_queue[0]);
		struct_queue_counter+=1;
		if(structures.elements()>0){
			if(set==0){
				loadCommands(command_path);
			}
			current_scene=real_path;
			Imageloader loader;
			PFile filereader;
			if(gui_mode==1){
				if(map!=nullptr){
					delete map;
					map=nullptr;
				}
				map=new Map();
				map->load(map_path);
			}
			PArray<Basicobject*> object_list;
			loadObjects(object_path,loader,filereader,real_path,object_list,set);
			if(set!=0){
				for(max_uint i=0;i<list.elements();i++){
					object_list.add(list[i]);
				}
				list.clear();
			}
			else{
				structures[0]->clear();
			}
			for(max_uint i=0;i<object_list.elements();i++){
				for(max_uint n=0;n<structures.elements();n++){
					object_list[i]->setStructure(structures[n]);
				}
				
			}
			for(max_uint n=0;n<structures.elements();n++){
				structures[n]->build(object_list,1);
			}
			if(phase_2==true){
				try{
					initOperationArray(operation_path);
				}
				catch(string& exceptions){
					std::cout<<exceptions<<"\n";
					exit(0);
				}
				catch(const char* exceptions){
					std::cout<<exceptions<<"\n";
					exit(0);
				}
			}
			initList();
			list_build(object_list,1);
			//#####################################
			for(max_uint i=0;i<object_list.elements();i++){
				backup_array.add(object_list[i]);
			}
			if(phase_2==true){
				if(set==0){
					save_start_pattern(object_list,objects_pattern);
					set=1;
				}
			}
			//#####################################
			string scene_log_file=getSceneLog();
			Logger::log(scene_log_file,"======================Speicherverwaltung-Anfang======================");
			string object_string="--->Szene mit Objektanzahl: ";
			object_string+=list.elements();
			Logger::log(scene_log_file,object_string);
			std::cout<<"Objekte_Insgesamt: "<<list.elements()<<"\n";
			MemoryManager::logReport(scene_log_file);
			object_list.clear();
			filereader.close();
			timerStart();
			scene_name=scenename;
			isReady=true;
		}
		sema_paint.V();
	}
	
	Basicobject* operator[](max_uint index){
		return list[index];
	}
	
	void full_clear(int counter=0,int phase_switch_off=0){
		if(counter==0){
			if(set_insert==0 && phase_2==true){
				createOperationFile(operation_pattern);
				set_insert=1;
			}
			if(set==0){
				manager.create(objects_pattern);
			}
			clearNames();
		}
		timerStop();
		MemoryManager::report();
		for(max_uint i=0;i<benchmarks.elements();i++){
			benchmarks[i].clear();
		}
		for(int i=0;i<structures.elements();i++){
			if(structures[i]!=nullptr){
				structures[i]->clearComplete();
				delete structures[i];
				structures[i]=nullptr;
			}
		}
		structures.clear();
		list.clear();
		if(phase_2==false){
			for(max_uint i=0;i<backup_array.elements();i++){
				if(backup_array[i]!=nullptr){
					delete backup_array[i];
				}
			}
		}
		backup_array.clear();
		command_list.clear();
		if(map!=nullptr){
			delete map;
			map=nullptr;
			map_visible=false;
		}
		current_timer_ms=timer_start_value;
		current_command_list=nullptr;
		timer.reset();
		timer_set=false;
		timer_set_time=0;
		collision_check_set=false;
		check_collisions=0;
		current_updates=0;
		hasCommands=false;
		next_command_time=0;
		current_scene="";
		currentobjects=0;
		draw_struc=false;
		MemoryManager::report();
		struct_queue_counter=0;
		max_strucs_list=0;
		struct_queue.clear();
		manager.clear();
		operation_array.clear();
		operation_init_counter=0;
		isReady=false;
		if(phase_switch_off==0 && phase_2==true){
			operation_amount=0;
			check_updates=0;
			update_check_set=true;
		}
		if(phase_switch_off==0){
			if(phase_2==true){
				phase_2=false;
				scene_name="";
				std::cout<<"Szene vollstaendig abgeschlossen!\n";
			}
			else{
				phase_2=true;
				sema_paint.V();
				std::cout<<"\n#################INSERT-REMOVE-Phase#################\n";
				std::string scene_name_placeholder=scene_name;
				loadScene(scene_name_placeholder.c_str(),check_updates);
			}
		}
		else if(phase_switch_off==2){
			phase_2=false;
			scene_name="";
		}
	}
	
	void clear(){
		timer_update_end();
		timerStop();
		if(struct_queue_counter>=max_strucs_list){
			full_clear();
			return;
		}
		MemoryManager::report();
		for(max_uint i=0;i<benchmarks.elements();i++){
			benchmarks[i].clear();
		}
		for(int i=0;i<structures.elements();i++){
			if(structures[i]!=nullptr){
				structures[i]->clearComplete();
				delete structures[i];
				structures[i]=nullptr;
			}
		}
		structures.clear();
		list.clear();
		if(phase_2==false){
			for(max_uint i=0;i<backup_array.elements();i++){
				if(backup_array[i]!=nullptr){
					delete backup_array[i];
				}
			}
		}
		backup_array.clear();
		command_list.clear();
		current_timer_ms=timer_start_value;
		current_command_list=nullptr;
		hasCommands=false;
		timer.reset();
		current_updates=0;
		next_command_time=0;
		currentobjects=0;
		struct_queue.clear();
		MemoryManager::report();
		reload_scene();
	}
	
	Mainframe(){}
	
	~Mainframe(){
		if(pixels!=nullptr){
			delete[] pixels;
		}
		full_clear(1,2);
		manager.clear();
		struct_queue_counter=0;
		struct_queue.clear();
	}
};