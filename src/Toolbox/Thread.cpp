#include <thread>
class Thread{
	
	std::thread customthread;
	std::thread::native_handle_type nativehandle;
	bool alife=false;
	
	void stop(){
		if(alife){ 
			if(customthread.joinable()){ 
				customthread.join();
			} 
			alife = false; 
		}
	}
	
public:
		
	//Mach weiter auch wenn Thread läuft
	void detach(){
		customthread.detach();
	}
	
	//Warte bis Thread fertig ist
	void join(){
		customthread.join();
	}
	
	template <class Function>
	void create(Function&& function){
		if(alife==false){
			alife=true;
			customthread=std::thread(function);
			nativehandle=customthread.native_handle();
			tune();
		}
	}
	
	template <class Function,class... Args>
	void create(Function&& function,Args&&... args){
		if(alife==false){
			alife=true;
			customthread=std::thread(function,args...);
			nativehandle=customthread.native_handle();
			tune();
		}
	}
	
	bool isAlife(){
		return alife;
	}
	
	void tune(DWORD priority = THREAD_PRIORITY_HIGHEST, DWORD affinity = 1, DWORD idealProcessor = 0) {
		if(nativehandle) {
			SetThreadPriority(reinterpret_cast<HANDLE>(nativehandle), priority);
			SetThreadAffinityMask(reinterpret_cast<HANDLE>(nativehandle), affinity);
			SetThreadIdealProcessor(reinterpret_cast<HANDLE>(nativehandle), idealProcessor);
		}
	}

		
	Thread(){};
	
	template <class Function>
	Thread(Function&& function){
		alife=true;
		customthread=std::thread(function);
		nativehandle=customthread.native_handle();
		tune();
	}
	
	template <class Function,class... Args>
	Thread(Function&& function,Args&&...args){
		alife=true;
		customthread=std::thread(function,args...);
		nativehandle=customthread.native_handle();
		tune();
	}
	
	template <class Function>
	Thread(int id,Function&& function){
		switch(id){
			//Warte bis Thread fertig ist
			case 0:
				alife=true;
				customthread=std::thread(function);
				nativehandle=customthread.native_handle();
				tune();
				customthread.join();
				break;
			//Mach weiter auch wenn Thread läuft
			case 1:
				alife=true;
				customthread=std::thread(function);
				nativehandle=customthread.native_handle();
				tune();
				customthread.detach();
				break;
			default:
				std::cout<<"Thread konnte nicht erstellt werden!"<<"\n";
				break;
		}
	}
	
	template <class Function,class... Args>
	Thread(int id,Function&& function,Args&&... args){
		switch(id){
			//Warte bis Thread fertig ist
			case 0:
				alife=true;
				customthread=std::thread(function,args...);
				nativehandle=customthread.native_handle();
				tune();
				customthread.join();
				break;
			//Mach weiter auch wenn Thread läuft
			case 1:
				alife=true;
				customthread=std::thread(function,args...);
				nativehandle=customthread.native_handle();
				tune();
				customthread.detach();
				break;
			default:
				std::cout<<"Thread konnte nicht erstellt werden!"<<"\n";
				break;
		}
	}
	
	~Thread(){
		stop();
	}
	
};