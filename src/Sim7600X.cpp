#include<iostream>
#include<pthread.h>
#include "Constants/Constant.hpp"
#include "Handlers/UartDataHandler.hpp"
#include "Contracts/ITransport.hpp"
#include "Transports/Uart.hpp"
#include "Contracts/ICommand.hpp"
#include "Commands/AtCommand.hpp"
#include "Contracts/IBuffer.hpp"
#include "Buffers/UartDataReceiverBuffer.hpp"
#include "Contracts/IObserver.hpp"
#include "Observers/UartObserver.hpp"
#include "Contracts/IHandler.hpp"
#include "Handlers/HttpDataHandler.hpp"
#include "Handlers/MqttDataHandler.hpp"
#include "SimServices/Http.hpp"
#include "SimServices/Mqtt.hpp"
#include "SimServices/Heartbeat.hpp"
#include <stdlib.h>
#include <unistd.h>

#define UART_BUFFER_SIZE 10000
using namespace std;

bool exhibit_flag = true;

pthread_mutex_t myMutex;

ITransport* uart = new Uart("/dev/ttyUSB0");

ICommand* at = new AtCommand(uart);

IBuffer* uart_buf = new UartDataReceiverBuffer(UART_BUFFER_SIZE);

IObserver *uart_observer = new UartObserver();

UartDataHandler uart_handler(uart, uart_buf, uart_observer);

IHandler* httpDataHandler = new HttpDataHandler();

IHandler* mqttDataHandler = new MqttDataHandler();

MqttService mqtt(at);

MqttContext mqtt_context_1(at, "0", "client test0", MQTT_SCHEME_TCP, MQTT_VER_3_1_1);

MqttConfig config_1("broker.mqttdashboard.com", "1883", "", "RD", "1", "0", "60");

HttpService http(at);

Heartbeat heartbeat(at);

void System_init(){

    while (true)
    {
        bool connect_with_module_sim_success = at->send_with_response_checking("ATE\r\n", "OK\r\n", 0);
        if(connect_with_module_sim_success){
            break;
        }
        sleep(1);
    }

//    while (true)
//    {
//        bool connect_with_module_sim_success = at->send_with_response_checking(" AT+CICCID\r\n", "+ICCID: ", 0);
//        if(connect_with_module_sim_success){
//            break;
//        }
//        at->send_with_response_checking("AT+CRESET\r\n", "OK\r\n", 0);
//        sleep(30);
//    }

    mqtt_context_1.DisConnect();
    mqtt.ReleaseContext(mqtt_context_1);
    mqtt.Stop();
    mqtt.Start();
    mqtt.AcquireContext(mqtt_context_1);
    mqtt_context_1.Connect(config_1, 0);
    mqtt_context_1.SubScribe("zxcv", 2);

    http.Stop();
    http.Start();

    exhibit_flag = false;

    cout << "system init done" << endl;
}

void* internet_heartbeat(void*){
    while(exhibit_flag == true);

    while (true)
    {
        pthread_mutex_lock(&myMutex);
        heartbeat.ping_to_internet();
        heartbeat.report_status();
        pthread_mutex_unlock(&myMutex);
        sleep(15);
    }

}

void* sim_insert_checking(void*){
    while(exhibit_flag == true);

    while (true)
    {
        pthread_mutex_lock(&myMutex);
        bool sim_inserted_state = heartbeat.check_sim_insert();
        if(!sim_inserted_state){
			at->send("AT+CRESET\r\n", 500000);
			sleep(30);
			bool connect_with_module_sim_success = at->send_with_response_checking(" AT+CICCID\r\n", "+ICCID: ", 0);
			if(connect_with_module_sim_success){
	            System_init();
			}
        }
        pthread_mutex_unlock(&myMutex);
        sleep(10);
    }
}

void* uart_receive_data(void* argv){
    while(true)
    {
        usleep(100000);
        uart_handler.receive_uart_data();
    }
}

void* uart_handler_data(void* argv){
    while (true)
    {
        usleep(100000);
        uart_handler.handler_uart_data();
    }
}

void* mqtt_handler_data(void* argv){
    while (true)
    {
        mqttDataHandler->handler();
    }
}

void* http_handler_data(void* argv){
    while (true)
    {
        httpDataHandler->handler();
    }
}

void* test_case(void* argv){
    while(exhibit_flag);

    pthread_mutex_lock(&myMutex);
    map<string, string>header;
    header.insert(pair<string, string>("URL", "https://api.github.com/users/hadley/orgs"));
    HttpRequest req2 = http.CreateRequest("", header);
    http.Get(req2, 0);
    pthread_mutex_unlock(&myMutex);

    while (true)
    {
        pthread_mutex_lock(&myMutex);
        mqtt_context_1.Publish("zxcv", "tuan", 1, 0);
        pthread_mutex_unlock(&myMutex);
        sleep(5);
    }
}

int main(){
    uart->set_up();
    uart_observer->subscribe(httpDataHandler);
    uart_observer->subscribe(mqttDataHandler);

    pthread_mutex_init(&myMutex,0);

    pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;
    pthread_t thread4;
    pthread_t thread5;
    pthread_t thread6;
//    pthread_t thread7;

    pthread_create(&thread1, NULL, uart_receive_data, NULL);
    pthread_create(&thread2, NULL, uart_handler_data, NULL);
    pthread_create(&thread3, NULL, mqtt_handler_data, NULL);
    pthread_create(&thread4, NULL, http_handler_data, NULL);
    pthread_create(&thread5, NULL, test_case, NULL);
    pthread_create(&thread6, NULL, internet_heartbeat, NULL);
//    pthread_create(&thread7, NULL, sim_insert_checking, NULL);

    System_init();

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);
    pthread_join(thread5, NULL);
    pthread_join(thread6, NULL);
//    pthread_join(thread7, NULL);

    pthread_mutex_destroy(&myMutex);

    return 0;
}
