#include <iostream>
#include <map>
#include "../Contracts/IHandler.hpp"
#include "../Constants/Constant.hpp"

using namespace std;

class HttpDataHandler: public IHandler
{
    private:
        void _handler_process(string data);
        
    public:
        HttpDataHandler();
        ~HttpDataHandler();
        void save_data(string title, string payload) override;
        void handler() override;
};

HttpDataHandler::HttpDataHandler()
{
}

HttpDataHandler::~HttpDataHandler()
{
}

void HttpDataHandler::save_data(string title, string payload){
    if(title.compare(HTTP_DATA_TITLE) != 0){
        return;
    }
    this->data_storage.push(payload);
}

void HttpDataHandler::handler(){
    if(!this->data_storage.empty()){
        string data = this->data_storage.front();
        this->data_storage.pop();
        this->_handler_process(data);
    }
}

void HttpDataHandler::_handler_process(string data){
    cout << "Http: " << data << endl;
}
