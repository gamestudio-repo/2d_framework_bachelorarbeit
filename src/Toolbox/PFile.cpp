#include <fstream>

class PFile {
	
	max_uint currentpos=0;
    max_uint lines = 0;
    max_uint bytesize = 0;
    char** filebuffer = nullptr;
	byte* bytebuffer=nullptr;
	byte mode=0;
	const char* filename=nullptr;
	
	bool loadByte(std::ifstream& file){
		bytebuffer = new byte[bytesize];
        file.read(reinterpret_cast<char*>(bytebuffer), bytesize);
        return 1;
	}
	
	std::string clearLine(std::string line){
		size_t counter=0;
		for(size_t i=0;i<line.size();i++){
			if(line[i]!='\0' && line[i]!='\r' && line[i]!='\n'){
				counter++;
			}
		}
		std::string newline;
		newline.resize(counter);
		counter=0;
		for(int i=0;i<line.size();i++){
			if(line[i]!='\0' && line[i]!='\r' && line[i]!='\n'){
				newline[counter]=line[i];
				counter++;
			}
		}
		return newline;
	}
	
	bool loadTxt(std::ifstream& file){
		std::string line;
        while (std::getline(file, line)) {
            lines++;
        }
        // Setze den Dateizeiger auf den Anfang der Datei zurück
        file.clear(); // Setze den Zustand des Streams zurück
        file.seekg(0, std::ios::beg);
        if (lines <= 0) {
            return 1;
        }
        filebuffer = new char*[lines];
        max_uint counter = 0;
        while (std::getline(file, line)) {
			line=clearLine(line);
            filebuffer[counter] = new char[line.size() + 1];
            copyArray(filebuffer[counter], line.c_str(),line.size());
			filebuffer[counter][line.size()]='\0';
            counter++;
        }
        return 1;
	}
	
	public:
	
	max_uint maxLines(){
		return lines;
	}
	
	bool isEnd(){
		if(currentpos>=lines){
			return true;
		}
		return false;
	}
	
	const char* getFilename(){
		return filename;
	}
	
	byte readByte(){
		currentpos+=1;
		return bytebuffer[currentpos-1];
	}
	
	char* readLine(){
		currentpos+=1;
		return filebuffer[currentpos-1];
	}
	
	byte& getByte(max_uint index){
		return bytebuffer[index];
	}
	
	char* getLine(max_uint index) const {
		return filebuffer[index];
	}
	
	max_uint size(){
		return bytesize;
	}
	
	bool exist(const char* filename){
		std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file) {
            return 0;
        }
		file.close();
		return 1;
	}
	
	bool create(const char* filename){
		std::fstream file;
        file.open(filename, std::ios::out | std::ios::app);
        if (file.is_open()) {
            file.close();
			return true;
        }
		return false;
	}
	
	bool append(char* string){
		std::fstream datei;
		datei.open(filename, std::ios::out | std::ios::app); // Schreib- und Append-Modus
		if (datei) {
			std::cerr << string;
			datei.close();
			return 1;
		}
		return 0;
	}
	
	bool append(const char* string){
		std::fstream datei;
		datei.open(filename, std::ios::out | std::ios::app); // Schreib- und Append-Modus
		if (datei) {
			datei<< string;
			datei.close();
			return 1;
		}
		return 0;
	}

    bool load(const char* filename,byte mode=0) {
		this->filename=filename;
		this->mode=mode;
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file) {
			string new_string="PFile Error: Datei '";
			new_string+=filename;
			new_string+="' kann nicht geoeffnet oder gefunden werden!";
            throw new_string;
            return 0;
        }
        // Größe der Datei ermitteln
        bytesize = file.tellg();
        // Setze den Dateizeiger auf den Anfang der Datei zurück
        file.seekg(0, std::ios::beg);
		if(mode==0){
			if(!loadTxt(file)){
				file.close();
				return 0;
			}
		}
		else{
			if(!loadByte(file)){
				file.close();
				return 0;
			}
		}
        file.close();
		return 1;
    }

    void close() {
		if(filebuffer!=nullptr){
			for (max_uint i = 0; i < lines; i++) {
				delete[] filebuffer[i];
			}
			delete[] filebuffer;
			filebuffer=nullptr;
		}
		if(bytebuffer!=nullptr){
			delete[] bytebuffer;
			bytebuffer=nullptr;
		}
        currentpos=0;
        lines = 0;
        bytesize = 0;
		mode=0;
		filename=nullptr;
    }
};
