class Timer {
public:
    Timer() {
        reset();
    }

    void start() {
        QueryPerformanceCounter(&start_time);
        paused = false;
        pause_time.QuadPart = 0;
    }

    void stop() {
        if (paused) {
            resume();
        }
        QueryPerformanceCounter(&end_time);
    }
	
	 // Pausiert den Timer (einmalig bis zum nächsten resume)
    void pause() {
        if (!paused) {
            QueryPerformanceCounter(&pause_time);
            paused = true;
        }
    }
	
	// Setzt den Timer dort fort, wo pause() aufgehört hat
    void resume() {
        if (paused) {
            LARGE_INTEGER resume_time;
            QueryPerformanceCounter(&resume_time);
            // Schiebe den Startzeitpunkt nach vorne um die Pausenlänge
            start_time.QuadPart += (resume_time.QuadPart - pause_time.QuadPart);
            paused = false;
        }
    }

    void reset() {
		QueryPerformanceFrequency(&frequency);
        start_time.QuadPart = 0;
        end_time.QuadPart   = 0;
        pause_time.QuadPart = 0;
        paused = false;
    }

    // Dauer in Nanosekunden (pausierte Intervalle werden abgezogen)
    long long duration_ns() const {
        LONGLONG delta;
        if (paused) {
            // Wenn aktuell pausiert, bis zum Pausenbeginn messen
            delta = pause_time.QuadPart - start_time.QuadPart;
        } else {
            delta = end_time.QuadPart - start_time.QuadPart;
        }
        return static_cast<long long>(delta) * 1'000'000'000LL / frequency.QuadPart;
    }

    // Dauer in Millisekunden (pausierte Intervalle werden abgezogen)
    double duration_ms() const {
        LONGLONG delta;
        if (paused) {
            delta = pause_time.QuadPart - start_time.QuadPart;
        } else {
            delta = end_time.QuadPart - start_time.QuadPart;
        }
        return static_cast<double>(delta) * 1000.0 / static_cast<double>(frequency.QuadPart);
    }

private:
    LARGE_INTEGER frequency;
    LARGE_INTEGER start_time;
    LARGE_INTEGER end_time;
    LARGE_INTEGER pause_time;
    bool          paused = false;
};
