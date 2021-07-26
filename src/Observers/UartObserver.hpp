#include<iostream>
#include "../Contracts/IObserver.hpp"

using namespace std;

class UartObserver: public IObserver
{
private:
    
public:
    UartObserver();
    ~UartObserver();
    void subscribe(IHandler* h) override;
    void publish(string title, string payload) override;
    void unsubscribe(IHandler* h) override;
};

UartObserver::UartObserver(/* args */)
{
}

UartObserver::~UartObserver()
{
}

void UartObserver::subscribe(IHandler* h){
    long int addr = (long int)h;
    this->handler_list.push_back(addr);
}

void UartObserver::unsubscribe(IHandler* h){

}

void UartObserver::publish(string title, string payload){
    list <long int> :: iterator it;
    for(it = this->handler_list.begin(); it != this->handler_list.end(); ++it){
        long int adr = *it;
        IHandler* h = (IHandler *)adr;
        h->save_data(title, payload);
    }
}
