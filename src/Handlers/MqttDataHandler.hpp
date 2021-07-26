#include <iostream>
#include <map>
#include "../Contracts/IHandler.hpp"
#include "../Constants/Constant.hpp"

using namespace std;

class MqttDataHandler: public IHandler
{
    private:
        void _handler_process(string data);

    public:
        MqttDataHandler();
        ~MqttDataHandler();
        void save_data(string title, string payload) override;
        void handler() override;
};

MqttDataHandler::MqttDataHandler()
{
}

MqttDataHandler::~MqttDataHandler()
{
}

void MqttDataHandler::save_data(string title, string payload){
    if(title.compare(MQTT_DATA_TITLE) != 0){
        return;
    }
    this->data_storage.push(payload);
}

void MqttDataHandler::handler(){
    if(!this->data_storage.empty()){
        string data = this->data_storage.front();
        this->data_storage.pop();
        this->_handler_process(data);
    }
}

void MqttDataHandler::_handler_process(string data){
    cout << "Mqtt: " << data << endl;
}
