#pragma once

#include<iostream>
#include"../Contracts/ITransport.hpp"

using namespace std;

class ICommand
{
    protected:
        ITransport* trans;
    public:
        ICommand(ITransport* trans);
        virtual ~ICommand();
        virtual void send(string command, int delay_time);
        virtual void receive(char* buf, int len);
        virtual bool send_with_response_checking(string command, string response_expect, int delay_time);
        virtual bool is_checking_response();
};

ICommand::ICommand(ITransport*trans)
{
    this->trans = trans;
}

ICommand::~ICommand()
{
}

void ICommand::send(string command, int delay_time)
{
}

void ICommand::receive(char* buf, int len)
{
}

bool ICommand::send_with_response_checking(string command, string response_expect, int delay_time)
{
    return true;
}

bool ICommand::is_checking_response()
{
    return false;
}
