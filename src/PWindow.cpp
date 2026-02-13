#include <conio.h> 
class PWindow{
	//---------------------------------
	//####PGF=PANZERS GAME FRAMEWORK###
	//--------------------------------
	inline static HBRUSH background=CreateSolidBrush(RGB(255,255,255));
	HWND window;
	char* title=nullptr;
	inline static int width=0;
	inline static int height=0;
	inline static HCURSOR cursor;
	Thread updatetick;
	Thread cmdThread;
	unsigned long long int currenttick=0;
	inline static Mainframe drawframe;
	inline static bool gui_mode=true;
	bool no_gui_started=false;
	PArray<string> commands={"help","setScene","drawStruc","compare","testScene_time","testScene_collision","testScene_update","showStructures","setFocus","turnMapOn","showCollisions","start","getUpdates"};
	char inputbuffer[256];

	bool tickPass(){
		uint64_t MS_PER_TICK = (1000 / 60);  // the time one game-tick represents
		uint64_t currentTime=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		if((currenttick + MS_PER_TICK)< currentTime){
			currenttick = currentTime+ MS_PER_TICK;
			return true;
		} 
		return false;
	}

	void updateThread(){
		while(true){
			refresh(0);
			Sleep(50);
		}
	}
	
	void updateThreadNoGui(){
		while(!no_gui_started){}
		while(true){
			try{
				drawframe.paintNoGui();
			}
		catch(string& exception){
			std::cout<<exception<<"\n";
		}
		catch(const char* exception){
			std::cout<<exception<<"\n";
		}
		catch(char* exception){
			std::cout<<exception<<"\n";
		}
		}
	}
	
	static void initThreadStatic(PWindow* instance){
		instance->updateThread();
	}
	
	static void initThreadStaticNoGui(PWindow* instance){
		instance->updateThreadNoGui();
	}
	
	bool stringToBool(string& string_bool){
		if(string_bool=="true"){
			return true;
		}
		return false;
	}
	
	void drawStructure(string boolean){
		if(boolean=="true"){
			drawframe.drawStructure(true);
			return;
		}
		drawframe.drawStructure(false);
	}
	
	void handleCommand(size_t command,LinkedList<string>& params,size_t& parameter_size){
		switch(command){
			case 0:
				std::cout<<"Ungueltiger Befehl! Nutze /help um alle Befehle anzuzeigen.\n";
				break;
			case 1:
				std::cout<<"###############################################Befehle###############################################\n";
				std::cout<<"/help                               -> Zeigt die Befehle an\n";
				std::cout<<"/drawStruc true|false               -> Zeichnet die Datenstruktur\n";
				std::cout<<"/testScene_update Anzahl_Updates \"SzenenName\"	-> Wechselt zu \"SzenenName\" fuer gewisse Anzahl an Updates\n";
				std::cout<<"/turnMapOn true|false					-> Zeichnet die Map\n";
				std::cout<<"/showCollisions true|false				-> Zeichnet die Kollisionen\n";
				std::cout<<"/start				-> Starte Szene ohne GUI (nur möglich bei keiner GUI)\n";
				std::cout<<"/getUpdates							-> Zeigt den Updatecounter an\n";
				std::cout<<"#####################################################################################################\n";
				break;
			case 3:
				if(parameter_size==1){
					drawStructure(params.element());
					return;
				}
				std::cout<<"Ungueltiger Befehl! Nutze den Befehl wie folgt:\n";
				std::cout<<"/drawStruc true|false               -> Zeichnet die Datenstruktur\n";
				break;
			case 7:
				if(parameter_size==2){
					unsigned long long updates=stringToInt(params.element());
					string scenename=(params.next())->element();
					drawframe.full_clear(1,2);
					drawframe.update_set_function(updates,scenename);
					return;
				}
				std::cout<<"Ungueltiger Befehl! Nutze den Befehl wie folgt:\n";
				std::cout<<"/testScene_update Anzahl_Updates \"SzenenName\"	-> Wechselt zu \"SzenenName\" fuer gewisse Anzahl an Updates\n";
				break;
			case 10:
				if(parameter_size==1){
					drawframe.turnMapOn(stringToBool(params.element()));
					return;
				}
				std::cout<<"Ungueltiger Befehl! Nutze den Befehl wie folgt:\n";
				std::cout<<"/turnMapOn true|false					-> Zeichnet die Map\n";
				break;
			case 11:
				if(parameter_size==1){
					drawframe.showCollisions(stringToBool(params.element()));
					return;
				}
				std::cout<<"Ungueltiger Befehl! Nutze den Befehl wie folgt:\n";
				std::cout<<"/showCollisions true|false				-> Zeichnet die Kollisionen\n";
				break;
			case 12:
				if(parameter_size==0){
					if(!gui_mode){
						no_gui_started=true;
					}
					else{
						std::cout<<"/start kann nicht ausgefuehrt werden, da das Programm im GUI-Mode ist!\n";
					}
					return;
				}
				std::cout<<"Ungueltiger Befehl! Nutze den Befehl wie folgt:\n";
				std::cout<<"/start				-> Starte Szene ohne GUI (nur möglich bei keiner GUI)\n";
				break;
			case 13:
				if(parameter_size==0){
					std::cout<<"Current Updatecounter: "<<drawframe.getUpdates()<<"\n";
					return;
				}
				std::cout<<"Ungueltiger Befehl! Nutze den Befehl wie folgt:\n";
				std::cout<<"/getUpdates				-> Zeigt den Updatecounter an\n";
				break;
		}
	}
	
	void getParameters(string& input,size_t& command_length,LinkedList<string>& params,size_t& parameter_size){
		if(command_length==input.len()){
			return;
		}
		size_t string_size=0;
		size_t string_start=command_length+1;
		for(size_t i=command_length+1;i<input.len();i++){
			if(input[i]==' '){
				if((i-string_start)>0){
					string_size=i-string_start;
					string parameter(string_size);
					for(size_t k=0;k<string_size;k++){
						parameter[k]=input[string_start+k];
					}
					params.add(parameter);
					parameter_size++;
					string_start=i+1;
				}
			}
		}
		if((input.len()-string_start)>0){
			string_size=input.len()-string_start;
			string parameter(string_size);
			for(size_t k=0;k<string_size;k++){
				parameter[k]=input[string_start+k];
			}
			params.add(parameter);
			parameter_size++;
		}
	}
	
	void handleInput(string& input){
		if(input.len()>0){
			if(input[0]!='/'){
				return;
			}
			size_t command=0;
			size_t command_length=input.len()-1;			// Länge des Befehls ohne Input und ohne '/'
			for(size_t i=0;i<input.len();i++){
				if(input[i]==' '){
					command_length=i-1;
					break;
				}
			}
			for(size_t i=0;i<commands.elements();i++){
				if(commands[i].len()==command_length){
					command=i+1;
					for(size_t j=0;j<commands[i].len();j++){
						if(commands[i][j]!=input[1+j]){
							command=0;
							break;
						}
					}
					if(command!=0){
						break;
					}
				}
			}
			LinkedList<string> params;
			size_t parameter_size=0;
			if(command!=0){
				command_length++;
				getParameters(input,command_length,params,parameter_size);
			}
			handleCommand(command,params,parameter_size);
			params.clear();
		}
	}
	
	void getCmdLine(string& input){
		std::cin.getline(inputbuffer, 256);
		input=inputbuffer;
	}
	
	void checkCMD(){
		string eingabe;
		while(true){
			std::cout << "Bitte gib etwas ein: ";
			getCmdLine(eingabe);
			if(eingabe.len()>0){
				std::cout << "Du hast eingegeben: " << eingabe << std::endl;
				handleInput(eingabe);
			}
			

		}
	}

	void initThreads(){
		if(gui_mode){
			updatetick.create(initThreadStatic,this);
			updatetick.detach();
			cmdThread.create(checkCMD,this);
			cmdThread.detach();
			std::cout<<"GUI Initialisiert!"<<"\n";
			return;
		}
		else{
			updatetick.create(initThreadStaticNoGui,this);
			updatetick.detach();
			std::cout<<"NO GUI-Mode Initalisiert!"<<"\n";
			checkCMD();
			return;
		}
	}
	
	void init(){
		initThreads();
	}

	static void drawWindow(HWND hwnd){
		PAINTSTRUCT     ps;
		HDC             hdc;
		HDC				hdcMem;
		HBITMAP			bitmap;
		//----Create-Draw-Objects---------------------
		hdc = BeginPaint(hwnd, &ps);
		hdcMem = CreateCompatibleDC(hdc);
		bitmap = CreateCompatibleBitmap(hdc, width,height);
		SelectObject(hdcMem,bitmap);
		//------Draw-Background------------------------ 
		FillRect(hdcMem,&ps.rcPaint,background);
		//------Draw-Main------------------------------
		try{
		drawframe.paint(hdcMem);			// TODO HIER
		}
		catch(string& exception){
			std::cout<<exception<<"\n";
		}
		catch(const char* exception){
			std::cout<<exception<<"\n";
		}
		catch(char* exception){
			std::cout<<exception<<"\n";
		}
		//------FinishDraw------------------------------
		BitBlt(hdc, 0, 0, width,height, hdcMem, 0, 0, SRCCOPY);
		DeleteDC(hdcMem);
		//------End-Drawing---------------------------
		EndPaint(hwnd, &ps);
	}
	
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {    
		switch (uMsg) {
			case WM_CREATE:
				break;
			case WM_COMMAND:
				break;
			case WM_KEYDOWN:
				break;
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			case WM_ERASEBKGND:
				break;
			case WM_LBUTTONDOWN:
				break;
			case WM_LBUTTONUP:
				break;
			case WM_MBUTTONDOWN:
				break;
			case WM_MBUTTONUP:
				break;
			case WM_RBUTTONDOWN:
				break;
			case WM_RBUTTONUP:
				break;
			case WM_PAINT:
				drawWindow(hwnd);
				break;
			case WM_NCPAINT:
				drawWindow(hwnd);
				break;
			case WM_SETCURSOR:{
				SetCursor(cursor);
				break;
			}
		}
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	
	void centerWindow(HWND parent_window, int width, int height,int* position){
		RECT rect;
		GetClientRect(parent_window, &rect);
		rect.left = (rect.right/2) - (width/2);
		rect.top = (rect.bottom/2) - (height/2);
		position[0]=rect.left;
		position[1]=rect.top;
	}
	
	
	void hide(HWND application){
		ShowWindow(application,SW_HIDE);
	}

	void show(HWND application){
		ShowWindow(application,SW_SHOW);
	}
	
	void refresh(int id){
		if(window!=NULL){
			switch(id){
				case 0:
					//Refresh wenn man in der Queue dran kommt
					RedrawWindow(window, NULL, NULL, RDW_INVALIDATE);
					return;
				case 1:
					//Refresh ohne auf Queue zu achten
					RedrawWindow(window,0,0,RDW_INVALIDATE | RDW_UPDATENOW);
					return;
			}
		}
	}
	
	void setTitleString(const char* string){
		if(title!=nullptr){
			delete[] title;
			title=nullptr;
		}
		title= new char[strlen(string)];
		for(int i=0;i<strlen(string);i++){
			title[i]=string[i];
		}
		title[strlen(string)]='\0';
	}
	
	HBITMAP createBitmapFromPixelArray(uint8_t* pixels, size_t width, size_t height) {
		return CreateBitmap(width, height, 1, 32, pixels);
	}
	
	HCURSOR createCursor(uint8_t* pixels,size_t width,size_t height){
		HBITMAP hBitmap = createBitmapFromPixelArray(pixels, width, height);
		ICONINFO iconInfo = {0};
		iconInfo.fIcon = FALSE; // FALSE für Cursor, TRUE für Icon
		iconInfo.xHotspot = 0;  // x-Hotspot position des Cursors
		iconInfo.yHotspot = 0;  // y-Hotspot position des Cursors
		iconInfo.hbmMask = hBitmap;
		iconInfo.hbmColor = hBitmap;
		return CreateIconIndirect(&iconInfo);
	}
	
	void changeCursor(uint8_t* pixels,size_t width,size_t height){
		if(width>32 || height>32){
			return;
		}
		cursor=createCursor(pixels,width,height);
		SetCursor(cursor);
	}
	
	public:
	
	virtual void center() {
		RECT rect;
		GetWindowRect(window, &rect);

		int windowWidth  = rect.right - rect.left;
		int windowHeight = rect.bottom - rect.top;

		// Bildschirmgröße holen
		int screenWidth  = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		// Neue Position berechnen
		int posX = (screenWidth  - windowWidth)  / 2;
		int posY = (screenHeight - windowHeight) / 2;

		// Fenster verschieben
		SetWindowPos(window, HWND_TOP, posX, posY, 0, 0, SWP_NOSIZE);
	}
	
	virtual void setDrawing(bool boolean){
		drawframe.setDrawing(boolean);
	}
	
	virtual void setSize(int newwidth,int newheight){
		this->width=width;
		this->height=height;
		SetWindowPos(window,NULL,0,0,newwidth,newheight,SWP_NOMOVE);
	}
	
	void getPosition(int* position){
		RECT rect;
		GetWindowRect(window,&rect);
		position[0]=rect.left;
		position[1]=rect.top;
	}
	
	void setBackground(int red,int green,int blue){
		background=CreateSolidBrush(RGB(red,green,blue));
	}

	void setTitle(const char* string){
		if(window==NULL){
			return;
		}
		setTitleString(string);
		SetWindowText(window, title);
	}
	
	void setVisible(bool boolean){
		if(boolean){
			show(window);
			return;
		}
		hide(window);
	}
	
	// Funktion zum Setzen der Resizability des Fensters
	void setResizable(bool resizable) {
		if(window!=NULL){
			LONG style = GetWindowLong(window, GWL_STYLE);
			if (resizable) {
				style |= WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
			} else {
				style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX);
			}
			SetWindowLong(window, GWL_STYLE, style);
			SetWindowPos(window, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
		}
	}
	
	template<typename objecttype>
	void add(objecttype* object){
		drawframe.add(object);
	}
	
	static void loadScene(const char* scenename){
		try{
			drawframe.loadScene(scenename,gui_mode);
		}
		catch(string& exception){
			std::cout<<exception<<"\n";
		}
		catch(const char* exception){
			std::cout<<exception<<"\n";
		}
		catch(char* exception){
			std::cout<<exception<<"\n";
		}
	}
	
	void setGUI(bool boolean){
		gui_mode=boolean;
		drawframe.setGUI(boolean);
	}
	
	void initDrawframe(int width,int height){
		drawframe.setSize(width,height);
	}
	
	void setFocusStruc(int newid){
		drawframe.setFocusStruc(newid);
	}
	
	
	void clearing(){
		drawframe.clear();
	}
	
	int createGUI(int width=0,int height=0,const char* title="Game",HINSTANCE hInstance=GetModuleHandle(NULL), HINSTANCE hPrevInstance=NULL, LPSTR pCmdLine=NULL, int nCmdShow=1)
	{
		this->width=width;
		this->height=height;
		const char CLASS_NAME[]  = "Application";
		WNDCLASS wc = { };
		wc.lpfnWndProc   = WindowProc;
		wc.hInstance     = hInstance;
		wc.lpszClassName = CLASS_NAME;
		RegisterClass(&wc);
		int position[2];
		centerWindow(GetDesktopWindow(), width, height,position);
		int posx=position[0];
		int posy=position[1];
		setTitleString(title);
		init();
		window = CreateWindowEx(			// Create the window.
			0,                              // Optional window styles.
			CLASS_NAME,                     // Window class, Register by RegisterClass
			title,    						// Window text
			WS_OVERLAPPEDWINDOW,            // Window style
			posx, posy, width, height,		// Size and position
			NULL,       					// Parent window    
			NULL,       					// Menu
			hInstance,  					// Instance handle
			NULL        					// Additional application data
			);
		if (window == NULL){
			return 0;
		}
		center();
		setResizable(false);
		ShowWindow(window, nCmdShow);		//ShowWindow(GetConsoleWindow(), SW_HIDE); Verstecke Console
		MSG msg = { };						// Run the message loop.
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return 0;
	}
	
	int createNoGui(int width,int height){
		this->width=width;
		this->height=height;
		init();
		return 0;
	}
	
	int create(int width=0,int height=0)
	{
		if(gui_mode){
			return createGUI(width,height);
		}
		return createNoGui(width,height);
	}
	
	Basicobject* operator[](max_uint index){
		return drawframe[index];
	}
	
	PWindow(){}
	
	~PWindow(){
		if(title!=nullptr){
			delete[] title;
		}
		
	}
	
};