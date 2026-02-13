#include <windows.h>
#include <vector>
#include <unordered_set>
#include <iostream>
#include "Libary.cpp"

PWindow window;
int width = 1200, height = 900;
bool draw = false;
bool gui_mode = false;

int main() {
    PFile filereader;
    if (filereader.load("window.txt")) {
        width = stringToInt(filereader.readLine());
        height = stringToInt(filereader.readLine());
        draw = stringToInt(filereader.readLine());
        gui_mode = stringToInt(filereader.readLine());
        filereader.close();
    }
	else{
		std::cout << "Fehler: Keine 'window.txt' Datei gefunden!\n";
		return 0;
	}
    std::cout << "#####--Windowgrosse: " << width << ", " << height << "--#####\n";
    std::cout << "#####--Visualliserung: ";
    if (draw) {
        std::cout << "Eingeschaltet";
    } else {
        std::cout << "Ausgeschaltet";
    }
    std::cout << "!--#####\n";
    std::cout << "#####--GUI-Mode: ";
    if (gui_mode) {
        std::cout << "Eingeschaltet";
    } else {
        std::cout << "Ausgeschaltet";
    }
    std::cout << "!--#####\n";
    if (!SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS)) {
        std::cerr << "SetPriorityClass war nicht erfolgreich!\n";
    }

    if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST)) {
        std::cerr << "SetThreadPriority war nicht erfolgreich!\n";
    }
    SetProcessAffinityMask(GetCurrentProcess(), 1);
    try {
        window.initDrawframe(width, height);
        window.setDrawing(draw);
        window.setGUI(gui_mode);
        //####################################//
        window.create(width, height);
        //####################################//
    }
    catch (std::string& exception) {
        std::cout << exception << "\n";
    }
    catch (const char* exception) {
        std::cout << exception << "\n";
    }
    catch (char* exception) {
        std::cout << exception << "\n";
    }
    return 0;
}
