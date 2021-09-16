#include <iostream>
#include <iomanip>

using namespace std;

class DayOfYear{
private:
    short unsigned int dayOfYear, year;
    static const short unsigned int days[];
public:
    DayOfYear(short unsigned int day = 1, short unsigned int year = 365) : dayOfYear(day), year(year)
    {}
    DayOfYear(short unsigned int day, short unsigned int month, short unsigned int year){
        dayOfYear = days[month] + day;
        this->year = year;
    }

    short unsigned int get_dayOfYear(){
        return dayOfYear;
    }

    DayOfYear operator++(){
        DayOfYear n;
        if(dayOfYear==365){
            n.dayOfYear=1;
            n.year = ++year;
        }else{
            n.dayOfYear = ++dayOfYear;
            n.year = year;
        }
        return n;
    }

    friend istream& operator>>(istream& in, DayOfYear& D){
        char c;
        short unsigned int month, day;
        in >> D.year >> c >> month >> c >> day;
        D.dayOfYear = days[month-1] + day;
    }

    friend ostream& operator<<(ostream& out, const DayOfYear& D){
        short unsigned int month, day;
        for(unsigned int i=0; i<12; ++i){
            if(D.dayOfYear > days[i]){
                month = i+1;
                day = D.dayOfYear - days[i];
            }
        }
        out << D.year << "-" << setw(2) << setfill('0') << month << "-" << setw(2) << setfill('0') << day;
    }

};

const short unsigned int DayOfYear::days[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

class Vehicle{
private:
    const int no;
    string model;
    float price24h;
public:
    Vehicle(int no, string model, float price24h): no(no), model(model), price24h(price24h)
    {}
    virtual ~Vehicle(){
        delete this;
    }
    int get_no(){
        return no;
    }
    string get_model(){
        return model;
    }
    float get_price(int daysOfRental){
        return price24h * daysOfRental;
    }
    virtual void print() = 0;
};

class Bike : public Vehicle{
public:
    Bike(int no, string model): Vehicle(no, model, 9.99)
    {}

    virtual void print(){
        cout << get_no() << ": " << get_model() << " (Bike)" << endl;
    }
};

class EMotorVehicle : public Vehicle{
public:
    EMotorVehicle(int no, string model, float price24h): Vehicle(no, model, price24h)
    {}
    virtual bool is_streetLegal() = 0;
    virtual void print(){
        if(is_streetLegal() == false){
            cout << get_no() << ": " << get_model()  << " (not street legal)";
        } else cout << get_no() << ": " << get_model();
    }
};

class EBike : public EMotorVehicle{
public:
    EBike(int no, string model, float price24h = 29.99): EMotorVehicle(no, model, price24h)
    {}
    virtual bool is_streetLegal(){
        return true;
    }
    virtual void print(){
        EMotorVehicle::print();
        cout<< " (EBike)"<<endl;
    }
};

class EScooter : public EMotorVehicle{
private:
    bool streetLegal;
public:
    EScooter(int no, string model, float price24h = 19.99, bool streetLegal = false): EMotorVehicle(no, model, price24h), streetLegal(streetLegal)
    {}
    virtual bool is_streetLegal(){
        return streetLegal;
    }
    virtual void print(){
        EMotorVehicle::print();
        cout<< " (EScooter)"<<endl;
    }
};

class Rental{
private:
    const int no;
    DayOfYear from;
    int days;
    string customer;
    static int last_no;
public:
    Rental(string customer, DayOfYear from, int days=1): customer(customer), from(from), days(days), no(++last_no)
    {}
    int get_days(){
        return days;
    }
    DayOfYear get_from(){
        return from;
    }
    DayOfYear get_until(){
        DayOfYear until(from);
        for(unsigned int i=0; i<get_days(); ++i){
             ++until;
        }
        return until;
    }
    void print(){
        cout << get_from() << " to " << get_until() << ", rental no " << this->no << " for " << this->customer;
    }

};

int Rental::last_no = 0;

class Schedule{
private:
    Vehicle* vehicle;
    Rental* year[365];
    Schedule* next;
public:
    Schedule(Vehicle* vehicle): vehicle(vehicle)
    {
        next = nullptr;
        for(unsigned int i=0; i<365; ++i){
            year[i] = nullptr;
        }
    }

    Vehicle* get_vehicle(){
        return vehicle;
    }

    Schedule* get_next(){
        return next;
    }

    void set_next(Schedule* next){
        this->next = next;
    }

    bool isFree(DayOfYear from, int days){
        if( year[from.get_dayOfYear()] != nullptr && year[from.get_dayOfYear() + days] != nullptr )
            return false;
        else return true;
    }

    float book(string customer, DayOfYear from, int days){
        year[from.get_dayOfYear()] = new Rental(customer, from, days);
        for(unsigned int i=1; i<days; ++i){
            year[from.get_dayOfYear() + i]= year[from.get_dayOfYear()];
        }
        return vehicle->get_price(days);
    }

    void print(DayOfYear D){
        get_vehicle()->print();
        if(year[D.get_dayOfYear()]!=nullptr){
            year[D.get_dayOfYear()]->print();
        }
    }

    void print(){ //WORKING
        cout << "SCHEDULE FOR " ;
        get_vehicle()->print();
        for(int i=0; i<365; ++i){ //CHECK IF THIS WORKS!! /////NOT WORKINGG
            if(year[i]!=nullptr){
                year[i]->print();
                int r = year[i]->get_days();
                cout << " , total: " << get_vehicle()->get_price(r) << " EUR" << endl;
                i = i + r;
            }
        }
    }
};

class Agency{
private:
    float profitPercent;
    float profit;
    Schedule* head;
    Schedule* last;
protected:
    Schedule* searchFor(int no){
        Schedule* p(head);
        while(p != nullptr){
            if(p->get_vehicle()->get_no() == no){
                return p;
            }
            p=p->get_next();
        }
        return nullptr;
    }
public:
    Agency(){
        profitPercent = 0.20;
        profit = 0.00;
        head = nullptr;
        last = nullptr;
    }
    float get_profit(){
        return profit;
    }
    void set_profitPercent(float profitPercent){
        this->profitPercent = profitPercent/100;
    }
    void add(Vehicle* vehicle){ //working
        Schedule* temp = new Schedule(vehicle);
        if(head == nullptr){
            head = temp;
            last = temp;
        }else{
            last->set_next(temp);
            last = temp;
        }
    }
    bool isFree(int no, DayOfYear from, int days){
        if(this->searchFor(no)->isFree(from, days) == true){
            return true;
        }else return false;
    }

    bool book(int no, string customer, DayOfYear from, int days){
        profit += profitPercent * this->searchFor(no)->book(customer, from, days);
        cout << "booked, price for this rental: " << this->searchFor(no)->get_vehicle()->get_price(days) << " EUR" <<endl;
        return true;
    }

    int chooseVehicle(){
        Schedule* temp(head);
        if(head == nullptr){
            cout << "This list is empty!" <<endl;
        }else{
            unsigned int i=0;
            while(temp != nullptr){
                temp->get_vehicle()->print();
                temp = temp->get_next();
            }
            unsigned int ans;
            cout << "choose vehicle no: ";
            cin >> ans;
            return ans;
        }
    }

    void print(DayOfYear D){
        Schedule* temp(head);
        if(head == nullptr){
            cout << "This list is empty!" <<endl;
        }else{
            while(temp != nullptr){
                temp->print(D);
                temp = temp->get_next();
            }
        }
    }

    void print(){ //working
        Schedule* temp(head);
        if(head == nullptr){
            cout << "This list is empty!" <<endl;
        }else{
            while(temp != nullptr){
                temp->print();
                temp = temp->get_next();
            }
        }
        cout<< "Profit = " << fixed << setprecision(2) << get_profit() << " EUR" << endl;
    }
};

int main()
{
    Agency agency; // used for creating the agency
    char answer; // used for selection
    DayOfYear today(01, 01, 2021); // used for default date
    DayOfYear rentalDay; // used for the renting day
    string renter; // used for the name of customer
    int daysOfRenting; // used for the number of days to rent
    float priceOfBooking; // used for the cost of booking
    int chosenVehicle; // used for the selected vehicle to rent
    unsigned int numerationOfBikes=0; // used for the bike number,id
    unsigned int numerationOfEBikes=0; // used for the E-bike number, id
    unsigned int numerationOfEScooters=0; // used for the E-scooter number id
    unsigned int numerationOfVehicles=0; // used for the number of vehicles created


    cout << "SIMPLIFIED SIMULATION OF A RENTAL AGENCY" << endl
         << "========================================" << endl << endl;

    do{
        cout << "----------------------------------------" << endl
             << "MENUE"                                    << endl
             << "A end of simulation"                      << endl
             << "B set new simulation date"                << endl
             << "C new rental manually"                    << endl
             << "D print rental of today " << today        << endl;
        DayOfYear temp(today);
        const DayOfYear tom(++temp);
        cout << "E print rental of tomorrow " << tom       << endl
             << "F print all rentals"                      << endl
             << "G print agency profit"                    << endl
             << "H set agency profit percent"              << endl
             << "I add bikes"                              << endl
             << "J add E-bikes"                            << endl
             << "K add E-scooters"                         << endl
             << "L new rental simulation"                  << endl
             << "your choice: ";
        cin >> answer;
        switch(answer){
        case 'A':
            break;
        case 'B':
            cout << "input date of today: ";
            cin >> temp;
            today=temp;
            break;
        case 'C': // if list is empty
            chosenVehicle = agency.chooseVehicle();
            cout << "day of rental: ";
            cin >> rentalDay;
            cout << "number of days: ";
            cin >> daysOfRenting;
            if(agency.isFree(chosenVehicle, rentalDay, daysOfRenting) == true){
                cout << "input data vehicle renter name: ";
                cin >> renter;
            }else{
                cerr << "sorry vehicle already booked" << endl;
                break;
            }
            if(agency.book(chosenVehicle, renter, rentalDay, daysOfRenting) == true){
                break;
            }else{
                cerr << "error occurred while booking"<<endl;
                break;
            }
        case 'D':
            cout << "SCHEDULE FOR " << today << endl;
            agency.print(today);
            break;
        case 'E':
            cout << "SCHEDULE FOR " << tom << endl;
            agency.print(tom);
            break;
        case 'F':
            agency.print();
            break;
        case 'G':
            cout << "agency profit: " << fixed << setprecision(2) << agency.get_profit() << " EUR" <<endl;
            break;
        case 'H':
            cout << "agency profit percent: ";
            float a;
            cin >> a;
            agency.set_profitPercent(a);
            break;
        case 'I':
            cout << "how many Bikes in this simulation? ";
            int n;
            cin >> n;
            for(unsigned int i=0; i<n; ++i){
                agency.add(new Bike(++numerationOfVehicles, "City"+to_string(++numerationOfBikes)));
            }
            break;
        case 'J':
            cout << "how many EBikes in this simulation? ";
            cin >> n;
            for(unsigned int i=0; i<n; ++i){
                agency.add(new EBike(++numerationOfVehicles, "Trekky"+to_string(++numerationOfEBikes)));
            }
            break;
        case 'K':
            cout << "how many EScooters in this simulation? ";
            cin >> n;
            for(unsigned int i=0; i<n; ++i){
                agency.add(new EScooter(++numerationOfVehicles, "Scooty"+to_string(++numerationOfEScooters)));
            }
            break;
        case 'L':
            break;
        }
    } while(answer != 'A');

    return 0;
}
