#include "BufferedOutputStream.h"

namespace sserver {

	BufferedOutputStream::BufferedOutputStream(unsigned int buffSize, const std::string &fName) 
		: _buffSize(buffSize), _current(0), _fos(fName) 
	{
		_pBuff = new char[_buffSize];
	}

	BufferedOutputStream::~BufferedOutputStream()
	{
        close();
		delete [] _pBuff;
	}

	void BufferedOutputStream::write(const char *src, unsigned int len) 
	{
		if (_current + len > _buffSize) flush();
		memcpy(&_pBuff[_current], src, len);
		_current += len;
	}

	void BufferedOutputStream::flush() 
	{
		if (_current == 0) return;
		_fos.write(_pBuff, _current);
		_current = 0;
	}

	void BufferedOutputStream::close() 
	{
		flush();
		_fos.close();
	}

}