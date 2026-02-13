class PMath{
	
	static constexpr int NUM_ITERATIONS = 31;
    static constexpr double K = 0.607252935; // Vorfaktor für Cordic
	
	/*
    // Näherung für arctan(2^-i) Werte
	static double arctan(double x) {
		x = (1.0 / (1 << ((int)x)));
		double result = 0.0; 
		double term = x; 
		for (int n = 0; n < NUM_ITERATIONS; ++n) { 
			result += term; 
			term *= -x * x * (2 * n + 1) / (2 * n + 3); 
		} 
		return result; 
	}
	*/

	
	static double transformAngle(int degrees) {
		int transformedDegrees = degrees % 360; // Reduziere den Winkel auf einen Zyklus von 360°
		if (transformedDegrees > 180) {
			transformedDegrees -= 360; // Transformiere Winkel in den Bereich [-180°, 180°]
		}
		if (transformedDegrees > 90) {
			transformedDegrees = 180 - transformedDegrees; // Transformiere Winkel in den Bereich [-90°, 90°]
		} else if (transformedDegrees < -90) {
			transformedDegrees = -180 - transformedDegrees; // Transformiere Winkel in den Bereich [-90°, 90°]
		}
		return transformedDegrees;
	}
	
	public:
	
    static constexpr double PI = 3.14159265358979323846;
	
	static double round(double value){
		int int_value = static_cast<int>(value);
		if(value-int_value>=0.5){
			return int_value+1;
		}
		return int_value;
	}
	
	static double roundUp(double value){
		int int_value = static_cast<int>(value);
		if(value-int_value>0){
			return int_value+1;
		}
		return int_value;
	}
	
	static double pow(double base, int exp){
		if(exp==0){
			return 1;
		}
		double result=1.0;
		while(exp>0){
			if(exp % 2 == 1){
				result *= base;
			}
			base*=base;
			exp/=2;
		}
		return result;
	}
	
	static double max(double firstvalue,double secondvalue){
		if(firstvalue<secondvalue){
			return secondvalue;
		}
		return firstvalue;
	}

	static double min(double firstvalue,double secondvalue){
		if(firstvalue>secondvalue){
			return secondvalue;
		}
		return firstvalue;
	}

	static double abs(double value){
		if(value<0){
			return (-value);
		}
		return value;
	}
	
	static double toDegrees(double radians) { 
		return radians * (180.0 / PI); 
	}
	
	static double toRadians(double degrees){ 
		return degrees * (PI / 180.0); 
	}

	static double arctan(int i) {
		switch(i) {
			case 0: return 0.7853981633974483; // arctan(1) = pi/4
			case 1: return 0.4636476090008061; // arctan(1/2)
			case 2: return 0.24497866312686414; // arctan(1/4)
			case 3: return 0.12435499454676144; // arctan(1/8)
			case 4: return 0.06241880999595735; // arctan(1/16)
			case 5: return 0.031239833430268277; // arctan(1/32)
			case 6: return 0.015623728620476831; // arctan(1/64)
			case 7: return 0.007812341060101111; // arctan(1/128)
			case 8: return 0.0039062301319669718; // arctan(1/256)
			case 9: return 0.0019531225164788188; // arctan(1/512)
			case 10: return 0.0009765621895593195; // arctan(1/1024)
			case 11: return 0.0004882812111948983; // arctan(1/2048)
			case 12: return 0.00024414062014936177; // arctan(1/4096)
			case 13: return 0.00012207031189367021; // arctan(1/8192)
			case 14: return 0.00006103515617420877; // arctan(1/16384)
			case 15: return 0.000030517578115526097; // arctan(1/32768)
			case 16: return 0.000015258789061315762; // arctan(1/65536)
			case 17: return 0.00000762939453110197; // arctan(1/131072)
			case 18: return 0.000003814697265606496; // arctan(1/262144)
			case 19: return 0.000001907348632810187; // arctan(1/524288)
			case 20: return 0.0000009536743164059606; // arctan(1/1048576)
			case 21: return 0.000000476837158203088; // arctan(1/2097152)
			case 22: return 0.0000002384185791015582; // arctan(1/4194304)
			case 23: return 0.00000011920928955078033; // arctan(1/8388608)
			case 24: return 0.000000059604644775390165; // arctan(1/16777216)
			case 25: return 0.000000029802322387695083; // arctan(1/33554432)
			case 26: return 0.000000014901161193847542; // arctan(1/67108864)
			case 27: return 0.000000007450580596923771; // arctan(1/134217728)
			case 28: return 0.0000000037252902984618855; // arctan(1/268435456)
			case 29: return 0.0000000018626451492309427; // arctan(1/536870912)
			case 30: return 0.0000000009313225746154713; // arctan(1/1073741824)
			default: 
				throw "Arctan Funktionserror: Parameter muss zwischen 0 und 30 liegen!";
		}
	}

    static double sin(int degrees) {
        double x = K;
        double y = 0.0;
        double z = toRadians(transformAngle(degrees));

        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            double x_new, y_new;
            double angle = arctan(i);
            if (z >= 0) {
                x_new = x - (y / (1 << i)); // bitweise Verschiebung ersetzt die Division
                y_new = y + (x / (1 << i));
                z -= angle;
            } else {
                x_new = x + (y / (1 << i));
                y_new = y - (x / (1 << i));
                z += angle;
            }
            x = x_new;
            y = y_new;
        }
        return y;
    }

    static double cos(int degrees) {
        double x = K;
        double y = 0.0;
        double z = toRadians(transformAngle(degrees));

        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            double x_new, y_new;
            double angle = arctan(i);
            if (z >= 0) {
                x_new = x - (y / (1 << i)); // bitweise Verschiebung ersetzt die Division
                y_new = y + (x / (1 << i));
                z -= angle;
            } else {
                x_new = x + (y / (1 << i));
                y_new = y - (x / (1 << i));
                z += angle;
            }
            x = x_new;
            y = y_new;
        }
		if(abs(degrees)>90 && abs(degrees)<270){
			return -x;
		}
        return x;
    }
	
};