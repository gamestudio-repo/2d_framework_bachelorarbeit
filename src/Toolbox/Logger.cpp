#include <iomanip> // for std::put_time
class Logger {
	
	
public:

	template<typename msg_type>
    static void log(const char* pathfile, msg_type msg,uint8_t mode=0) {
        std::fstream file;
        file.open(pathfile, std::ios::out | std::ios::app);
        if (file.is_open()) {
            std::ostringstream oss;
			if(mode==0){
            // Zeitstempel erstellen
				auto now = std::chrono::system_clock::now();
				std::time_t now_time = std::chrono::system_clock::to_time_t(now);
				oss << "[" << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S") << "] ";
			}
            // Nachricht anhängen
            oss << msg << '\n';

            // In Datei schreiben
            file << oss.str();
            file.close();
        }
        // Fehlerbehandlung bewusst minimal gehalten
    }

    // Keine Instanzen erlaubt
    Logger() = delete;
};

