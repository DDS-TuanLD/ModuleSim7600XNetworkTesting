#include <iostream>
#include "../Contracts/ICommand.hpp"
#include "../Contracts/IBuffer.hpp"
#include "../Contracts/IObserver.hpp"
#include "../Constants/Constant.hpp"
#include <string.h>
#include <unistd.h>
using namespace std;

class UartDataHandler
{
    private:
        string last_command;
        IObserver* ob;
        ITransport* trans;
        IBuffer* buf;

    private:
        string _get_message_valid_from_buf();
        void _get_http_body_data(string cmd);
        bool _mqtt_filter(string cmd);
        bool _http_filter(string cmd);

    public:
        UartDataHandler(ITransport* trans, IBuffer*buf, IObserver* ob);
        ~UartDataHandler();

        void lock();

        void unlock();

        bool is_locked();

        void set_last_command(string command);

        string get_last_command();

        void receive_uart_data();

        void handler_uart_data();
};

UartDataHandler::UartDataHandler(ITransport*trans, IBuffer*buf, IObserver* ob)
{
    this->trans = trans;
    this->buf = buf;
    this->ob = ob;
}

UartDataHandler::~UartDataHandler()
{
}

string UartDataHandler::get_last_command(){
    return this->last_command;
}

void UartDataHandler::set_last_command(string command){
    this->last_command = command;
}

void UartDataHandler::receive_uart_data()
{
    int i;
    char temp[100] = {0};
    this->trans->receive(temp, sizeof(temp));
    int count = strlen(temp);
    if(count == 0){
    	return;
    }
    for (i = 0; i< count; i++){
        while(this->buf->is_full());
        this->buf->push(temp[i]);
    }
}

void UartDataHandler::handler_uart_data()
{

    bool is_handler_by_filter;
    
    string cmd = this->_get_message_valid_from_buf();

    if (cmd == ""){
        return;
    }

    cout << "handler_uart_cmd: " << cmd << endl;

    if (cmd.find(this->trans->get_response_checking_data()) != string::npos){
        this->trans->set_response_checking_status(false);
    }

    is_handler_by_filter = this->_mqtt_filter(cmd);
    if(is_handler_by_filter) return;
   
    is_handler_by_filter = this->_http_filter(cmd);
    if(is_handler_by_filter) return;
}

string UartDataHandler::_get_message_valid_from_buf(){
    string cmd;
    int r = this->buf->get_read_point();
    int count = 0;

    while(true){

        if(this->buf->get_bytes_written() == 0){
            return cmd;
        }
    
        int point_1 = count + r;
        int point_2 = count + r +1;
        
        if(point_1 >= this->buf->get_size()){
            point_1 = point_1-this->buf->get_size();
        }
 
        if(point_2 >= this->buf->get_size()){
            point_2 = point_2-this->buf->get_size();
        }

        if(this->buf->get(point_1) == '>'){
            cmd.push_back('>');
            this->buf->pop();
            break;
        }

        if((this->buf->get(point_1) == '\r')&&(this->buf->get(point_2) == '\n')){
            int i;
            for(i = 0; i<= count+1; i++){
                cmd.push_back(this->buf->pop());
            }
            break;
        }

        count = count + 1;
        if(count > this->buf->get_bytes_written()){
            return cmd;
        }
    }
    return cmd;
}

void UartDataHandler::_get_http_body_data(string cmd){
    string http_body_length;
    int last_comma_index = cmd.find_last_of(',');
    int i;
    int count = cmd.length();
    for(i = last_comma_index+1; i< count; i++){
        if(cmd[i] == '\r'){
            break;
        }
        http_body_length.push_back(cmd[i]);
    } 

    string get_http_body_cmd = "AT+HTTPREAD=";
    get_http_body_cmd.append(http_body_length);
    get_http_body_cmd.append(",");
    get_http_body_cmd.append(http_body_length);
    get_http_body_cmd.append("\r\n");
    this->trans->send(get_http_body_cmd);
}

bool UartDataHandler::_mqtt_filter(string cmd){
    static bool mqtt_topic_incomming = false;
    static bool mqtt_msg_incomming = false;
    static string mqtt_msg;
    static string mqtt_topic;

    if (cmd.find("+CMQTTRXSTART") != string::npos){
        return true;
    }

    if(cmd.find("+CMQTTRXEND") != string::npos){
        return true;
    }

    if (cmd.find("+CMQTTRXTOPIC") != string::npos){
        mqtt_topic_incomming = true;
        return true;
    }

    if (mqtt_topic_incomming == true){
        cmd.pop_back();
        cmd.pop_back();
        mqtt_topic = cmd;
        mqtt_topic_incomming = false;
        return true;

    }

    if (cmd.find("+CMQTTRXPAYLOAD") != string::npos){
        mqtt_msg_incomming = true;
        return true;
    }

    if (mqtt_msg_incomming == true){
        cmd.pop_back();
        cmd.pop_back();
        mqtt_msg = cmd;
        mqtt_msg_incomming = false;

        if(mqtt_topic.compare("") == 0){
            mqtt_msg.clear();
            return true;
        }

        string payload = "{\"topic\":\"";
        payload.append(mqtt_topic);
        payload.append("\",\"msg\":");
        payload.append(mqtt_msg);
        payload.append("}");

        mqtt_msg.clear();
        mqtt_topic.clear();

        this->ob->publish(MQTT_DATA_TITLE, payload);
        return true; 
    }
    return false;
}

bool UartDataHandler::_http_filter(string cmd){
    static bool http_body_data_incomming = false;
    static string http_body_data;
    
    if(cmd.find("AT+HTTPREAD") != string::npos){
        return true;
    }

    if (cmd.find("+HTTPREAD: DATA") != string::npos){
        http_body_data_incomming = true;
        return true;
    }

    if( http_body_data_incomming == true){
        cmd.pop_back();
        cmd.pop_back();
        http_body_data.append(cmd);
        http_body_data_incomming = false;
        return true;
    }

    if(cmd.find("+HTTPREAD: 0") != string::npos){

        this->ob->publish(HTTP_DATA_TITLE, http_body_data);

        http_body_data.clear();
        return true;
    }

    if(cmd.find("+HTTPACTION: 0,200") != string::npos){
        this->_get_http_body_data(cmd);
        return true;
    }

    if(cmd.find("+HTTPACTION: 1,200") != string::npos){
        this->_get_http_body_data(cmd);
        return true;
    }

    return false;
}
