#include <cmath>

#define QUERY 0
#define INSERT 1
#define REMOVE 2
#define BUILD 3
#define UPDATE 4
#define NOTIFY 5


struct Benchmark{
	Timer timer;
	//0:longest  1:shortest   2:average
	PArray<double> times={0,0,0};
	max_uint counter=0;
	max_uint count=0;
	PArray<string> time_names={"Longest Time","Shortest Time","Average Time"};
	bool started=false;
	string operation="";
	string name="";
	
	void start(){
		timer.start();
	}
	
	void stop(){
		timer.stop();
		double current_time=timer.duration_ms();
		if(started==false){
			for(byte i=0;i<times.elements();i++){ 
				times[i]=current_time;
			}
			count = 1;
			started = true;
			return;
		}
		times[2]=(times[2]+current_time);
		if(times[0]<current_time){
			times[0]=current_time;
		}
		if(times[1]>current_time){
			times[1]=current_time;
		}
		count++;
	}
	
	double valuePercent(double comparing){
		if(comparing>1){
			return (comparing*100);
		}
		return (1/comparing)*100;
	}
	
	void print(){
		std::cout<<"###################################\n";
		std::cout<<"----- Benchmark von "<<name<<" fuer "<<operation<<"-Operationen -----\n";
		for(byte i=0;i<times.elements()-1;i++){
			std::cout<<time_names[i]<<": "<<times[i]<<" ms\n";
		}
		if(count>0){
			double aver=times[times.elements()-1]/count;
			std::cout<<time_names[times.elements()-1]<<": "<<aver<<" ms\n";
		}
		else{
			std::cout<<time_names[times.elements()-1]<<": "<<times[times.elements()-1]<<" ms\n";
		}
		std::cout<<"Found: "<<counter<<"\n";
		std::cout<<"###################################\n";
	}
	
	void compare(Benchmark& other){
		print();
		other.print();
		std::cout<<"###################################\n";
		std::cout<<"----- Vergleich: "<<name<<" gegenueber "<<other.name<<" fuer "<<operation<<"-Operationen -----\n";
		double comparing=0;
		for(byte i=0;i<times.elements()-1;i++){
			if(times[i]==0){
				std::cout<<"Fehler bei "<<time_names[i]<<": Es kann nicht verglichen werden, da noch keine Werte gemessen wurde!"<<"\n";
			
			}
			else{
				comparing=(other.times[i])/(times[i]);
				if(comparing>1){
					std::cout<<"Schneller fuer "<<time_names[i]<<": "<<valuePercent(comparing)<<"%\n";
				}
				else{
					std::cout<<"Langsamer fuer "<<time_names[i]<<": "<<valuePercent(comparing)<<"%\n";
				}
			}
		}
		if(times[times.elements()-1]==0){
			std::cout<<"Fehler bei "<<time_names[times.elements()-1]<<": Es kann nicht verglichen werden, da noch keine Werte gemessen wurde!"<<"\n";
		}
		else{
			double aver_1=0;
			if(other.count>0){
				aver_1=other.times[times.elements()-1]/other.count;
			}
			else{
				aver_1=other.times[times.elements()-1];
			}
			double aver_2=0;
			if(count>0){
				aver_2=times[times.elements()-1]/count;
			}
			else{
				aver_2=times[times.elements()-1];
			}
			if(aver_2==0){
				std::cout<<"Fehler bei "<<time_names[times.elements()-1]<<": Es kann nicht verglichen werden, da noch keine Werte gemessen wurde!"<<"\n";
			}
			else{
				comparing=aver_1/aver_2;
				if(comparing>1){
					std::cout<<"Schneller fuer "<<time_names[times.elements()-1]<<": "<<valuePercent(comparing)<<"%\n";
				}
				else{
					std::cout<<"Langsamer fuer "<<time_names[times.elements()-1]<<": "<<valuePercent(comparing)<<"%\n";
				}
			}
		}
		if(counter==0){
			if(other.counter==0){
				std::cout<<"Fehler bei Counter: Es kann nicht verglichen werden, da noch keine Werte gemessen wurde!"<<"\n";
			}
			else{
				std::cout<<"Weniger Gefunden: 100%\n";
			}
		}
		else{
			comparing=(double)other.counter;
			comparing/=counter;
			if(comparing>1){
				std::cout<<"Weniger Gefunden: "<<valuePercent(comparing)<<"%\n";
			}
			else{
				std::cout<<"Mehr Gefunden: "<<valuePercent(comparing)<<"%\n";
			}
		}
		std::cout<<"###################################\n";
	}
	
	void logPrint(const char* path,uint8_t mode=0){
		Logger::log(path,"###################################");
		string placeholder;
		placeholder="----- Benchmark von ";
		placeholder+=name;
		placeholder+=" fuer ";
		placeholder+=operation;
		placeholder+="-Operationen -----";
		Logger::log(path,placeholder);
		for(byte i=0;i<times.elements()-1;i++){
			placeholder=time_names[i];
			placeholder+=": ";
			placeholder+=times[i];
			if(mode==0){
				placeholder+=" ms";
			}
			Logger::log(path,placeholder,mode);
		}
		if(count>0){
			double aver=times[times.elements()-1]/count;
			placeholder=time_names[times.elements()-1];
			placeholder+=": ";
			placeholder+=aver;
			if(mode==0){
				placeholder+=" ms";
			}
			Logger::log(path,placeholder,mode);
		}
		else{
			placeholder=time_names[times.elements()-1];
			placeholder+=": ";
			placeholder+=times[times.elements()-1];
			if(mode==0){
				placeholder+=" ms";
			}
			Logger::log(path,placeholder,mode);
		}
		placeholder="Found: ";
		placeholder+=counter;
		Logger::log(path,placeholder);

	}
	
	void logCompare(const char* path,Benchmark& other){
		Logger::log(path,"###################################");
		string placeholder;
		placeholder="----- Vergleich: ";
		placeholder+=name;
		placeholder+=" vs ";
		placeholder+=other.name;
		placeholder+=" fuer ";
		placeholder+=operation;
		placeholder+="-Operationen -----";
		Logger::log(path,placeholder);
		double comparing=0;
		for(byte i=0;i<times.elements()-1;i++){
			if(times[i]==0){
				placeholder="Fehler bei ";
				placeholder+=time_names[i];
				placeholder+=": Es kann nicht verglichen werden, da noch keine Werte gemessen wurde!";
				Logger::log(path,placeholder);
			}
			else{
				comparing=(other.times[i])/(times[i]);
				if(comparing>1){
					placeholder="Schneller fuer ";
					placeholder+=time_names[i];
					placeholder+=": ";
					placeholder+=valuePercent(comparing);
					placeholder+="%";
					Logger::log(path,placeholder);
				}
				else{
					placeholder="Langsamer fuer ";
					placeholder+=time_names[i];
					placeholder+=": ";
					placeholder+=valuePercent(comparing);
					placeholder+="%";
					Logger::log(path,placeholder);
				}
			}
		}
		if(times[times.elements()-1]==0){
			placeholder="Fehler bei ";
			placeholder+=time_names[times.elements()-1];
			placeholder+=": Es kann nicht verglichen werden, da noch keine Werte gemessen wurde!";
			Logger::log(path,placeholder);
		}
		else{
			double aver_1=0;
			if(other.count>0){
				aver_1=other.times[times.elements()-1]/other.count;
			}
			else{
				aver_1=other.times[times.elements()-1];
			}
			double aver_2=0;
			if(count>0){
				aver_2=times[times.elements()-1]/count;
			}
			else{
				aver_2=times[times.elements()-1];
			}
			if(aver_2==0){
				placeholder="Fehler bei ";
				placeholder+=time_names[times.elements()-1];
				placeholder+=": Es kann nicht verglichen werden, da noch keine Werte gemessen wurde!";
				Logger::log(path,placeholder);
			}
			else{
				comparing=aver_1/aver_2;
				if(comparing>1){
					placeholder="Schneller fuer ";
					placeholder+=time_names[times.elements()-1];
					placeholder+=": ";
					placeholder+=valuePercent(comparing);
					placeholder+="%";
					Logger::log(path,placeholder);
				}
				else{
					placeholder="Langsamer fuer ";
					placeholder+=time_names[times.elements()-1];
					placeholder+=": ";
					placeholder+=valuePercent(comparing);
					placeholder+="%";
					Logger::log(path,placeholder);
				}
			}
		}
		if(counter==0){
			if(other.counter==0){
				Logger::log(path,"Fehler bei Counter: Es kann nicht verglichen werden, da noch keine Werte gemessen wurde!");
			}
			else{
				Logger::log(path,"Weniger Gefunden: 100%");
			}
		}
		else{
			comparing=(double)other.counter;
			comparing/=counter;
			if(comparing>1){
				placeholder="Weniger Gefunden: ";
				placeholder+=valuePercent(comparing);
				placeholder+="%";
				Logger::log(path,placeholder);
			}
			else{
				placeholder="Mehr Gefunden: ";
				placeholder+=valuePercent(comparing);
				placeholder+="%";
				Logger::log(path,placeholder);
			}
		}
	}
	
	void clear(){
		times={0,0,0};
		started=false;
		counter=0;
	}
	
	Benchmark(const std::initializer_list<const char*>& initList) {
		if(initList.size() == 2){
			auto it = initList.begin();
			name = *it;
			++it;
			operation = *it;
		}
	}
	
	Benchmark(){}
	
	Benchmark(const char* newname,const char* operationname){
		name=newname;
		operation=operationname;
	}	
};

template<typename objecttype,typename object_form=rect<2>>
class StrucStatBase{
	PArray<Benchmark> benchs;
	Benchmark bench;
	//#######################//
	public:
	
	virtual void* getStructure(){
		return nullptr;
	}
	
	virtual int getID() const {
		return 0;
	}
	
	virtual string& getName(){
		return benchs[0].name;
	}
	
	//Erstelle die Datenstruktur durch Objektliste
	virtual void build(PArray<objecttype>& elements,byte bench=0){}
	
	// Fügt Objekt in die Datenstruktur ein
	virtual bool insert(const objecttype& object,byte bench=0){
		return false;
	}
	
	// Löscht Objekt von der Datenstruktur
	virtual bool remove(const objecttype& object,byte bench=0){
		return false;
	}
	
	// Speichert alle gefunden Objekte in "array" und gibt die größe zurück
	virtual size_t query(const object_form& query_rect,PArray<objecttype>& array,byte bench=0,objecttype object_pointer=nullptr){
		return 0;
	}
	
	// Allgemeine Funktion zum Updaten der Struktur->Falls nötig, z.B. Balanzieren oder Überprüfungen
	virtual void update(byte bench=0){}
	
	// Falls Objekte sich Updaten (Position,Größe,usw.)
	virtual void notify(const objecttype& object,const object_form& old_frame,byte bench=0){}
	
	// Visuelle Darstellung der Struktur
	virtual void drawStructure(object_form& frame,int bitwidth,uint8_t* pixels){}
	
	// Lösche und gebe alle Resourcen frei
	virtual void clear(){}
	
	virtual void setCounter(byte benchtype,max_uint new_counter){}
	
	virtual max_uint getCounter(byte benchtype){
		return 0;
	}
	
	virtual Benchmark& getBenchmark(byte benchtype){
		return bench;
	}
	
	virtual PArray<Benchmark>& getBenchmarks(){
		return benchs;
	}
	
	virtual void clearComplete(){}
	
};

template<typename StrucType,typename objecttype,typename object_form=rect<2>>
class StrucStat : public StrucStatBase<objecttype>{
	StrucType structure=nullptr;
	PArray<Benchmark> benchmarks={
		{"Datenstruktur","Query"},
		{"Datenstruktur","Insert"},
		{"Datenstruktur","Remove"},
		{"Datenstruktur","Build"},
		{"Datenstruktur","Update"},
		{"Datenstruktur","Notify"},
	};
	//#######################//
	public:
	
	virtual void* getStructure() override {
		return structure;
	}
	
	virtual int getID() const override {
		return structure->getID();
	}
	
	virtual string& getName() override {
		return structure->getName();
	}
	
	//Erstelle die Datenstruktur durch Objektliste
	void build(PArray<objecttype>& elements,byte bench=0) override {
		if(bench==0){
			structure->build(elements);
			return;
		}
		bool created=false;
		benchmarks[BUILD].start();
		created=structure->build(elements);
		benchmarks[BUILD].stop();
		if(created){
			benchmarks[BUILD].counter++;
		}
	}
	
	// Fügt Objekt in die Datenstruktur ein
	bool insert(const objecttype& object,byte bench=0) override {
		if(bench==0){
			return structure->insert(object);
		}
		bool boolean=false;
		benchmarks[INSERT].start();
		boolean=(structure->insert(object));
		benchmarks[INSERT].stop();
		if(boolean){
			benchmarks[INSERT].counter+=1;
		}
		return boolean;
	}
	
	// Löscht Objekt von der Datenstruktur
	bool remove(const objecttype& object,byte bench=0) override {
		if(bench==0){
			return structure->remove(object);
		}
		bool boolean=false;
		benchmarks[REMOVE].start();
		boolean=(structure->remove(object));
		benchmarks[REMOVE].stop();
		if(boolean){
			benchmarks[REMOVE].counter+=1;
		}
		return boolean;
	}
	
	// Speichert alle gefunden Objekte in "array" und gibt die größe zurück
	size_t query(const object_form& query_rect,PArray<objecttype>& array,byte bench=0,objecttype object_pointer=nullptr) override {
		if(bench==0){
			return structure->query(query_rect,array,object_pointer);
		}
		size_t query_amount=0;
		benchmarks[QUERY].start();
		query_amount=structure->query(query_rect,array,object_pointer);
		benchmarks[QUERY].stop();
		return query_amount;
		
	}
	
	// Allgemeine Funktion zum Updaten der Struktur->Falls nötig, z.B. Balanzieren oder Überprüfungen
	void update(byte bench=0) override {
		if(bench==0){
			structure->update();
			return;
		}
		benchmarks[UPDATE].start();
		structure->update();
		benchmarks[UPDATE].stop();
	}
	
	// Falls Objekte sich Updaten (Position,Größe,usw.)
	void notify(const objecttype& object,const object_form& old_frame,byte bench=0) override {
		if(bench==0){
			structure->notify(object,old_frame);
			return;
		}
		bool notified=false;
		benchmarks[NOTIFY].start();
		notified=structure->notify(object,old_frame);
		benchmarks[NOTIFY].stop();
		if(notified){
			benchmarks[NOTIFY].counter++;
		}
	}
	
	// Visuelle Darstellung der Struktur
	void drawStructure(object_form& frame,int bitwidth,uint8_t* pixels) override {
		structure->drawStructure(frame,bitwidth,pixels);
	}
	
	// Lösche und gebe alle Resourcen frei
	void clear() override {
		structure->clear();
		for(max_uint i=0;i<benchmarks.elements();i++){
			benchmarks[i].clear();
		}
	}
	
	void setCounter(byte benchtype,max_uint new_counter) override {
		benchmarks[benchtype].counter=new_counter;
	}
	
	max_uint getCounter(byte benchtype) override {
		return benchmarks[benchtype].counter;
	}
	
	Benchmark& getBenchmark(byte benchtype) override {
		return benchmarks[benchtype];
	}
	
	PArray<Benchmark>& getBenchmarks() override {
		return benchmarks;
	}
	
	void clearComplete() override {
		if(structure!=nullptr){
			structure->clearComplete();
			for(max_uint i=0;i<benchmarks.elements();i++){
				benchmarks[i].clear();
			}
			delete structure;
			structure=nullptr;
		}
	}
	
	StrucStat(StrucType new_struc,const char* data_struc_name=nullptr){
		structure=new_struc;
		if(data_struc_name!=nullptr){
			structure->setStrucName(data_struc_name);
			for(max_uint i=0;i<benchmarks.elements();i++){
				benchmarks[i].name=data_struc_name;
			}
		}
	}
	
	~StrucStat(){
	}
	
};