// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include <vector>

#include "String.h"

class Buffer {
 public:
  Buffer(bool last = false);
	Buffer(String* content, bool last = false);
	~Buffer();

	void write(String* content);
	void setNext(Buffer* next);
	String* read();
	Buffer* getNext();
	bool getRead();
	bool getWrote();
	bool isLast();

 private:
  String* _content;
	Buffer* _next;
	bool _last;
	bool _read;
	bool _wrote;

	FRIEND_TEST(RotatingReaderTest, bufferConstructorTest);
};

class RotatingReader {
 public:
	RotatingReader(unsigned int nBuffers);
	~RotatingReader();

	Buffer* getNextReadBuffer();
	Buffer* getNextWriteBuffer();

 private:
  unsigned int _nBuffers;
	Buffer* _readBuffer;
	Buffer* _writeBuffer;
	Buffer* _firstBuffer;


	FRIEND_TEST(RotatingReaderTest, RotatingReaderConstructorTest);
};
