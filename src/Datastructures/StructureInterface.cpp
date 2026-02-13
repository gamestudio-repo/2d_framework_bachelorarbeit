template<
	typename objecttype,
	int StructID = 0,
	typename object_form=rect<2>
>
class StructureInterface{
	
	string strucName = "Unknown";
	
	public:
	
	string& getName(){
		return strucName;
	}
	
	int getID() const {
		return StructID;
	}
	
	//###Zu messende Funktionen
	
	//Erstelle die Datenstruktur durch Objektliste
	virtual bool build(PArray<objecttype>& elements){
		return false;
	}
	
	// Fügt Objekt in die Datenstruktur ein
	virtual bool insert(const objecttype& object){
		return false;
	}
	
	// Löscht Objekt von der Datenstruktur
	virtual bool remove(const objecttype& object){
		return false;
	}
	
	// Speichert alle gefunden Objekte in "array" und gibt die größe zurück
	virtual size_t query(const object_form& query_rect,PArray<objecttype>& array,objecttype object_pointer=nullptr){
		return 0;
	}
	
	// Allgemeine Funktion zum Updaten der Struktur->Falls nötig, z.B. Balanzieren oder Überprüfungen
	virtual void update(){}
	
	//###
	
	// Falls Objekte sich Updaten (Position,Größe,usw.)
	virtual bool notify(const objecttype& object,const object_form& old_frame){
		return false;
	}
	
	// Visuelle Darstellung der Struktur
	virtual void drawStructure(object_form& frame,int bitwidth,uint8_t* pixels){}
	
	// Lösche und gebe alle Resourcen frei
	virtual void clear(){}
	
	// Lösche und gebe alle Resourcen frei
	virtual void clearComplete(){}
	
	virtual ~StructureInterface() {}
	
	// Setter
    void setStrucName(const char* name) {
        strucName = name;
        MemoryManager::registerStructName(StructID, strucName);
    }

    // Getter
    static constexpr int getStrucID() { return StructID; }

    // Speicheroperatoren
    static void* operator new(std::size_t size) {
        void* ptr = ::operator new(size);
        MemoryManager::registerAlloc(ptr, size, StructID);
        return ptr;
    }

    static void operator delete(void* ptr) noexcept {
        MemoryManager::unregisterAlloc(ptr);
        ::operator delete(ptr);
    }

    static void* operator new[](std::size_t size) {
        void* ptr = ::operator new[](size);
        MemoryManager::registerAlloc(ptr, size, StructID);
        return ptr;
    }

    static void operator delete[](void* ptr) noexcept {
        MemoryManager::unregisterAlloc(ptr);
        ::operator delete(ptr);
    }
	
};

