#include <simlib.h>
#include <getopt.h>
#include <iostream>
#include "ims-project.hpp"

using namespace std;

Facility StartYear("StartYear");
Facility EndYear("EndYear");
int yearCnt = 2018;
unsigned long petrol_cars = DEFAULT_PETROL_CARS_NUM;
unsigned long electric_cars = DEFAULT_ELECTRIC_CARS_NUM;

double petrol_after_petrol = DEFAULT_AFTER_PETROL_PERCENTAGE;
double petrol_after_electric = DEFAULT_AFTER_ELECTRIC_PERCENTAGE;

unsigned long CO2 = 0;


void handle_err(int err_code) {
    switch (err_code) {
    case INVALID_ARGUMENT:
        cout << "Try:" << endl 
        << "-p (--petrol)   -> percentual value of new electric cars that will be bought by owners of petrol cars" << endl
        << "-e (--electric) -> percentual value of new electric cars that will be bought by actual owners of electric cars" << endl;
        break;

    case INVALID_VALUE:
        cout << "Value must be number in %" << endl; 
        break;
    
    case INVALID_RANGE:
        cout << "Percentage must be in range 0.0-100.0" << endl;
    }

    exit(err_code);
}

void print_sim_start(double pp, double ep, double pe, double ee) {
    cout << "-------------------------------------------------------------" << endl
    << "---                    SIMULATION                         ---" << endl
    << "-------------------------------------------------------------" << endl
    << "--- INPUTS            --- After petrol --- After electric ---" << endl
    << "--- Buy Petrol:               " << pp << " %            " << pe << " %" << endl
    << "--- Buy Electric:             " << ep << " %            " << ee << " %" << endl
    << "-------------------------------------------------------------" << endl;
}


bool too_old_car(double type) {
    if (Random() > type) {
        return true;
    } else {
        return false;
    }
}


bool choose_new_engine(double percentage) {
    if (Random() <= (percentage / 100)) {
        return true;    // choose new petrol
    } else {
        return false;   // choose new electric
    }
}


class PetrolCar: public Process {

public:
    void Behavior() {
        Priority = 1;

        newYearPetrol:
        
        

        for (unsigned long i = 0; i < petrol_cars; i++) {
            if (too_old_car(TOO_OLD_PETROL_CAR)) {
                petrol_cars--;

                for (int j = 0; j < 2; j++) {
                    if (choose_new_engine(petrol_after_petrol)) {
                        petrol_cars++;
                        CO2 += CO2_NEW_PETROL;
                    } else {
                        electric_cars++;
                        CO2 += CO2_NEW_ELECTRIC;
                    }
                }
            }
        }

        for (unsigned long i = 0; i < electric_cars; i++) {
            CO2 += CO2_PETROL_FUEL;
            CO2 += CO2_PETROL_DRIVE;
        }
        Passivate();
        
        goto newYearPetrol;
    }       

};


class ElectricCar: public Process {

public:
    void Behavior() {
        Priority = 2;

        newYearElectric:
        

        for (unsigned long i = 0; i < electric_cars; i++) {
            if (too_old_car(TOO_OLD_ELECTRIC_CAR)) {
                electric_cars--;

                for (int j = 0; j < 2; j++) {
                    if (choose_new_engine(petrol_after_electric)) {
                        petrol_cars++;
                        CO2 += CO2_NEW_PETROL;
                    } else {
                        electric_cars++;
                        CO2 += CO2_NEW_ELECTRIC;
                    }
                }
            } else {
                if (this->too_old_battery()) {
                    CO2 += CO2_NEW_BATTERY;
                }
            }
        }

        for (unsigned long i = 0; i < electric_cars; i++) {
            CO2 += CO2_ELECTRIC_ENERGY;
        }

        Passivate();
        
        goto newYearElectric;
    }


    bool too_old_battery() {
    if (Random() < TOO_OLD_BATTERY) {
        return true;
    } else {
        return false;
    }
}
        

};




class YearTimer: public Event {

public:
    unsigned long yearCO2;
    PetrolCar* p_car = new PetrolCar;
    ElectricCar* e_car = new ElectricCar;
    Stat* CO2Stat = new Stat("Year CO2");

    void Behavior() {
    
        p_car->Activate();
        e_car->Activate();

        
        cout << "-------------------------------------------------------------" << endl;
        cout << "---                  Year " << yearCnt++ << "                            ---" << endl;
        cout << "- Number of petrol cars:      " << petrol_cars << endl;
        cout << "- Number of electric cars:    " << electric_cars << endl;

        cout << "ročné emisie:   " << (CO2 - yearCO2) / 1000 << " t" << endl;
        cout << "celkové emisie: " << CO2 / 1000 << " t" << endl;
        

        if (CO2 -yearCO2 != 0)
            (*CO2Stat)((CO2 - yearCO2)/1000);
        yearCO2 = CO2;
        Activate(Time + NEXT_YEAR);
        
        
    }

    ~YearTimer() {
        CO2Stat->Output();
    }

private:
    const double NEXT_YEAR = 1.0;
};







int main(int argc, char** argv){
    
    double length_od_sim = DEFAULT_LENGTH;

    double electric_after_petrol;
    double electric_after_electric;


    int opt;
    char *err_str;
    static const char *sOpts = "p:e:";
    static const struct option lOpts[] = {
        {"petrol", required_argument, nullptr, 'p'},
        {"electric", required_argument, nullptr, 'e'},
        {nullptr, no_argument, nullptr, 0},
    };

	while ((opt = getopt_long(argc, argv, sOpts, lOpts, nullptr)) != -1) {
        switch (opt) {

        case 'p':
            petrol_after_petrol = strtod(optarg, &err_str);

            if (*err_str != '\0') {
                handle_err(INVALID_VALUE);
            } else if (petrol_after_petrol < 0.0 || petrol_after_petrol > 100.0) {
                handle_err(INVALID_RANGE);
            }
            break;

        case 'e':
            petrol_after_electric = strtod(optarg, &err_str);

            if (*err_str != '\0') {
                handle_err(INVALID_VALUE);
            } else if (petrol_after_electric < 0.0 || petrol_after_electric > 100.0) {
                handle_err(INVALID_RANGE);
            }
            break;
        
        case 0:
            break;
        default:
            handle_err(INVALID_ARGUMENT);
        }
    }

    if (optind < argc)
	{
        handle_err(INVALID_ARGUMENT);
	}

    electric_after_petrol = 100.0 - petrol_after_petrol;
    electric_after_electric = 100.0 - petrol_after_electric;



    print_sim_start(petrol_after_petrol, electric_after_petrol, petrol_after_electric, electric_after_electric);

    Init(SIM_START, length_od_sim);
    (new YearTimer)->Activate();  
    Run();

    SIMLIB_statistics.Output();

    return 0;
}