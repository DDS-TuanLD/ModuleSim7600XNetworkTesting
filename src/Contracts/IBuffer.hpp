#pragma once

class IBuffer
{
    protected:
        int size;
    public:
        IBuffer(int size);
        virtual ~IBuffer();

        virtual void push(char dt);

        virtual char pop();

        virtual char get(int index);

        virtual void reset();

        virtual int get_write_point();

        virtual int get_read_point();

        virtual int get_bytes_written();

        virtual bool is_full();

        virtual int get_size();
};

IBuffer::IBuffer(int size)
{
    this->size = size;
}

IBuffer::~IBuffer()
{
}

void IBuffer::push(char dt){
    
}

char IBuffer::pop(){
    return '\0';
}

char IBuffer::get(int index){
    return '\0';
}

void IBuffer::reset(){

}

int IBuffer::get_write_point(){
    return 0;
}

int IBuffer::get_read_point(){
    return 0;
}

int IBuffer::get_bytes_written(){
    return 0;
}

bool IBuffer::is_full(){
    return false;
}

int IBuffer::get_size(){
    return this->size;
}



