template <typename type>
struct PArray{
	
	private:
	
	type* arraylist=nullptr;
	max_uint arraysize=0;
	max_uint adder=4;
	unsigned char limiter=3;
	unsigned char counter=0;
	max_uint current_elements=0;
	
	operator type*(){
		return arraylist;
	}
	
	void clearArray(){
		if(arraylist!=nullptr){
			delete[] arraylist;
			arraylist=nullptr;
		}
		arraysize=0;
		adder=4;
		limiter=3;
		counter=0;
		current_elements=0;
	}
	
	void expandArray(){
		if(counter>=limiter){
			adder=adder<<1;
			counter=0;
		}
		else{
			counter++;
		}
		if(arraylist!=nullptr){	
			type* arraylist_new =new type[arraysize+adder];
			copyArray(arraylist_new,arraylist,arraysize);
			delete[] arraylist;
			arraylist=arraylist_new;
			arraysize+=adder;
		}
		else{
			arraylist=new type[arraysize+adder];
			arraysize+=adder;
		}
		
	}
	
	public:
	
	void clear(){
		clearArray();
	}
	
	max_uint elements(){
		return current_elements;
	}
	
	max_uint size() const {
		return arraysize;
	}
	
	void add(type element){
		if(current_elements>=arraysize){
			expandArray();
		}
		arraylist[current_elements]=element;
		current_elements++;
	}
	
	void remove(max_uint id){
		if(id<current_elements && id>=0){
			for(max_uint i=id;i<current_elements-1;i++){
				arraylist[i]=arraylist[i+1];
			}
			current_elements--;
		}
	}
	
	void create(max_uint newsize){
		clear();
		arraysize=newsize;
		arraylist=new type[arraysize];
	}
	
	type& operator[](max_uint position){
		if(position>=arraysize){
			if(position<arraysize+adder){
				expandArray();
			}
			else{
				throw "PArray Error: Index out of range!";
			}
		}
		return arraylist[position];
	}
	
	PArray& operator=(const PArray& other) {
		if(this!=&other){
			if(arraysize!=other.size()){
				if(arraylist!=nullptr){
					delete[] arraylist;
				}
				arraysize = other.size();
				arraylist = new type[arraysize];
			}
			adder=other.adder;
			limiter=other.limiter;
			counter=other.counter;
			current_elements=other.current_elements;
			copyArray(arraylist,other.arraylist,arraysize);
		}
		return *this;
	}
	
	// Movekonstruktor
	PArray& operator=(PArray&& other) noexcept {
		if(this!=&other){
			if(arraylist!=nullptr){
				delete[] arraylist;
			}
			arraysize = other.arraysize;
			arraylist = other.arraylist; 
			adder=other.adder;
			limiter=other.limiter;
			counter=other.counter;
			current_elements=other.current_elements;
			other.arraylist=nullptr; 
			other.arraysize=0;
			other.adder=0;
			other.limiter=0;
			other.counter=0;
			other.current_elements=0;
		}
		return *this;
	}
	
	// Movekonstruktor
	PArray& operator=(const std::initializer_list<type> other){
		if(other.size()!=arraysize){
			if(arraylist!=nullptr){
				delete[] arraylist;
			}
			arraysize = other.size();
			arraylist = new type[arraysize];
		}
		adder=4;
		counter=0;
		current_elements=other.size();
		std::copy(other.begin(), other.end(), arraylist);
		return *this;
	}
	
	PArray(const std::initializer_list<type>  initList) {
		arraysize = initList.size();
		arraylist = new type[arraysize]; 
		current_elements=arraysize;
		std::copy(initList.begin(), initList.end(), arraylist);
	}
	
	// Kopierkonstruktor
	PArray(const PArray& other) {
		arraysize = other.size();
		arraylist = new type[arraysize];
		adder=other.adder;
		limiter=other.limiter;
		counter=other.counter;
		current_elements=other.current_elements;
		copyArray(arraylist,other.arraylist,arraysize);
		
	}
	
	// Movekonstruktor
	PArray(PArray&& other) noexcept {
		arraysize = other.arraysize;
		arraylist = other.arraylist; 
		adder=other.adder;
		limiter=other.limiter;
		counter=other.counter;
		current_elements=other.current_elements;
		other.arraylist=nullptr; 
		other.arraysize=0;
		other.adder=0;
		other.limiter=0;
		other.counter=0;
		other.current_elements=0;
	}
	
	PArray(int newsize) {
		arraysize = newsize;
		arraylist = new type[arraysize]; 
	}
	
	PArray(){}
	
	~PArray(){
		if(arraylist!=nullptr){
			delete[] arraylist;
		}
	}
	
};

struct string{
	char* array=nullptr;
	size_t size=0;
	
	public:
	
	operator char* const(){
		return array;
	}
	
	string split(const char split_char){
		size_t split_pos=0;
		for(size_t i=0;i<size;i++){
			if(array[i]==split_char){
				split_pos=i;
			}
		}
		string split_string(split_pos);
		if(split_pos>0){
			for(size_t i=0;i<split_pos;i++){
				split_string[i]=array[i];
			}
		}
		return split_string;
	}
	
	size_t len() const {
		return size;
	}
	
	void copy(char* other,size_t startpos,size_t length){
		if(length!=size){
			if(array!=nullptr){
				delete[] array;
				array=nullptr;
			}
		}
		if(array==nullptr){
			size=length;
			array=new char[size+1];
			array[size]='\0';
		}
		for(size_t i=0;i<length;i++){
			array[i]=other[startpos+i];
		}
		
	}
	
	bool operator==(const string& other){
		if(other.size==size){
			return equalString(array,other.array,size);
		}
		return false;
	}
	
	bool operator!=(const string& other){
		return !(*this==other);
	}
	
	bool operator==(const char* other){
		size_t other_size=stringLength(other);
		if(other_size==size){
			return equalString(array,other,size);
		}
		return false;
	}
	
	bool operator!=(const char* other){
		return !(*this==other);
	}
	
	string operator+(int value){
		size_t length_value=valueLength(value);
		string tmp(size+length_value);
		copyArray(tmp.array, array, size);
		size_t written = snprintf(tmp.array+size, length_value, "%zu", value);
		if (written < 0 || written >= length_value) {
			std::cerr << "Fehler beim addieren(+) von einem Int Value mit dem String: Der Puffer reicht nicht aus für die Ausgabe.\n";
			throw "Fehler beim addieren von einem Int Value mit dem String: Der Puffer reicht nicht aus für die Ausgabe.\n";
		}
		return tmp;
    }
	
	string operator+(const char* other){
		size_t other_size=stringLength(other);
		string tmp(other_size+size);
		copyArray(tmp.array, array, size);
		copyArray(tmp.array+size, other, other_size);
		return tmp;
    }
	
	
    string operator+(const char other){
		string tmp(size+1);
		copyArray(tmp.array, array, size);
		tmp.array[size]=other;
		return tmp;
    }
	
    string operator+(const string& other){
		string tmp(other.size+size);
		copyArray(tmp.array, array, size);
		copyArray(tmp.array+size, other.array, other.size);
		return tmp;
    }
	
	string& operator+=(int value){
		size_t length_value=valueLength(value);
		char buffer[size+length_value+1];
		copyArray(buffer, array, size);
		size_t written = snprintf(buffer+size, length_value, "%zu", value);
		if (written < 0 || written >= length_value) {
			std::cerr << "Fehler beim addieren(+=) von einem Int Value mit dem String: Der Puffer reicht nicht aus für die Ausgabe.\n";
			throw "Fehler beim addieren von einem Int Value mit dem String: Der Puffer reicht nicht aus für die Ausgabe.\n";
		}
		if(array!=nullptr){
			delete[] array;
		}
		size=size+length_value;
		array=new char[size+1];
		copyArray(array, buffer, size);
		array[size]='\0';
		return *this;
    }
	
	string& operator+=(max_uint value) {
		// Geschätzte maximale Länge für eine unsigned long long-Zahl (max 20 Dezimalstellen)
		constexpr size_t MAX_DIGITS = 20;
		// Gesamtlänge des neuen Puffers (aktueller Inhalt + max. Zahl + Nullterminator)
		size_t total_length = size + MAX_DIGITS + 1;
		char* buffer = new char[total_length];
		// Bestehenden Inhalt kopieren
		copyArray(buffer, array, size);
		// Zahl als String einfügen (beginnend am Ende des bisherigen Inhalts)
		int written = snprintf(buffer + size, MAX_DIGITS + 1, "%llu", value);
		// Fehlerprüfung
		if (written < 0 || written >= static_cast<int>(MAX_DIGITS + 1)) {
			std::cerr << "Fehler beim snprintf: Der Puffer reicht nicht aus für die Ausgabe.\n";
			delete[] buffer;
			throw "Fehler beim addieren von max_uint zum String: Der Puffer reicht nicht aus.";
		}

		// Alten Speicher freigeben
		if (array != nullptr) {
			delete[] array;
		}

		// Neuen Speicher übernehmen
		array = buffer;
		size = size + written;
		array[size] = '\0'; // Sicherstellen, dass der String korrekt abgeschlossen ist

		return *this;
	}
	
	string& operator+=(double value) {
		size_t buffer_size = 32; // ausreichend für die meisten Fließkommazahlen
		char buffer[buffer_size+1];
		int written = snprintf(buffer, buffer_size, "%.6f", value);  // Format z. B. 6 Nachkommastellen
		if (written <= 0 || written >= buffer_size) {
			std::cerr << "Fehler beim addieren(+=) von einem Double Value mit dem String: Der Puffer reicht nicht aus.\n";
			throw "Fehler beim addieren von einem Double Value mit dem String: Der Puffer reicht nicht aus.\n";
		}
		return (*this += buffer);  // nutze bereits vorhandene +=(const char*)
	}

	
	string& operator+=(const char* other){
		size_t other_size=stringLength(other);
		char buffer[size+other_size];
		copyArray(buffer, array, size);
		copyArray(buffer+size, other, other_size);
		if(array!=nullptr){
			delete[] array;
		}
		size=size+other_size;
		array=new char[size+1];
		copyArray(array, buffer, size);
		array[size]='\0';
		return *this;
    }
	
	
    string& operator+=(const char other){
		char buffer[size+1];
		copyArray(buffer, array, size);
		buffer[size]=other;
		if(array!=nullptr){
			delete[] array;
		}
		size=size+1;
		array=new char[size+1];
		copyArray(array, buffer, size);
		array[size]='\0';
		return *this;
    }
	
    string& operator+=(const string& other){
		char buffer[other.size+size];
		copyArray(buffer, array, size);
		copyArray(buffer+size, other.array, other.size);
		if(array!=nullptr){
			delete[] array;
		}
		size=other.size+size;
		array=new char[size+1];
		copyArray(array, buffer, size);
		array[size]='\0';
		return *this;
    }
	
	string& operator=(const char other){
		if(size!=1){
			if(array!=nullptr){
				delete[] array;
			}
			size=1;
			array=new char[size+1];
		}
		array[0]=other;
		array[size]='\0';
		return *this;
	}
	
	string& operator=(const char* other){
		size_t other_size=stringLength(other);
		if(other_size>0){
			if(other_size!=size){
				if(array!=nullptr){
					delete[] array;
				}
				size=other_size;
				array=new char[size+1];
			}
			copyArray(array, other, size);
			array[size]='\0';
		}
		return *this;
	}
	
	// Schnellerer Kopier-Operator
	string& operator=(const string& other) {
        if (this != &other){
			if(other.size>0){
				if(other.size!=size){
					if(array!=nullptr){
						delete[] array;
					}
					size = other.size;
					array = new char[size + 1];
				}
				copyArray(array, other.array, size);
				array[size] = '\0';
			}
		}
		return *this;
    }

    // Schnellerer Move-Operator
    string& operator=(string&& other) noexcept {
        if (this != &other) {
			if(array!=nullptr){
				delete[] array;
			}
            array = other.array;
            size = other.size;
            other.array = nullptr;
            other.size = 0;
        }
		return *this;
    }
	
	char& operator[](size_t index){
		return array[index];
	}
	
	char& operator[](size_t index) const {
		return array[index];
	}
	
	string(){}
	
	// Kopierkonstruktor
    string(const string& other) {
        size = other.size;
        array = new char[size+1];
        copyArray(array, other.array, size);
		array[size]='\0';
    }

    // Move-Konstruktor
    string(string&& other) noexcept {
        array = other.array;
        size = other.size;
        other.array = nullptr;
        other.size = 0;
    }
	
	string(const char other){
		size=1;
		array=new char[size+1];
		array[0]=other;
		array[size]='\0';
	}
	
	string(const char* other){
		size=stringLength(other);
		array=new char[size+1];
		memcpy(array, other, size);
		array[size]='\0';
	}
	
	string(size_t newsize){
		size=newsize;
		array=new char[size+1];
		array[size]='\0';
	}
	
	~string(){
		if(array!=nullptr){
			delete[] array;
		}
	}
};

template<typename type>
struct LinkedList{
	LinkedList<type>* newlist=nullptr;
	type elementitem;
	bool has_element=false;
	
	bool hasElement() const {
		return has_element;
	}
	
	void add(type element){
		if(has_element==false){
			elementitem=element;
			has_element=true;
			return;
		}
		if(newlist==nullptr){
			newlist=new LinkedList();
		}
		newlist->add(element);
	}
	
	type& element(){
		return elementitem;
	}
	
	LinkedList* next(){
		return newlist;
	}
	
	void clear(){
		if(newlist!=nullptr){
			newlist->clear();
			delete newlist;
			newlist=nullptr;
		}
		has_element=false;
	}
	
};

template<typename store_type,typename size_type=size_t>
struct position{
	
	size_type size=0;
	store_type*	dimension=nullptr;
	
	operator store_type*(){
		return dimension;
	}
	
	store_type& operator[](size_type index){
		return dimension[index];
	}
	
	const store_type& operator[](size_type index) const {
		return dimension[index];
	}
	
	position operator/(const position& other) {
		if(size != other.size) {
			throw std::runtime_error("Dimensionen stimmen nicht überein!");
		}
		position<store_type,size_type> tmp;
		tmp.size=size;
		tmp.dimension=new store_type[tmp.size];
		for(store_type i=0;i<size;i++){
			tmp.dimension[i]=dimension[i]/other.dimension[i];
		}
		return tmp;
    }
	
	position operator*(const position& other) {
		if(size != other.size) {
			throw std::runtime_error("Dimensionen stimmen nicht überein!");
		}
		position<store_type,size_type> tmp;
		tmp.size=size;
		tmp.dimension=new store_type[tmp.size];
		for(store_type i=0;i<size;i++){
			tmp.dimension[i]=dimension[i]*other.dimension[i];
		}
		return tmp;
    }
	
	position operator-(const position& other) {
		if(size != other.size) {
			throw std::runtime_error("Dimensionen stimmen nicht überein!");
		}
		position<store_type,size_type> tmp;
		tmp.size=size;
		tmp.dimension=new store_type[tmp.size];
		for(store_type i=0;i<size;i++){
			tmp.dimension[i]=dimension[i]-other.dimension[i];
		}
		return tmp;
    }

	position operator+(const position& other) {
		if(size != other.size) {
			throw std::runtime_error("Dimensionen stimmen nicht überein!");
		}
		position<store_type,size_type> tmp;
		tmp.size=size;
		tmp.dimension=new store_type[tmp.size];
		for(store_type i=0;i<size;i++){
			tmp.dimension[i]=dimension[i]+other.dimension[i];
		}
		return tmp;
    }
	
	position& operator/=(const position& other) {
		if(size != other.size) {
			throw std::runtime_error("Dimensionen stimmen nicht überein!");
		}
		for(store_type i=0;i<size;i++){
			dimension[i]=dimension[i]/other.dimension[i];
		}
		return *this;
    }
	
	position& operator*=(const position& other) {
		if(size != other.size) {
			throw std::runtime_error("Dimensionen stimmen nicht überein!");
		}
		for(store_type i=0;i<size;i++){
			dimension[i]=dimension[i]*other.dimension[i];
		}
		return *this;
    }
	
	position& operator-=(const position& other) {
		if(size != other.size) {
			throw std::runtime_error("Dimensionen stimmen nicht überein!");
		}
		for(store_type i=0;i<size;i++){
			dimension[i]=dimension[i]-other.dimension[i];
		}
		return *this;
    }
	
	position& operator+=(const position& other) {
		if(size != other.size) {
			throw std::runtime_error("Dimensionen stimmen nicht überein!");
		}
		for(store_type i=0;i<size;i++){
			dimension[i]=dimension[i]+other.dimension[i];
		}
		return *this;
    }
	
	bool operator==(const position& other) const {
		if(other.size!=size){
			return false;
		}
		for(size_type i=0;i<size;++i){
            if(dimension[i]!=other.dimension[i]){
                return false;
			}
        }
		return true;
	}
	
	bool operator!=(const position& other) const {
        return !(*this == other);
	}
	
	// Initializer-List Zuweisungsoperator
	position& operator=(const std::initializer_list<store_type>& initList) {
        if(initList.size()>0){
			if(initList.size()!=size){
				size=initList.size();
				if(dimension!=nullptr){
					delete[] dimension;
				}
				dimension=new store_type[size];
			}
			std::copy(initList.begin(), initList.end(), dimension);
		}
		return *this;
    }
	
	//Kopier-Operator 
	position& operator=(const position& other) {
        if(&other!=this){
			if(other.size!=size){
				if(dimension!=nullptr){
					delete[] dimension;
				}
				size=other.size;
				dimension=new store_type[size];
			}
			copyArray(dimension,other.dimension,size);
		}
		return *this;
    }
	
	//Move-Operator 
	position& operator=(position&& other) noexcept {
        if(&other!=this){
			if(dimension!=nullptr){
				delete[] dimension;
			}
			dimension=other.dimension;
			size=other.size;
			other.dimension=nullptr;
			other.size=0;
		}
		return *this;
    }
	
	position(){}
	
	position(size_type new_size){
		if(new_size>0){
			size=new_size;
			dimension=new store_type[size]; 
			for(size_type i=0;i<size;i++){
				dimension[i]=0;
			}
		}
	}
	
	position(const std::initializer_list<store_type>  initList) {
		if(initList.size()>0){
			size=initList.size();
			dimension=new store_type[size];
			std::copy(initList.begin(), initList.end(), dimension);
		}
	}
	
	// Kopier-Konstruktor
    position(const position& other){
		if(other.size>0){
			size=other.size;
			dimension=new store_type[size];
			copyArray(dimension,other.dimension,size);
		}
    }
	
	// Move-Konstruktor
    position(position&& other) noexcept{
		dimension=other.dimension;
		size=other.size;
        other.dimension = nullptr;
        other.size = 0;
    }
	
	~position(){
		if(dimension!=nullptr){
			delete[] dimension;
		}
	}
	
};

template<unsigned int dimension,typename dimensiontype=int>
struct rect{
	position<dimensiontype> pos = position<dimensiontype>(dimension);
	dimensiontype dim_sizes[dimension]={};
	
	bool intersect(const rect<dimension>& check_rect) const {
		for(dimensiontype i=0;i<dimension;i++){
			if (pos[i] + dim_sizes[i] <= check_rect.pos[i] || 
				pos[i] >= check_rect.pos[i] + check_rect.dim_sizes[i]){    
				return false;
			}
		}
		return true;
	}
	
	bool contain(const rect<dimension>& check_rect) const {
		for (dimensiontype i = 0; i < dimension; i++) {
			if (pos[i] > check_rect.pos[i] || 
				pos[i] + dim_sizes[i] < check_rect.pos[i] + check_rect.dim_sizes[i]) {
				return false;
			}
		}
		return true;
	}
	
	void expand(const rect<dimension>& check_rect){
		for (unsigned int i = 0; i < dimension; i++) {
			dimensiontype new_min = PMath::min(pos[i], check_rect.pos[i]);
			dimensiontype new_max = PMath::max(pos[i] + dim_sizes[i], check_rect.pos[i] + check_rect.dim_sizes[i]);
			pos[i] = new_min;
			dim_sizes[i] = new_max - new_min;
		}
	}
	
	dimensiontype area() const {
		dimensiontype result = 1;
		for (unsigned int i = 0; i < dimension; ++i) {
			result *= dim_sizes[i];
		}
		return result;
	}
	
	//Gibt den Schnitt aus beiden Kollisionen zurück
	rect<dimension> getIntersection(const rect<dimension>& check_rect) const {
		rect<dimension> collision;
		for(dimensiontype i=0;i<dimension;i++){
			if (pos[i] + dim_sizes[i] > check_rect.pos[i] &&
				pos[i] < check_rect.pos[i] + check_rect.dim_sizes[i]){
					collision.pos[i]=PMath::max(pos[i],check_rect.pos[i]);
					collision.dim_sizes[i] = PMath::min(pos[i] + dim_sizes[i], check_rect.pos[i] + check_rect.dim_sizes[i]) - collision.pos[i];
			}
			else{
				return rect<dimension>(); // Leeres Rechteck
			}
		}
		return collision;
	}
	
	operator position<dimensiontype> const(){
		return pos;
	}
	
	dimensiontype& operator[](dimensiontype index){
		return dim_sizes[index];
	}
	
	const dimensiontype& operator[](dimensiontype index) const {
		return dim_sizes[index];
	}
	
	bool operator!=(const rect& copy){
		return !((*this)==copy);
	}
	
	bool operator==(const rect& copy){
		for(dimensiontype i=0;i<dimension;i++){
			if(pos[i]!=copy.pos[i]){
				return false;
			}
		}
		for(dimensiontype i=0;i<dimension;i++){
			if(dim_sizes[i]!=copy.dim_sizes[i]){
				return false;
			}
		}
		return true;
	}
	
	rect& operator=(const rect& copy){
		for(dimensiontype i=0;i<dimension;i++){
			pos[i]=copy.pos[i];
		}
		for(dimensiontype i=0;i<dimension;i++){
			dim_sizes[i]=copy.dim_sizes[i];
		}
		return *this;
	}
	
	rect(){}
	
};


void printRect(const rect<2> rect_object){
	std::cout<<rect_object[0]<<","<<rect_object[1]<<","<<rect_object.pos[0]<<","<<rect_object.pos[1]<<"\n";
}