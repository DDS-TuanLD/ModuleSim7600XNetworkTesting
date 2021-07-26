#pragma once

#include <iostream>
#include <queue>

using namespace std;

class IHandler
{
    protected:
        queue<string> data_storage;
    public:
        IHandler();
        virtual ~IHandler();
        virtual void handler();
        virtual void save_data(string title, string payload);
};

IHandler::IHandler()
{
}

IHandler::~IHandler()
{
}

void IHandler::handler(){
    return;
}

void IHandler::save_data(string title, string payload){
    
}
