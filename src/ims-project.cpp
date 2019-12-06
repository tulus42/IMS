#include <simlib.h>
#include <getopt.h>
#include <iostream>
#include "ims-project.hpp"

using namespace std;

Facility StartYear("StartYear");
Facility EndYear("EndYear");


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




class YearTimer: public Process {

public:
    void Behavior() {
        cout << "Rok\n";
        Release(EndYear);
        Activate(Time+1);
        Seize(EndYear);
    }



};







int main(int argc, char** argv){

    unsigned long petrol_cars = DEFAULT_PETROL_CARS_NUM;
    unsigned long electric_car = DEFAULT_ELECTRIC_CARS_NUM;
    double length_od_sim = DEFAULT_LENGTH;

    double petrol_after_petrol = DEFAULT_AFTER_PETROL_PERCENTAGE;
    double electric_after_petrol;
    double petrol_after_electric = DEFAULT_AFTER_ELECTRIC_PERCENTAGE;
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


    return 0;
}