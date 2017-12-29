#pragma once

#include "Poco/FileStream.h"

namespace sserver {

	class BufferedOutputStream 
	{
	public:
		BufferedOutputStream(unsigned int buffSize, const std::string &fName);
		~BufferedOutputStream();

		void write(const char *src, unsigned int len);
		void flush();
		void close(); 

	private:
		unsigned int             _buffSize;
		char                     *_pBuff;
		unsigned int             _current;
		Poco::FileOutputStream   _fos;
	};

}