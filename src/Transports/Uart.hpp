#include <iostream>
#include "../Contracts/ITransport.hpp"
#include<iostream>
#include<string.h>
#include<fcntl.h>
#include<errno.h>
#include<termios.h>
#include <unistd.h>
using namespace std;

class Uart: public ITransport
{
    private:
        int port;
    public:
        Uart(string port_name);
        ~Uart();
        void send(string command) override;
        void receive(char* buf, int len) override;
        void set_up() override;
        bool get_response_checking_status() override;
        void set_response_checking_status(bool response_checking_status) override;
        void set_response_checking_data(string response_expect_data) override;
        string get_response_checking_data();
};

Uart::Uart(string port_name): ITransport(port_name)
{
    this->port = open(port_name.c_str(), O_RDWR);
}

Uart::~Uart()
{
}

void Uart::set_up(){
    
    struct termios tty;

    // Read in existing settings, and handle any error
    if(tcgetattr(this->port, &tty) != 0) {
        return;
    }

    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
    tty.c_cflag |= CS8; // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
    // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

    tty.c_cc[VTIME] = 0;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    if (tcsetattr(this->port, TCSANOW, &tty) != 0) {
    	return;
    }
}

void Uart::send(string command){
    write(this->port, command.c_str(), command.length());
    return;
}

void Uart::receive(char* buf, int len){
    read(this->port, buf, len);
}

bool Uart::get_response_checking_status()
{
    return this->response_checking_state;
}

void Uart::set_response_checking_status(bool response_checking_status)
{
    this->response_checking_state = response_checking_status;
}

void Uart::set_response_checking_data(string response_expect_data)
{
    this->response_checking_data = response_expect_data;
}

string Uart::get_response_checking_data()
{
    return this->response_checking_data;
}
