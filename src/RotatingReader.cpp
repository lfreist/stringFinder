// Copyright Leon Freist
// Author Leon Freist <freist@informatik.uni-freiburg.de>

#include "RotatingReader.h"

Buffer::Buffer(bool last) {
	_read = false;
	_wrote = false;
  _next = nullptr;
	_content = nullptr;
	_last = last;
}

Buffer::Buffer(String* content, bool last) {
	_content = content;
	_read = false;
	_wrote = true;
	_next = nullptr;
	_last = last;
}

Buffer::~Buffer() {
	if (!isLast()) {
		delete _next;
	}
}

void Buffer::write(String* content) {
	_content = content;
	_read = false;
	_wrote = true;
}

void Buffer::setNext(Buffer* next) {
	_next = next;
}

String* Buffer::read() {
	if (_content == nullptr) {
		return nullptr;
	}
	_read = true;
	_wrote = false;
  return _content;
}

Buffer* Buffer::getNext() {
  return _next;
}

bool Buffer::getRead() {
	return _read;
}

bool Buffer::getWrote() {
	return _wrote;
}

bool Buffer::isLast() {
	return _last;
}

// ______________________________________

RotatingReader::RotatingReader(unsigned int nBuffers) {
	_readBuffer = nullptr;
	_writeBuffer = nullptr;
	_firstBuffer = nullptr;
	_nBuffers = nBuffers;
	if (nBuffers == 0) { return; }
	if (nBuffers == 1) {
		_writeBuffer = new Buffer(true);
		_readBuffer = _writeBuffer;
		return;
	}
	Buffer* buf = nullptr;
	_firstBuffer = new Buffer();
	_writeBuffer = _firstBuffer;
	for (unsigned int i = 2; i < nBuffers; i++) {
		buf = new Buffer();
		_writeBuffer->setNext(buf);
		_writeBuffer = buf;
	}
	Buffer* last = new Buffer(true);
	_writeBuffer->setNext(last);
	last->setNext(_firstBuffer);
	_writeBuffer = _firstBuffer;
	_readBuffer = _firstBuffer;
}

RotatingReader::~RotatingReader() {
	delete _firstBuffer;
}

Buffer* RotatingReader::getNextReadBuffer() {
	for (unsigned int i = 0; i < _nBuffers; i++) {
		if (!_readBuffer->getRead() && _readBuffer->read() != nullptr) {
			return _readBuffer;
		}
		_readBuffer = _readBuffer->getNext();
	}
	return nullptr;
}

Buffer* RotatingReader::getNextWriteBuffer() {
	Buffer* tmp = _writeBuffer;
	if (_writeBuffer->getNext() == nullptr) {
		return tmp;
	}
	_writeBuffer = _writeBuffer->getNext();
	return tmp;
}
