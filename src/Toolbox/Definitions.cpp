typedef unsigned long long int max_uint;
typedef long long int max_int;
typedef unsigned char byte;

template <typename stringtype_1,typename stringtype_2,typename size_type>
bool equalString(stringtype_1 string_1,stringtype_2 string_2,size_type size){
	for(size_type i=0;i<size;i++){
		if(string_1[i]!=string_2[i]){
			return false;
		}
	}
	return true;
}

template <typename arraytype1,typename arraytype2,typename size_type>
void copyArray(arraytype1* emptyarray,arraytype2* copyarray,size_type size){
	if(size>0){
		for(size_type i=0;i<size;i++){
			emptyarray[i]=copyarray[i];
		}
		//memcpy(emptyarray, copyarray, size*sizeof(arraytype2));
	}
}

template<typename stringtype>
max_uint stringLength(stringtype string){
	max_uint length = 0;
	if(!string){
		return length;
	}
    while (string[length] != '\0') {
        length++;
    }
    return length;
}

void valueToChar(size_t value, char* buffer, size_t bufferSize) {
    if (bufferSize == 0 || buffer == nullptr) {
        throw "Fehler: Ungültiger Buffer in valueToChar()";
    }
    size_t written = snprintf(buffer, bufferSize, "%zu", value);
    // Sicherheitshalber terminieren, falls snprintf das nicht tut
    buffer[bufferSize] = '\0';
    if (written < 0 || static_cast<size_t>(written) >= bufferSize) {
        std::cerr << "Fehler bei valueToChar(): Der Puffer reicht nicht aus für die Ausgabe.\n";
        return;
    }
}


template<typename value_type>
max_uint valueLength(value_type value) {
	if(value==0){
		return 1;
	}
	max_uint counter=0;
	while(value>0){
		value=value/10;
		counter++;
	}
	return counter;
}

int stringToInt(const char* str,int length=-1,int startpos=-1) {
	if(startpos==-1){
		startpos=0;
	}
	int result = 0;
	bool minus=false;
	str=str+startpos;
	// Prüfen, ob die Eingabe leer ist
	if (str == nullptr || *str == '\0') {
		throw std::invalid_argument("Ungueltige Eingabe bei stringToInt: Die Zeichenkette darf nicht leer sein.");
	}
	if(length==-1){
		if(*str=='-'){
			minus=true;
			str++;
		}
		while(*str){
			if (*str == '\r' || *str == '\0' || *str == '\n') {
				break;
			}
			if (*str < '0' || *str > '9') {
				std::string error="Ungueltige Eingabe bei der Funktion stringToInt. Eingabe enthaelt nicht-numerische Zeichen \'";
				error+=*str;
				error+="\' .\n";
				throw std::invalid_argument(error);
			}
			result = result * 10 + (*str - '0');  // Jede Ziffer umwandeln und addieren
			str++;
		}
	}
	else{
		while(length>0){
			if (*str < '0' || *str > '9') {
				std::string error="Ungueltige Eingabe bei der Funktion stringToInt. Eingabe enthaelt nicht-numerische Zeichen \'";
				error+=*str;
				error+="\' .\n";
				throw std::invalid_argument(error);
			}
			result = result * 10 + (*str - '0');  // Jede Ziffer umwandeln und addieren
			str++;
			length-=1;
		}
	}
	if(minus){
		result=result*(-1);
	}
	return result;
}


int stringToInt(std::string string,int length=-1,int startpos=-1) {
	const char* str=string.c_str();
	if(startpos==-1){
		startpos=0;
	}
	int result = 0;
	bool minus=false;
	str=str+startpos;
	// Prüfen, ob die Eingabe leer ist
	if (str == nullptr || *str == '\0') {
		throw std::invalid_argument("Ungueltige Eingabe bei stringToInt: Die Zeichenkette darf nicht leer sein.");
	}
	if(length==-1){
		if(*str=='-'){
			minus=true;
			str++;
		}
		while(*str){
			if (*str == '\r' || *str == '\0' || *str == '\n') {
				break;
			}
			if (*str < '0' || *str > '9') {
				std::string error="Ungueltige Eingabe bei der Funktion stringToInt. Eingabe enthaelt nicht-numerische Zeichen \'";
				error+=*str;
				error+="\' .\n";
				throw std::invalid_argument(error);
			}
			result = result * 10 + (*str - '0');  // Jede Ziffer umwandeln und addieren
			str++;
		}
	}
	else{
		while(length>0){
			if (*str < '0' || *str > '9') {
				std::string error="Ungueltige Eingabe bei der Funktion stringToInt. Eingabe enthaelt nicht-numerische Zeichen \'";
				error+=*str;
				error+="\' .\n";
				throw std::invalid_argument(error);
			}
			result = result * 10 + (*str - '0');  // Jede Ziffer umwandeln und addieren
			str++;
			length-=1;
		}
	}
	if(minus){
		result=result*(-1);
	}
	return result;
}

int random(int max, int min=0) {
	return min + rand() % (max - min + 1);
}
