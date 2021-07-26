#include<iostream>
#include"../Contracts/ITransport.hpp"
#include"../Contracts/ICommand.hpp"
#include<unistd.h>

using namespace std;

class AtCommand: public ICommand
{
    public:
        AtCommand(ITransport* trans);
        ~AtCommand();
        void send(string command, int delay_time) override;
        void receive(char* buf, int len) override;
        bool send_with_response_checking(string command, string response_expect, int delay_time) override;
        bool is_checking_response() override;
};

AtCommand::AtCommand(ITransport* trans): ICommand(trans)
{
}

AtCommand::~AtCommand()
{
}

void AtCommand::send(string command, int delay_time)
{
    this->trans->send(command);
    usleep(delay_time);
}

void AtCommand::receive(char* buf, int len){
    this->trans->receive(buf, len);
}

bool AtCommand::is_checking_response(){
    return this->trans->get_response_checking_status();
}

bool AtCommand::send_with_response_checking(string command, string response_expect, int delay_time)
{
    this->trans->send(command);
    this->trans->set_response_checking_data(response_expect);
    this->trans->set_response_checking_status(true);
    usleep(delay_time);
    int timeout = 0;
    while (this->trans->get_response_checking_status())
    {
        usleep(100000);
        timeout = timeout + 1;
        if(timeout == 30){
            return false;
        }
    }
    return true;
}

