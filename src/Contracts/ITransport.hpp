#pragma once

#include <iostream>
using namespace std;

class ITransport
{
    protected:
        bool response_checking_state;
        string response_checking_data;
    public:
        ITransport(string port_name);
        ITransport();
        virtual ~ITransport();
        virtual void send(string command);
        virtual void receive(char* buf, int len);
        virtual void set_up();
        virtual bool get_response_checking_status();
        virtual void set_response_checking_status(bool response_checking_status);
        virtual void set_response_checking_data(string response_expect_data);
        virtual string get_response_checking_data();
};

ITransport::ITransport()
{

}

ITransport::ITransport(string port_name)
{
}

ITransport::~ITransport()
{
}

void ITransport::send(string command)
{
}

void ITransport::set_up()
{
}

void ITransport::receive(char* buf, int len)
{
}

bool ITransport::get_response_checking_status()
{
    return this->response_checking_state;
}

void ITransport::set_response_checking_status(bool response_checking_status)
{
}

void ITransport::set_response_checking_data(string response_expect_data)
{
}

string ITransport::get_response_checking_data()
{
    return this->response_checking_data;
}
