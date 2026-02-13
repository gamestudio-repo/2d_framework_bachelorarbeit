#include <unordered_map>
#include <mutex>

class MemoryManager {
    struct AllocationInfo {
        std::size_t size;
        int structID;
    };

    static inline std::unordered_map<void*, AllocationInfo> allocations;
    static inline std::unordered_map<int, std::size_t> perStructAlloc;
    static inline std::unordered_map<int, std::size_t> perStructCount;
    static inline std::unordered_map<int, string> structNames;
    static inline std::size_t totalAllocated = 0;
    static inline std::mutex allocMutex;

	static void logMessage(std::ostringstream& oss,const char* msg){
		// Zeitstempel erstellen
		auto now = std::chrono::system_clock::now();
		std::time_t now_time = std::chrono::system_clock::to_time_t(now);
		oss << "[" << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S") << "] ";
		oss << msg <<"\n";
	}

public:
    static void registerAlloc(void* ptr, std::size_t size, int structID) {
        std::lock_guard<std::mutex> lock(allocMutex);
        allocations[ptr] = {size, structID};
        totalAllocated += size;
        perStructAlloc[structID] += size;
        perStructCount[structID]++;
    }

    static void unregisterAlloc(void* ptr) {
        std::lock_guard<std::mutex> lock(allocMutex);
        auto it = allocations.find(ptr);
        if (it != allocations.end()) {
            std::size_t size = it->second.size;
            int structID = it->second.structID;
            totalAllocated -= size;
            perStructAlloc[structID] -= size;
            perStructCount[structID]--;
            allocations.erase(it);
        }
    }

    static void registerStructName(int id, const string& name) {
        std::lock_guard<std::mutex> lock(allocMutex);
        structNames[id] = name;
    }

    static void report() {
        std::lock_guard<std::mutex> lock(allocMutex);
        std::cout << "\n[MemoryManager] Total Allocated: " << totalAllocated << " Bytes\n";

        for (const auto& [id, size] : perStructAlloc) {
            string name = structNames.count(id) ? structNames[id] : "Unknown";
            std::cout << "  -> StructID " << id << " (" << name << "): "
                      << size << " Bytes in " << perStructCount[id] << " allocations\n";
        }
        if (!allocations.empty()) {
			std::unordered_map<int, std::size_t> unfreedSize;
			std::unordered_map<int, std::size_t> unfreedCount;

			for (const auto& [ptr, info] : allocations) {
				unfreedSize[info.structID] += info.size;
				unfreedCount[info.structID]++;
			}

			std::cout << "\n[MemoryManager] Unfreed Allocations Summary:\n";
			for (const auto& [id, size] : unfreedSize) {
				string name = structNames.count(id) ? structNames[id] : "Unknown";
				std::cout << "  * StructID " << id << " (" << name << "): "
						  << size << " Bytes in " << unfreedCount[id] << " allocations\n";
			}
		} else {
			std::cout << "\n[MemoryManager] All memory was properly freed.\n";
		}
    }
	
	static void logReport(const char* path) {
		std::fstream file;
        file.open(path, std::ios::out | std::ios::app);
        if (file.is_open()) {
            std::ostringstream oss;
			// Fehlerbehandlung bewusst minimal gehalten
			std::lock_guard<std::mutex> lock(allocMutex);
			std::string placeholder="[MemoryManager] Total Allocated: ";
			placeholder+=std::to_string(totalAllocated);
			placeholder+=" Bytes";
			logMessage(oss,placeholder.c_str());
			std::string name;
			for (const auto& [id, size] : perStructAlloc) {
				name = structNames.count(id) ? structNames[id] : "Unknown";
				placeholder= " -> StructID ";
				placeholder+=std::to_string(id);
				placeholder+=" (";
				placeholder+=name;
				placeholder+="): ";
				placeholder+=std::to_string(size);
				placeholder+=" Bytes in ";
				placeholder+=std::to_string(perStructCount[id]);
				placeholder+=" allocations";
				logMessage(oss,placeholder.c_str());
			}
			if (!allocations.empty()) {
				std::unordered_map<int, std::size_t> unfreedSize;
				std::unordered_map<int, std::size_t> unfreedCount;

				for (const auto& [ptr, info] : allocations) {
					unfreedSize[info.structID] += info.size;
					unfreedCount[info.structID]++;
				}

				logMessage(oss,"[MemoryManager] Unfreed Allocations Summary:");
				for (const auto& [id, size] : unfreedSize) {
					name = structNames.count(id) ? structNames[id] : "Unknown";
					placeholder= "  * StructID ";
					placeholder+=std::to_string(id);
					placeholder+=" (";
					placeholder+=name;
					placeholder+="): ";
					placeholder+=std::to_string(size);
					placeholder+=" Bytes in ";
					placeholder+=std::to_string(unfreedCount[id]);
					placeholder+=" allocations";
					logMessage(oss,placeholder.c_str());
				}
			} else {
				logMessage(oss,"[MemoryManager] All memory was properly freed.");
			}
            // In Datei schreiben
            file << oss.str();
            file.close();
        }
    }
};
