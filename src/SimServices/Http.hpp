#include <map>
#include <string.h>
#include "../Contracts/ICommand.hpp"
#include <sstream>

using namespace std;

typedef enum HTTPREQUEST{
	GET=0,
	POST,
	PUT,
	DELETE
}HTTPREQUEST;

class HttpRequest{
	public:
		string body;
		map<string, string>header;

	public:
		HttpRequest(){
			this->body = "";
			this->header = map<string, string>();
		};

		~HttpRequest(){};
};

class HttpService{
	private:
		ICommand* command;

	private:
		void sendHttpRequest(HttpRequest req, HTTPREQUEST r, int retry_time);
		void sendReqHeader(map<string, string>header);
		void sendReqBody(string body);
		bool sendReqCmd(HTTPREQUEST r);

	public:
		HttpService(ICommand* command){
			this->command = command;
		}
		~HttpService(){};
		void Start();
		void Stop();
		HttpRequest CreateRequest(string body, map<string, string>header);
		void Get(HttpRequest req, int retry_time);
		void Post(HttpRequest req, int retry_time);
		void Put(HttpRequest req, int retry_time);
		void Del(HttpRequest req, int retry_time);
};

void HttpService::Start(){
	string http_start_cmd = "AT+HTTPINIT\r\n";
	this->command->send(http_start_cmd, 2000000);
}

void HttpService::Stop(){
	string http_stop_cmd = "AT+HTTPTERM\r\n";
	this->command->send(http_stop_cmd, 2000000);
}

HttpRequest HttpService::CreateRequest(string body, map<string, string>header){
	HttpRequest req;
	req.body = body;
	req.header = header;
	return req;
}

void HttpService::Get(HttpRequest req, int retry_time){
	this->sendHttpRequest(req, GET, retry_time);
}

void HttpService::Post(HttpRequest req, int retry_time){
	this->sendHttpRequest(req, POST, retry_time);
}

void HttpService::sendHttpRequest(HttpRequest req, HTTPREQUEST r, int retry_time){
	static int retry = 0;

	this->sendReqHeader(req.header);
	this->sendReqBody(req.body);
	bool req_success = this->sendReqCmd(r);

	if(retry_time == 0){
        return;
    }

	if(!req_success){
        retry = retry + 1;
        this->sendHttpRequest(req, r ,retry_time);
        if (retry == retry_time){
            retry = 0;
            return;
        }
    }
    retry = 0;	
}

void HttpService::sendReqHeader(map<string, string>header){
	map<string, string>::iterator itr;	
	for (itr = header.begin(); itr != header.end(); ++itr) {
		string add_header_cmd = "AT+HTTPPARA=\"";
		add_header_cmd.append(itr->first);
		add_header_cmd.append("\",\"");
		add_header_cmd.append(itr->second);
		add_header_cmd.append("\"\r\n");
		
		this->command->send_with_response_checking(add_header_cmd, "OK\r\n", 0);
    }
	return;
}

void HttpService::sendReqBody(string body){
	if(body.length() != 0){
		string add_body_cmd = "AT+HTTPDATA=";

		stringstream ss;
		ss<<body.length();
		string body_lenght;
		ss>>body_lenght;

		add_body_cmd.append(body_lenght);
		add_body_cmd.append(",10000\r\n");

		this->command->send_with_response_checking(add_body_cmd, "DOWNLOAD\r\n", 0);
		this->command->send_with_response_checking(body, "OK\r\n", 0);	
	}	
	return;
}

bool HttpService::sendReqCmd(HTTPREQUEST r){
	
	stringstream ss;
	string http_method;
	ss<< r;
	ss>> http_method;

	string get_http_req_cmd = "AT+HTTPACTION=";
	get_http_req_cmd.append(http_method);
	get_http_req_cmd.append("\r\n");
	
	string res_expect = "+HTTPACTION: ";;
	res_expect.append(http_method);
	res_expect.append(",200");

	bool req_success = this->command->send_with_response_checking(get_http_req_cmd, res_expect, 0);
	return req_success;
}
