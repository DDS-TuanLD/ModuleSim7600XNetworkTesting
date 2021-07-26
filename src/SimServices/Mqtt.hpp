#include <iostream>
#include "../Contracts/ICommand.hpp"
#include <sstream>

using namespace std;

class MqttConfig{
    public:
        string host;
        string port;
        string clientId;
        string username;
        string password;
        string cleanSession;
        string keepALive;
    
    public:
        MqttConfig(string host, string port, string clientId, string username, 
            string password, string cleanSession, string keepALive){
                this->host = host;
                this->port = port;
                this->clientId = clientId;
                this->username = username;
                this->password = password;
                this->cleanSession = cleanSession;
                this->keepALive = keepALive;
            };
        ~MqttConfig(){};
};

class MqttContext{
    private:
        ICommand *command;

    public:
        string id;
        string scheme;
        string version;
        string name;

    private:
        void setPubTopic(string topic);
        void setPubPayload(string mess);
        bool sendDataToBroker(int qos);
        void setSubTopic(string topic, int qos);
        void sendSubCommnad();
        void unSetSubTopic(string topic);
        void sendUnSubCmd();

    public:
        MqttContext(ICommand* command, string id, string name, string scheme, string version){
            this->id = id;
            this->name = name;
            this->scheme = scheme;
            this->version = version;
            this->command = command;
        };
        ~MqttContext(){};
        void Connect(MqttConfig c, int retry_time);
        void DisConnect();
        void SubScribe(string topic, int qos);
        void UnSubscribe(string topic);
        void Publish(string topic, string mess, int qos, int retry_time);
};

class MqttService{
    private:
        static int nums_of_context;
        ICommand* command;
    public:
        MqttService(ICommand *command){
            this->command = command;
        }
        ~MqttService(){};
        void Start();
        void Stop();
        void AcquireContext(MqttContext context);
        void ReleaseContext(MqttContext context);
};

int MqttService::nums_of_context = 0;

void MqttService::Start(){
    string cmd = "AT+CMQTTSTART\r\n";
    this->command->send(cmd, 2000000);
}

void MqttService::Stop(){
    string cmd = "AT+CMQTTSTOP\r\n";
    this->command->send(cmd, 2000000);
}

void MqttService::AcquireContext(MqttContext context){
    string cmd = "AT+CMQTTACCQ=";
   
    cmd.append(context.id);
    cmd.append(",\"");
    cmd.append(context.name);
    cmd.append("\",");
    cmd.append(context.scheme);
    cmd.append(",");
    cmd.append(context.version);
    cmd.append("\r\n");

    this->command->send(cmd, 2000000);
}

void MqttService::ReleaseContext(MqttContext context){
    string cmd = "AT+CMQTTREL=";
    cmd.append(context.id);
    cmd.append("\r\n");
    this->command->send(cmd, 2000000);
}

void MqttContext::Connect(MqttConfig config, int retry_time){
    static int retry = 0;
    string cmd = "AT+CMQTTCONNECT=";

    cmd.append(this->id);
    cmd.append(",\"");
    if (this->scheme.compare("0") == 0){
        cmd.append("tcp");
    }
    cmd.append("://");
    cmd.append(config.host);
    cmd.append(":");
    cmd.append(config.port);
    cmd.append("\",");
    cmd.append(config.keepALive);
    cmd.append(",");
    cmd.append(config.cleanSession);
    if((config.username != "") && (config.password != "")){
        cmd.append(",\"");
        cmd.append(config.username);
        cmd.append("\",\"");
        cmd.append(config.password);
        cmd.append("\"");
    }
    cmd.append("\r\n");
    bool connect_success = this->command->send_with_response_checking(cmd, "+CMQTTCONNECT: 0,0\r\n", 0);

    if(retry_time == 0){
        return;
    }

    if(!connect_success){
        retry = retry + 1;
        this->Connect(config, retry_time);
        if (retry == retry_time){
            return;
        }
    }
    retry = 0;
}

void MqttContext::Publish(string topic, string mess, int qos, int retry_time){
    static int retry = 0;
    this->setPubTopic(topic);
    this->setPubPayload(mess);
    bool pub_success = this->sendDataToBroker(qos);

    if(retry_time == 0){
        return;
    }

    if(!pub_success){
        retry = retry + 1;
        this->Publish(topic, mess, qos, retry_time);
        if (retry == retry_time){
            return;
        }
    }
    retry = 0;
}

void MqttContext::setPubTopic(string topic){
    string cmd = "AT+CMQTTTOPIC=";
    string topic_len;

    stringstream ss;
    ss << topic.length();
    ss >> topic_len;

    cmd.append(this->id);
    cmd.append(",");
    cmd.append(topic_len);
    cmd.append("\r\n");

    this->command->send_with_response_checking(cmd, ">", 0);
    string cmd_topic = topic.append("\r\n");
    this->command->send_with_response_checking(cmd_topic, "OK\r\n", 0);
}

void MqttContext::setPubPayload(string payload){
    string cmd = "AT+CMQTTPAYLOAD=";
    string payload_len;

    stringstream ss;
    ss << payload.length();
    ss >> payload_len;

    cmd.append(this->id);
    cmd.append(",");
    cmd.append(payload_len);
    cmd.append("\r\n");
    this->command->send_with_response_checking(cmd, ">", 0);
    string payload_send = payload.append("\r\n");
    this->command->send_with_response_checking(payload_send, "OK\r\n", 0);
}

bool MqttContext::sendDataToBroker(int qos){
    string cmd = "AT+CMQTTPUB=";
    string qos_send;

    stringstream ss;
    ss << qos;
    ss >> qos_send;

    cmd.append(this->id);
    cmd.append(",");
    cmd.append(qos_send);
    cmd.append(",120\r\n");
    bool send_success = this->command->send_with_response_checking(cmd, "+CMQTTPUB: 0,0\r\n", 0);
    return send_success;
}

void MqttContext::SubScribe(string topic, int qos){
    this->setSubTopic(topic, qos);
    this->sendSubCommnad();
}

void MqttContext::setSubTopic(string topic, int qos){
    string cmd = "AT+CMQTTSUBTOPIC=";
    string topic_len;
    string qos_send;

    stringstream ss2;
    ss2 << topic.length();
    ss2 >> topic_len;

    stringstream ss3;
    ss3 << qos;
    ss3 >> qos_send;

    cmd.append(this->id);
    cmd.append(",");
    cmd.append(topic_len);
    cmd.append(",");
    cmd.append(qos_send);
    cmd.append("\r\n");

    this->command->send_with_response_checking(cmd, ">", 0);
    string cmd_topic = topic.append("\r\n");
    this->command->send_with_response_checking(cmd_topic, "OK\r\n", 0);
}

void MqttContext::sendSubCommnad(){
    string sub_cmd = "AT+CMQTTSUB=";
    sub_cmd.append(this->id);
    sub_cmd.append("\r\n");
    this->command->send_with_response_checking(sub_cmd, "+CMQTTSUB: 0,0\r\n", 0);
}

void MqttContext::UnSubscribe(string topic){
    this->unSetSubTopic(topic);
    this->sendUnSubCmd();
}

void MqttContext::unSetSubTopic(string topic){
    string cmd = "AT+CMQTTUNSUBTOPIC=";
    string topic_len;

    stringstream ss2;
    ss2 << topic.length();
    ss2 >> topic_len;

    cmd.append(this->id);
    cmd.append(",");
    cmd.append(topic_len);
    cmd.append("\r\n");
    this->command->send_with_response_checking(cmd, ">", 0);
    string cmd_topic = topic.append("\r\n");
    this->command->send_with_response_checking(cmd_topic, "OK\r\n", 0);
}

void MqttContext::sendUnSubCmd(){
    string cmd = "AT+CMQTTUNSUB=";
    cmd.append(this->id);
    cmd.append(",0\r\n");
    this->command->send(cmd, 1000000);
}

void MqttContext::DisConnect(){
    string cmd = "AT+CMQTTDISC=";
    cmd.append(this->id);
    cmd.append(",120\r\n");
    this->command->send(cmd, 2000000);
}
