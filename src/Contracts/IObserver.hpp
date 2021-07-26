#pragma once

#include <iostream>
#include <list>
#include "../Contracts/IHandler.hpp"

using namespace std;

class IObserver
{
    protected:
        list<long int> handler_list;
    public:
        IObserver();
        virtual ~IObserver();
        virtual void subscribe(IHandler* h);
        virtual void publish(string title, string payload);
        virtual void unsubscribe(IHandler* h);
};

IObserver::IObserver()
{
}

IObserver::~IObserver()
{
}

void IObserver::subscribe(IHandler* h){

}

void IObserver::publish(string title, string payload){

}

void IObserver::unsubscribe(IHandler* h){
    
}
