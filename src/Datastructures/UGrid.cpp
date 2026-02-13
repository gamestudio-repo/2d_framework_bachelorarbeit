#pragma once
#include <cmath>
#include <cstdint>

// ========== GridList (keine STL, kein self-delete) ==========
template<typename objecttype,int struc_id=0>
struct GridList {
    objecttype list_element = nullptr;
    GridList* next_list = nullptr;
    GridList* previous_list = nullptr;

    GridList() = default;

    // iterative tail-insert (kein Rekursions-overhead)
    void add(const objecttype& new_element) {
        if (list_element == nullptr) {
            list_element = new_element;
            return;
        }
        GridList* cur = this;
        while (cur->next_list != nullptr) cur = cur->next_list;
        cur->next_list = new GridList();
        MemoryManager::registerAlloc(cur->next_list, sizeof(GridList), struc_id);
        cur->next_list->previous_list = cur;
        cur->next_list->list_element = new_element;
    }

    bool hasElement() const { 
		return list_element != nullptr; 
	}
	
    GridList* next() { 
		return next_list; 
	}
	
    const GridList* next() const { 
		return next_list; 
	}
	
    objecttype& element() { 
		return list_element; 
	}
	
    const objecttype& element() const { 
		return list_element; 
	}

    // Entfernen: gibt Pointer zurück, den Aufrufer löschen muss (oder nullptr wenn nichts zu löschen)
    GridList* pop_return_deleted() {
        if (previous_list != nullptr) {
            // Mittlerer oder letzter Knoten: Verknüpfung anpassen, Caller löscht this
            if (next_list != nullptr) next_list->previous_list = previous_list;
            previous_list->next_list = next_list;
            return this;
        }
        // Head-Knoten
        if (next_list != nullptr) {
            GridList* placeholder = next_list;
            // Kopf durch nächsten ersetzen (Daten kopieren)
            list_element = placeholder->list_element;
            next_list = placeholder->next_list;
            if (next_list != nullptr) next_list->previous_list = this;
            return placeholder; // Caller löscht placeholder
        }
        // Alleine stehender Knoten: nur Inhalt leeren, nichts zu löschen
        list_element = nullptr;
        return nullptr;
    }

    // Lösche komplette Kette (Caller löscht die einzelnen Knoten)
    void clear_chain() {
        GridList* cur = next_list;
        while (cur != nullptr) {
            GridList* nxt = cur->next_list;
            MemoryManager::unregisterAlloc(cur);
            delete cur;
            cur = nxt;
        }
        next_list = nullptr;
        previous_list = nullptr;
        list_element = nullptr;
    }

    ~GridList() = default;
};

// ========== UGrid (keine STL, einfache Deduplikation) ==========
template<typename objecttype,int struc_id=0,typename object_form=rect<2>>
class UGrid : public StructureInterface<objecttype,struc_id,object_form> {
private:
    int tilewidth = 0;
    int tileheight = 0;
    int rows = 0;
    int columns = 0;
    GridList<objecttype,struc_id>** list_array = nullptr;
    bool isMapped = false;
    object_form mapped_form;
	
    static int safeFloor(double v){ 
		return (int)std::floor(v); 
	}
    static int safeCeil(double v){ 
		return (int)std::ceil(v); 
	}

    rect<2> getCheckField(const object_form& f) const {
        rect<2> objectform;
        if (isMapped) {
            if (!mapped_form.intersect(f)) return objectform;
            objectform = mapped_form.getIntersection(f);
        } else {
            objectform = f;
        }

        if (tilewidth <= 0 || tileheight <= 0) return rect<2>();

        int originX = isMapped ? mapped_form.pos[0] : 0;
        int originY = isMapped ? mapped_form.pos[1] : 0;

        double relX = double(objectform.pos[0] - originX);
        double relY = double(objectform.pos[1] - originY);
        double w = double(objectform[0]);
        double h = double(objectform[1]);

        int minX = safeFloor(relX / double(tilewidth));
        int minY = safeFloor(relY / double(tileheight));
        int maxX = safeCeil((relX + w) / double(tilewidth));
        int maxY = safeCeil((relY + h) / double(tileheight));

        if (minX < 0) minX = 0;
        if (minY < 0) minY = 0;
        if (maxX > columns) maxX = columns;
        if (maxY > rows)    maxY = rows;

        if (maxX <= minX || maxY <= minY) return rect<2>(); // leer

        rect<2> field;
        field.pos[0] = minX;
        field.pos[1] = minY;
        field[0] = maxX - minX;
        field[1] = maxY - minY;
        return field;
    }

    // kleine dynamische "seen" Struktur ohne STL: wächst bei Bedarf
    struct SeenArray {
        objecttype* data = nullptr;
        int capacity = 0;
        int count = 0;
        void ensureCapacity(int need) {
            if (capacity >= need) return;
            int newcap = (capacity == 0) ? 16 : capacity * 2;
            while (newcap < need) newcap *= 2;
            objecttype* n = new objecttype[newcap];
            for (int i = 0; i < count; ++i) n[i] = data[i];
            if (data) delete[] data;
            data = n;
            capacity = newcap;
        }
        bool contains(objecttype v) const {
            for (int i = 0; i < count; ++i) if (data[i] == v) return true;
            return false;
        }
        void push(objecttype v) {
            ensureCapacity(count + 1);
            data[count++] = v;
        }
        void clear() { if (data) { delete[] data; data = nullptr; } capacity = 0; count = 0; }
    };

public:

	void getAllElements(PArray<objecttype>& elements){
		PArray<objecttype> placeholder;
		GridList<objecttype,struc_id>* current_list;
		if(list_array!=nullptr){
			for(int y=0;y<rows;y++){
				for(int x=0;x<columns;x++){
					if(list_array[y*columns+x]!=nullptr){
						current_list=list_array[y*columns+x];
						while(current_list!=nullptr){
							if(current_list->hasElement()){
								placeholder.add(current_list->element());
								current_list=current_list->next();
							}
						}
					}
				}
			}
		}
		bool contains;
		for(max_uint i=0;i<placeholder.elements();i++){
			contains=false;
			for(max_uint k=0;k<elements.elements();k++){
				if(placeholder[i]==elements[k]){
					contains=true;
					break;
				}
			}
			if(!contains){
				elements.add(placeholder[i]);
			}
		}
	}

    void setMapped(const object_form& mapped_form_, bool boolean) {
        isMapped = boolean;
        mapped_form = mapped_form_;
    }

    void setSize(int tilewidth_, int tileheight_, int columns_, int rows_) {
        clearComplete();
        tilewidth = tilewidth_;
        tileheight = tileheight_;
        columns = columns_;
        rows = rows_;
        if (columns <= 0 || rows <= 0) {
            list_array = nullptr;
            return;
        }
        list_array = new GridList<objecttype,struc_id>*[rows * columns];
        MemoryManager::registerAlloc(list_array, sizeof(GridList<objecttype,struc_id>*) * rows * columns, struc_id);
        for (int i = 0; i < rows * columns; ++i) {
            list_array[i] = new GridList<objecttype,struc_id>();
            MemoryManager::registerAlloc(list_array[i], sizeof(GridList<objecttype,struc_id>), struc_id);
        }
    }

    bool build(PArray<objecttype>& elements) override {
        clear();
        bool all_insert = true;
        for (int i = 0; i < elements.elements(); ++i) {
            if (!insert(elements[i])) all_insert = false;
        }
        return all_insert;
    }

    bool insert(const objecttype& object) override {
        if (!list_array) return false;
        rect<2> check = getCheckField(object->getRect());
        if (check[0] == 0 || check[1] == 0) return false;
        bool inserted = false;
        for (int y = 0; y < check[1]; ++y) {
            for (int x = 0; x < check[0]; ++x) {
                int idx = (y + check.pos[1]) * columns + (x + check.pos[0]);
                GridList<objecttype,struc_id>* cell = list_array[idx];
                // prüfe Kopien im selben Zellkopf
                bool present = false;
                GridList<objecttype,struc_id>* cur = cell;
                while (cur) {
                    if (cur->hasElement() && cur->element() == object) { present = true; break; }
                    cur = cur->next();
                }
                if (!present) {
                    cell->add(object);
                    inserted = true;
                }
            }
        }
        return inserted;
    }

    bool remove_intern(const objecttype& object, const object_form& old_frame, int mode = 0) {
        if (!list_array) return false;
        rect<2> check = getCheckField(old_frame);
        if (check[0] == 0 || check[1] == 0) return false;

        if (mode != 0) {
            rect<2> now = getCheckField(object->getRect());
            if (now == check) return false; // keine Zelländerung, nichts zu tun
        }

        bool removedAny = false;
        for (int y = 0; y < check[1]; ++y) {
            for (int x = 0; x < check[0]; ++x) {
                int idx = (y + check.pos[1]) * columns + (x + check.pos[0]);
                GridList<objecttype,struc_id>* cur = list_array[idx];
                // iteriere auf sichere Weise: wenn ein Knoten gelöscht wurde, wir fangen neu an für diese Zelle
                bool restart = true;
                while (restart) {
                    restart = false;
                    cur = list_array[idx];
                    while (cur != nullptr) {
                        if (cur->hasElement() && cur->element() == object) {
                            GridList<objecttype,struc_id>* toDelete = cur->pop_return_deleted();
                            if (toDelete != nullptr) {
                                MemoryManager::unregisterAlloc(toDelete);
                                delete toDelete;
                                removedAny = true;
                                restart = true;
                                break; // restart scan for this cell
                            } else {
                                // nur Wert gelöscht, setze flag und weiter suchen
                                removedAny = true;
                            }
                        }
                        cur = cur->next();
                    }
                }
            }
        }
        return removedAny;
    }

    bool remove(const objecttype& object) override {
        return remove_intern(object, object->getRect(), 0);
    }

    size_t query(const object_form& query_rect, PArray<objecttype>& array, objecttype object_pointer = nullptr) override {
        if(list_array==nullptr){
			return 0;
		}
		size_t found=0;
		GridList<objecttype,struc_id>* current_list=nullptr;
		objecttype object_placeholder=nullptr;
		rect<2> check_rect=getCheckField(query_rect);
		if (check_rect[0] == 0 || check_rect[1] == 0){
			return 0;
		}
		for(int y=0;y<check_rect[1];y++){
			for(int x=0;x<check_rect[0];x++){
				current_list=list_array[(y+check_rect.pos[1])*columns+(x+check_rect.pos[0])];
				while(current_list!=nullptr){
					if(current_list->hasElement()){
						object_placeholder=current_list->element();
						if(query_rect.intersect(object_placeholder->getRect())){
							for(int i=0;i<array.elements();i++){
								if(array[i]==object_placeholder){
									object_placeholder=nullptr;
									break;
								}
							}
							if(object_placeholder!=nullptr && object_pointer!=object_placeholder){
								array.add(object_placeholder);
								found++;
							}
						}
					}
					current_list=current_list->next();
				}
			}
		}
		return found;
    }

    bool notify(const objecttype& object, const object_form& old_frame) override {
        bool removed = remove_intern(object, old_frame, 1);
        if (removed) return insert(object);
        return false;
    }

    void drawStructure(object_form& pixel_frame,int bitwidth,uint8_t* pixels) override {
        if (!list_array) return;
        uint8_t color[3] = {255,0,0};
        int draw_rows = rows;
        int draw_columns = columns;
        int start_x = 0;
        int start_y = 0;
        if (isMapped) { start_x = mapped_form.pos[0]; start_y = mapped_form.pos[1]; }
        if ((start_y + rows*tileheight) > pixel_frame[1]) {
            draw_rows = (((rows*tileheight)-((start_y+rows*tileheight)-pixel_frame[1]))/tileheight);
        }
        if ((start_x + columns*tilewidth) > pixel_frame[0]) {
            draw_columns = (((columns*tilewidth)-((start_x+columns*tilewidth)-pixel_frame[0]))/tilewidth);
        }
        for (int y = 0; y < draw_rows; ++y) {
            for (int x = 0; x < draw_columns; ++x) {
                for (int w = 0; w < tilewidth; ++w) {
                    pixels[bitwidth*(pixel_frame[1]-(y*tileheight+1+start_y))+(x*tilewidth+w+start_x)*3+2]=color[0];
                    pixels[bitwidth*(pixel_frame[1]-(y*tileheight+1+start_y))+(x*tilewidth+w+start_x)*3+1]=color[1];
                    pixels[bitwidth*(pixel_frame[1]-(y*tileheight+1+start_y))+(x*tilewidth+w+start_x)*3+0]=color[2];
                    if (y+1<draw_rows){
                        pixels[bitwidth*(pixel_frame[1]-((y+1)*tileheight+1+start_y))+(x*tilewidth+w+start_x)*3+2]=color[0];
                        pixels[bitwidth*(pixel_frame[1]-((y+1)*tileheight+1+start_y))+(x*tilewidth+w+start_x)*3+1]=color[1];
                        pixels[bitwidth*(pixel_frame[1]-((y+1)*tileheight+1+start_y))+(x*tilewidth+w+start_x)*3+0]=color[2];
                    }
                }
                for (int h = 0; h < tileheight; ++h) {
                    pixels[bitwidth*(pixel_frame[1]-(y*tileheight+1+h+start_y))+(x*tilewidth+start_x)*3+2]=color[0];
                    pixels[bitwidth*(pixel_frame[1]-(y*tileheight+1+h+start_y))+(x*tilewidth+start_x)*3+1]=color[1];
                    pixels[bitwidth*(pixel_frame[1]-(y*tileheight+1+h+start_y))+(x*tilewidth+start_x)*3+0]=color[2];
                    if (x+1<draw_columns){
                        pixels[bitwidth*(pixel_frame[1]-(y*tileheight+1+h+start_y))+((x+1)*tilewidth+start_x)*3+2]=color[0];
                        pixels[bitwidth*(pixel_frame[1]-(y*tileheight+1+h+start_y))+((x+1)*tilewidth+start_x)*3+1]=color[1];
                        pixels[bitwidth*(pixel_frame[1]-(y*tileheight+1+h+start_y))+((x+1)*tilewidth+start_x)*3+0]=color[2];
                    }
                }
            }
        }
    }

    void update() override { /* optional */ }

    void clear() override {
        if (list_array==nullptr) return;
        for (int i = 0; i < rows*columns; ++i) {
            if (list_array[i]) list_array[i]->clear_chain();
        }
    }

    void clearComplete() override {
        if (!list_array) return;
        for (int i = 0; i < rows*columns; ++i) {
            if (list_array[i]) {
                list_array[i]->clear_chain();
                MemoryManager::unregisterAlloc(list_array[i]);
                delete list_array[i];
                list_array[i] = nullptr;
            }
        }
        MemoryManager::unregisterAlloc(list_array);
        delete[] list_array;
        list_array = nullptr;
        isMapped = false;
        tilewidth = tileheight = rows = columns = 0;
    }
	
		// Konstruktor: direkte Spalten/Zeilen-Angabe
	UGrid(int tilewidth_, int tileheight_, int columns_, int rows_) {
		list_array = nullptr;
		setSize(tilewidth_, tileheight_, columns_, rows_);
	}

	// Konstruktor: Breite/Höhe in Pixeln plus Tile-Größe -> berechnet columns/rows
	UGrid(int tilewidth_, int tileheight_, int width, int height, int /*size_unused*/) {
		list_array = nullptr;
		int columns_ = 0;
		int rows_ = 0;
		if (tilewidth_ > 0 && tileheight_ > 0) {
			columns_ = (int)std::ceil(double(width) / double(tilewidth_));
			rows_ = (int)std::ceil(double(height) / double(tileheight_));
		}
		setSize(tilewidth_, tileheight_, columns_, rows_);
	}

	// Default-Konstruktor (bereits vorhanden, hier zur Vollständigkeit)
	UGrid() {
		list_array = nullptr;
	}


    ~UGrid() = default;
};
