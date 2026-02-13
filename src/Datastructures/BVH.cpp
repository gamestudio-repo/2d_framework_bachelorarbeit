template<typename objecttype,int struc_id=0,typename object_form=rect<2>>
class BVH:public StructureInterface<objecttype,struc_id,object_form>{
	object_form form;
	objecttype element=nullptr;
	BVH* node_left=nullptr;
	BVH* node_right=nullptr;
	bool expanded=false;
	
	static const int REBUILD_LIMIT=100;
	int REBUILD_COUNTER=0;
	static const bool REBUILD_METHOD_ON=true;
	static const int BUFFER_EXPANDER_LIMIT = 10;
	
	bool isLeaf() const {
		return ((node_left==nullptr) && (node_right==nullptr));
	}
	
	void replaceWithNode(BVH* new_node) {
		if (new_node != nullptr) {
			form     = new_node->form;
			element  = new_node->element;
			expanded = new_node->expanded;
			node_left=new_node->node_left;
			node_right=new_node->node_right;
			new_node->node_left=nullptr;
			new_node->node_right=nullptr;
		}
	}


	
	void merge(PArray<objecttype>& array, int left, int mid, int right, int axis) {
		int size_left = mid - left + 1;
		int size_right = right - mid;
		PArray<objecttype> L(size_left);
		PArray<objecttype> R(size_right);
		for (int i = 0; i < size_left; i++)
			L[i] = array[left + i];
		for (int j = 0; j < size_right; j++)
			R[j] = array[mid + 1 + j];
		int i = 0, j = 0, k = left;
		while (i < size_left && j < size_right) {
			rect<2> rect_left = L[i]->getRect();
			rect<2> rect_right = R[j]->getRect();

			if (rect_left.pos[axis] <= rect_right.pos[axis]) {
				array[k] = L[i];
				i++;
			} else {
				array[k] = R[j];
				j++;
			}
			k++;
		}
		while (i < size_left) {
			array[k++] = L[i++];
		}
		while (j < size_right) {
			array[k++] = R[j++];
		}
	}
	
	void mergeSort(PArray<objecttype>& elements,int left, int right, int axis = 0){
		if (left < right) {
			int mid = left + (right - left) / 2;
			mergeSort(elements, left, mid, axis);
			mergeSort(elements, mid + 1, right, axis);
			merge(elements, left, mid, right, axis);
		}
	}
		
	void stableSortByAxis(PArray<objecttype>& arr, int axis) {
		size_t n = arr.elements();
		if (n <= 1) return;

		size_t mid = n / 2;
		PArray<objecttype> left(mid);
		PArray<objecttype> right(n - mid);

		for (size_t i = 0; i < mid; ++i) left[i] = arr[i];
		for (size_t i = mid; i < n; ++i) right[i - mid] = arr[i];

		stableSortByAxis(left, axis);
		stableSortByAxis(right, axis);

		// Merge stabil
		size_t i = 0, j = 0, k = 0;
		while (i < left.elements() && j < right.elements()) {
			auto li = left[i]->getRect().pos[axis];
			auto rj = right[j]->getRect().pos[axis];
			if (li <= rj) {
				arr[k++] = left[i++];
			} else {
				arr[k++] = right[j++];
			}
		}
		while (i < left.elements()) arr[k++] = left[i++];
		while (j < right.elements()) arr[k++] = right[j++];
	}

	size_t medianIndex(PArray<objecttype>& arr, int axis) {
		stableSortByAxis(arr, axis);
		return arr.elements() / 2;
	}

	
	void resetForm(){
		form[0]=0;
		form[1]=0;
		form.pos[0]=0;
		form.pos[1]=0;
	}
	
	void updateForm() {
		expanded=false;
		resetForm();
		if (node_left && node_right) {
			expand(node_left->form);
			expand(node_right->form);
		} else if (node_left) {
			expand(node_left->form);
		} else if (node_right) {
			expand(node_right->form);
		}
	}
	
	void expand(const object_form& objectform) {
		if(!expanded){
			form=objectform;
			expanded=true;
			return;
		}
		form.expand(objectform);
	}
	
	void expand_buffer_form(object_form& expaned_form,const objecttype& object){
		expaned_form=object->getRect();
		position<int> expanding={BUFFER_EXPANDER_LIMIT*(object->getSpeed()),BUFFER_EXPANDER_LIMIT*(object->getSpeed())};
		for(uint8_t i=0;i<2;i++){
			expaned_form[i]+=expanding[i];
			if((expaned_form.pos[i]-(expanding[i]/2))>=0){
				expaned_form.pos[i]-=expanding[i]/2;
			}
			else{
				expaned_form.pos[i]=0;
			}
		}
	}
	
	void expand_buffer(const objecttype& object) {
		object_form expaned_form;
		expand_buffer_form(expaned_form,object);
		if(!expanded){
			form=expaned_form;
			expanded=true;
			return;
		}
		form.expand(expaned_form);
	}
	
		// Hilfsfunktion: Achse mit größter Ausdehnung wählen
	int chooseSplitAxis(const object_form& bounds) {
		return (bounds[0] >= bounds[1]) ? 0 : 1;
	}

	// Interne Insert-Logik, die auch Build nutzt
	void insertInternal(PArray<objecttype>& objs) {
		size_t count = objs.elements();

		// Blattfall
		if (count == 1) {
			element = objs[0];
			expand_buffer(objs[0]);
			return;
		}

		// Bounding-Box berechnen
		expanded = false;
		for (size_t i = 0; i < count; ++i)
			expand_buffer(objs[i]);

		// Achse wählen
		int axis = chooseSplitAxis(form);

		// Median finden
		size_t mid = medianIndex(objs, axis);

		// Arrays für Kinder
		PArray<objecttype> leftArr, rightArr;
		leftArr.create(mid);
		for (size_t i = 0; i < mid; ++i) leftArr.add(objs[i]);
		rightArr.create(count - mid);
		for (size_t i = mid; i < count; ++i) rightArr.add(objs[i]);

		// Kinder anlegen
		if(node_left==nullptr){
			node_left  = new BVH();
		}
		if(node_right==nullptr){
			node_right = new BVH();
		}
		node_left->insertInternal(leftArr);
		node_right->insertInternal(rightArr);
	}

	
	bool intern_remove(const objecttype& object, const object_form& objectform,int mode=0,bool removed=false) {
		if (!expanded || !form.contain(objectform))
			return false;

		// Blattfall
		if (isLeaf()) {
			if (element == object) {
				if(mode!=0){
					if(form.contain(object->getRect())){
						return false;
					}
				}
				element = nullptr;
				expanded = false;
				return true;
			}
			return false;
		}

		// Prüfen, ob Objekt links oder rechts liegt
		if (node_left && node_left->intern_remove(object, objectform,mode))
			removed = true;
		else if (node_right && node_right->intern_remove(object, objectform,mode))
			removed = true;

		if (removed) {
			if(node_left && node_left->isLeaf()){
				if(node_left->element==nullptr){
					delete node_left;
					node_left=nullptr;
					BVH* placeholder=node_right;
					replaceWithNode(placeholder);
					delete placeholder;
					return true;
				}
			}
			if(node_right && node_right->isLeaf()){
				if(node_right->element==nullptr){
					delete node_right;
					node_right=nullptr;
					BVH* placeholder=node_left;
					replaceWithNode(placeholder);
					delete placeholder;
					return true;
				}
			}
		}
		updateForm();
		return removed;
	}
	
	// Hilfsfunktion: Ist dieser Knoten leer?
	bool isEmpty() const {
		if (isLeaf()) {
			return (element == nullptr);
		}
		return ((!node_left || node_left->isEmpty()) && (!node_right || node_right->isEmpty()));
	}
	
	uint8_t chooseNodeInsert(const object_form& object_rect,const object_form& var_1,const object_form& var_2){
		int oldArea1 = var_1.area();
		int oldArea2 = var_2.area();
		
		object_form merged1 = var_1;
		merged1.expand(object_rect);
		int newArea1 = merged1.area();
		
		object_form merged2 = var_2;
		merged2.expand(object_rect);
		int newArea2 = merged2.area();
		
		int cost1 = newArea1 - oldArea1;
		int cost2 = newArea2 - oldArea2;
		if(cost1==cost2){
			if(oldArea1>oldArea2){
				return 1;
			}
			return 0;
		}
		if(cost1>cost2){
			return 1;
		}
		return 0;
	}
	
	// Fügt Objekt in die Datenstruktur ein
	bool intern_insert(const objecttype& object) {
		object_form obj_rect = object->getRect();
		// Fall: leerer Baum
		if (isLeaf()) {
			if(element==nullptr){
				element = object;
				expand_buffer(object);
				return true;
			}
			// Fall: Blatt mit Platz (nur 1 Element bisher)
			else{
				PArray<objecttype> objs;
				objs.create(2);
				objs.add(element);
				objs.add(object);
				element = nullptr;
				insertInternal(objs);
				return true;
			}
		}
		// Fall: Interner Knoten → Kind wählen
		if (node_left && node_left->form.contain(obj_rect)) {
			node_left->intern_insert(object);
		} else if (node_right && node_right->form.contain(obj_rect)) {
			node_right->intern_insert(object);
		} else {
			uint8_t node_choose=0;
			if(node_right==nullptr && node_left==nullptr){
				node_choose=0;
			}
			else if(node_right!=nullptr && node_left!=nullptr){
				node_choose=chooseNodeInsert(obj_rect,node_left->form,node_right->form);
			}
			else if(node_left==nullptr){
				node_choose=1;
			}
			if(node_choose==0){
				if(node_left==nullptr){
					node_left=new BVH();
				}
				node_left->intern_insert(object);
			}
			else{
				if(node_right==nullptr){
					node_right=new BVH();
				}
				node_right->intern_insert(object);
			}
		}
		// Bounding-Box hochpropagieren
		updateForm();
		return true;
	}
	
	public:
	
	void rebuild(){
		if(REBUILD_COUNTER>=REBUILD_LIMIT){
			PArray<objecttype> elements;
			getAllElements(elements);
			build(elements);
			REBUILD_COUNTER=0;
		}
	}
	
	void getAllElements(PArray<objecttype>& array){
		if(isLeaf()){
			if(element!=nullptr){
				array.add(element);
			}
			return;
		}
		if(node_left!=nullptr){
			node_left->getAllElements(array);
		}
		if(node_right!=nullptr){
			node_right->getAllElements(array);
		}
	}
	
	//Erstelle die Datenstruktur durch Objektliste
	bool build(PArray<objecttype>& elements) override {
		clear();
		if (elements.elements() == 0) return false;
		insertInternal(elements);
		return true;
	}
	
	
	// Fügt Objekt in die Datenstruktur ein
	bool insert(const objecttype& object) override {
		if(intern_insert(object)){
			if(REBUILD_METHOD_ON){
				REBUILD_COUNTER++;
				rebuild();
			}
			return true;
		}
		return false;
	}
	
	bool removeByFrame(const objecttype& object, const object_form& old_frame,int mode=0){
		if(intern_remove(object,old_frame,mode)){
			if(REBUILD_METHOD_ON){
				REBUILD_COUNTER++;
				rebuild();
			}
			return true;
		}
		return false;
	}
	
	// Löscht Objekt von der Datenstruktur
	bool remove(const objecttype& object) override {
		if(intern_remove(object,object->getRect())){
			if(REBUILD_METHOD_ON){
				REBUILD_COUNTER++;
				rebuild();
			}
			return true;
		}
		return false;
	}
	
	// Speichert alle gefunden Objekte in "array" und gibt die größe zurück
	size_t query(const object_form& query_rect, PArray<objecttype>& array,objecttype object_pointer=nullptr) override {
		if (!form.intersect(query_rect)) {
			// Dieser BVH-Knoten liegt außerhalb der Anfrage → ignorieren
			return 0;
		}
		size_t found = 0;
		if (isLeaf()) {
			// Blattknoten: prüfe enthaltenes Objekt
			if (element != nullptr && object_pointer!=element) {
				if (query_rect.intersect(element->getRect())) {
					array.add(element);
					found++;
				}
			}
		} else {
			// Interner Knoten: rekursiv weiter prüfen
			if (node_left != nullptr) {
				found += node_left->query(query_rect, array,object_pointer);
			}
			if (node_right != nullptr) {
				found += node_right->query(query_rect, array, object_pointer);
			}
		}
		return found;
	}

	
	// Falls Objekte sich Updaten (Position,Größe,usw.)
	bool notify(const objecttype& object,const object_form& old_frame) override {
		if(intern_remove(object,old_frame,1)){
			if(REBUILD_METHOD_ON){
				REBUILD_COUNTER++;
			}
			if(insert(object)){
				return true;
			} 
			if(REBUILD_METHOD_ON){
				rebuild();
			}
		}
		return false;
	}
	
	// Visuelle Darstellung der Struktur
	void drawStructure(object_form& pixel_frame,int bitwidth,uint8_t* pixels) override {
		uint8_t color[3]={255,0,0};
		if(isLeaf()){
			color[1]=255;
			color[2]=255;
		}
		for(int i=0;i<form[1];i++){
			pixels[bitwidth*(pixel_frame[1]-(i+1+form.pos[1]))+(form.pos[0])*3+2]=color[0];
			pixels[bitwidth*(pixel_frame[1]-(i+1+form.pos[1]))+(form.pos[0])*3+1]=color[1];
			pixels[bitwidth*(pixel_frame[1]-(i+1+form.pos[1]))+(form.pos[0])*3]=color[2];
			pixels[bitwidth*(pixel_frame[1]-(i+1+form.pos[1]))+(form.pos[0]+(form[0]-1))*3+2]=color[0];
			pixels[bitwidth*(pixel_frame[1]-(i+1+form.pos[1]))+(form.pos[0]+(form[0]-1))*3+1]=color[1];
			pixels[bitwidth*(pixel_frame[1]-(i+1+form.pos[1]))+(form.pos[0]+(form[0]-1))*3]=color[2];
		}
		for(int i=0;i<form[0];i++){
			pixels[bitwidth*(pixel_frame[1]-(1+form.pos[1]))+(form.pos[0]+i)*3+2]=color[0];
			pixels[bitwidth*(pixel_frame[1]-(1+form.pos[1]))+(form.pos[0]+i)*3+1]=color[1];
			pixels[bitwidth*(pixel_frame[1]-(1+form.pos[1]))+(form.pos[0]+i)*3]=color[2];
			pixels[bitwidth*(pixel_frame[1]-(1+form.pos[1]+(form[1]-1)))+(form.pos[0]+i)*3+2]=color[0];
			pixels[bitwidth*(pixel_frame[1]-(1+form.pos[1]+(form[1]-1)))+(form.pos[0]+i)*3+1]=color[1];
			pixels[bitwidth*(pixel_frame[1]-(1+form.pos[1]+(form[1]-1)))+(form.pos[0]+i)*3]=color[2];
		}
		if(node_left!=nullptr){
			node_left->drawStructure(pixel_frame,bitwidth,pixels);
		}
		if(node_right!=nullptr){
			node_right->drawStructure(pixel_frame,bitwidth,pixels);
		}
		
	}
	
	// Allgemeine Funktion zum Updaten der Struktur->Falls nötig, z.B. Balanzieren oder Überprüfungen
	void update() override {
		
	}
	
	// Lösche und gebe alle Resourcen frei
	void clear() override {
		if(node_left!=nullptr){
			node_left->clear();
			delete node_left;
			node_left=nullptr;
		}
		if(node_right!=nullptr){
			node_right->clear();
			delete node_right;
			node_right=nullptr;
		}
		REBUILD_COUNTER=0;
		element  = nullptr;
		expanded = false;
		resetForm();
	}
	
	// Lösche und gebe alle Resourcen frei
	void clearComplete() override {
		clear();
	}
	
	BVH(){}
	
	~BVH(){
	}
	
};