#ifndef DATA_BUFFER_H
#define DATA_BUFFER_H

#include <string>

class DataBuffer
{
public:
	DataBuffer(int size);
	DataBuffer(char * data, int size);
	DataBuffer(const char * data, int size);
	~DataBuffer();

	void copy(int offset, void * source, int size);
	void copy(int offset, const void * source, int size);
	void clear();
	char get(int index);
	void set(int index, char x);

	char * getData() { return data; }
	int getSize() { return size; }
	void setSize(int x) { size = x; }
private:
	char * data;
	int size, maxSize;
};

#endif