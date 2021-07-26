#include <iostream>
#include "../Contracts/IBuffer.hpp"

using namespace std;

class UartDataReceiverBuffer: public IBuffer
{
    private:
        int write_point;
        int read_point;
        int bytes_written;
        char *data;

    public:
        UartDataReceiverBuffer(int size);

        ~UartDataReceiverBuffer();

        void push(char dt) override;

        char pop() override;

        char get(int index) override;

        void reset() override;

        int get_write_point() override;

        int get_read_point() override;

        int get_bytes_written() override;

        bool is_full() override;

        int get_size() override;
};

UartDataReceiverBuffer::UartDataReceiverBuffer(int size): IBuffer(size)
{
    this->write_point = 0;
    this->read_point = 0;
    this->bytes_written = 0;
    this->data = new char[size];
}

UartDataReceiverBuffer::~UartDataReceiverBuffer()
{
}

void UartDataReceiverBuffer::push(char dt){
    if(this->bytes_written == this->size){
        return;
    }

    this->data[this->write_point] = dt;
    this->write_point = this->write_point+ 1;

    if(this->write_point == this->size){
        this->write_point = 0;
    }
    this->bytes_written = this->bytes_written + 1;
}

char UartDataReceiverBuffer::pop(){
    if (this->bytes_written == 0){
        return '\0';
    }

    char temp = this->data[this->read_point];
    this->read_point = this->read_point + 1;

    if(this->read_point == this->size){
        this->read_point = 0;
    }

    this->bytes_written = this->bytes_written - 1;
    return temp;
}

char UartDataReceiverBuffer::get(int index){

    if(index >= this->size){
        return '\0';
    }

    return this->data[index];
}

void UartDataReceiverBuffer:: reset(){
    this->bytes_written = this->write_point = this->read_point = 0;
}

int UartDataReceiverBuffer::get_write_point(){
    return this->write_point;
}

int UartDataReceiverBuffer::get_read_point(){
    return this->read_point;
}

int UartDataReceiverBuffer::get_bytes_written(){
    return this->bytes_written;
}

int UartDataReceiverBuffer::get_size(){
    return this->size;
}

bool UartDataReceiverBuffer::is_full(){
    return false;
}
