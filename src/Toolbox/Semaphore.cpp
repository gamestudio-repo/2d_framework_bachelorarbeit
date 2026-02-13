#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#pragma once
#include <windows.h>
#include <stdexcept>

class Semaphore {
private:
    HANDLE semHandle;  // Windows-Kernel-Semaphor-Handle

public:
    // Konstruktor: Erstellt eine performante Semaphore
    Semaphore(uint16_t init_count) {
        create(init_count);
    }

    // Erstellt oder ersetzt eine Semaphore mit gegebenem Anfangswert
    void create(uint16_t init_count) {
        if (semHandle!=nullptr){ 
			CloseHandle(semHandle); // Vorheriges Handle freigeben
		}
        semHandle = CreateSemaphore(nullptr, init_count, LONG_MAX, nullptr);
        if (!semHandle) {
            throw std::runtime_error("Semaphore konnte nicht erstellt werden!");
        }
    }

    // Warteoperation (P): Blockiert effizient ohne CPU zu verschwenden
    void P() {
        WaitForSingleObject(semHandle, INFINITE);
    }

    // Freigabeoperation (V): Gibt eine Ressource frei
    void V() {
        ReleaseSemaphore(semHandle, 1, nullptr);
    }

    // Destruktor: Gibt Systemressourcen frei
    ~Semaphore() {
        if (semHandle) CloseHandle(semHandle);
    }

    // Kopieren verhindern
    Semaphore(const Semaphore&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

    // Optional: Move-Semantik
    Semaphore(Semaphore&& other) noexcept : semHandle(other.semHandle) {
        other.semHandle = nullptr;
    }

    Semaphore& operator=(Semaphore&& other) noexcept {
        if (this != &other) {
            if (semHandle) CloseHandle(semHandle);
            semHandle = other.semHandle;
            other.semHandle = nullptr;
        }
        return *this;
    }
};

#endif // SEMAPHORE_HPP
