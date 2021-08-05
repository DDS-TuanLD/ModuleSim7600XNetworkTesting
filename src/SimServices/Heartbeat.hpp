#include <iostream>
#include "../Contracts/ICommand.hpp"
#include <sstream>
#include "../Constants/Constant.hpp"

using namespace std;

class Heartbeat{
    private:
        ICommand* command;
        bool internet_connect_state;
        static int disconnect_count;
        bool simcard_insert_state;
    public:
        Heartbeat(ICommand* command){
            this->command = command;
            this->internet_connect_state = true;
            this->simcard_insert_state = true;
        }
        ~Heartbeat(){};
        void ping_to_internet();
        void report_status();
        bool check_sim_insert();
};

int Heartbeat:: disconnect_count = 0;

bool Heartbeat:: check_sim_insert(){
    bool is_inserted = this->command->send_with_response_checking(" AT+CICCID\r\n", "+ICCID: ", 0);
    return is_inserted;
}

void Heartbeat:: ping_to_internet(){
    int is_connect = this->command->send_with_response_checking("AT+CPING=\"www.google.com\",1,3,64,1000,10000, 255\r\n", "+CPING: 3,3,3,0", 3000000);
    if(is_connect){
        this->disconnect_count = 0;
    }
    if(!is_connect){
        this->disconnect_count = this->disconnect_count + 1;
    }
    if(this->disconnect_count > 3){
        this->disconnect_count = 3;
    }
    return;
}

void Heartbeat::report_status(){
    if((disconnect_count == 0) && (!this->internet_connect_state)){
        cout << "connect with internet" << endl;
        this->internet_connect_state = true;
    }

    if((disconnect_count == 3) && (this->internet_connect_state)){
        cout << "disconnect with internet" << endl;
        this->internet_connect_state = false;
    }
}
