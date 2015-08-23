#include "DataBuffer.h"

DataBuffer::DataBuffer(int size)
{
	this->data = new char[size];
	this->maxSize = size;
	this->size = 0;
}

DataBuffer::DataBuffer(char * data, int size)
{
	this->data = new char[size];
	memcpy(this->data, data, size);
	this->size = this->maxSize = size;
}

DataBuffer::DataBuffer(const char * data, int size)
{
	this->data = new char[size];
	memcpy(this->data, data, size);
	this->size = this->maxSize = size;
}

DataBuffer::~DataBuffer()
{
	delete [] data;
}

void DataBuffer::copy(int offset, const void * source, int size)
{
	if (offset + size > this->maxSize) return;
	memcpy(data+offset, source, size);
	if (this->size < offset + size)
		this->size = offset+size;
}

void DataBuffer::copy(int offset, void * source, int size)
{
	copy(offset, (const char *)source, size);
}

void DataBuffer::clear()
{
	data[0] = '\0';
	size = 0;
}

char DataBuffer::get(int index)
{
	if (index >= size) return '\0';
	return data[index];
}

void DataBuffer::set(int index, char x)
{
	if (index >= size) return;
	data[index] = x;
}