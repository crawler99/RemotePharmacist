#include "Utils.h"
#include "Poco/Net/NetException.h"
#include "Poco/DigestStream.h"
#include "Poco/MD5Engine.h"
#include "../logger/Logger.h"

namespace sserver {

    std::string Utils::readRightPaddedStr(const char *src, unsigned int srcLen, char padChar)
    {
        const char *ptr = src;
        unsigned int ctr = 0;
        while((ctr < srcLen) && (*ptr != padChar)) { ++ctr; ++ptr; }
        return std::string(src, ctr);
    }

    void Utils::writeRightPaddedStr(char *dst, const char *src, unsigned int dstLen, 
                                    unsigned int srcLen, char padChar)
    {
        unsigned int actual = std::min<unsigned int>(dstLen, srcLen);
        memcpy(dst, src, actual);
        memset(dst + actual, padChar, dstLen - actual);
    }

	std::string Utils::genCredential(const std::string &str)
	{
		Poco::MD5Engine md5;
		Poco::DigestOutputStream ostr(md5);
		ostr << str;
		ostr.flush();
		const Poco::DigestEngine::Digest &digest = md5.digest();
		return Poco::DigestEngine::digestToHex(digest);
	}

    bool Utils::sendBytes(Poco::Net::StreamSocket &sock, const void *buffer, unsigned int len)
    {
        if (len == 0) return true;

        const char *p = static_cast<const char*>(buffer);                                                                                                                            
        unsigned int n = 0;
        do 
        {  
            try
            {
                unsigned int size = sock.sendBytes(p+n, len-n);                                                                                                                        
                if (size < 0)
                {
                    LOG_ERROR("Failed to send data, abort");
                    return false;
                }
                n += size;
            }                     
            catch (Poco::Net::NetException &e)                                                                                                                                       
            {
                LOG_ERROR(std::string("Error when sending data: [") + e.displayText() + "]");
                return false;
            }
			catch (...)
			{
				LOG_ERROR("Error when sending data for unknown reason");
				return false;
			}
        } while (n < len);

        return (n == len);
    }

    bool Utils::recvBytes(Poco::Net::StreamSocket &sock, char *buffer, unsigned int len)
    {
        if (len == 0) return true;

        unsigned int n = 0;
        do 
        {  
            try
            {
                unsigned int size = sock.receiveBytes(buffer+n, len-n);
                if (size == 0)
                {
                    LOG_ERROR("Socket is shut down by client");
                    return false;
                }
                n += size;
            }
            catch (Poco::TimeoutException &)
            {
                LOG_WARNING("Receiving timeout, try again ...");
                continue;
            }
            catch (Poco::Net::NetException &e)
            {
                LOG_ERROR(std::string("Error when receiving data: [") + e.displayText() + "]"); 
                return false;
            }
			catch (...)
			{
				LOG_ERROR("Error when receiving data for unknown reason");
				return false;
			}
        } while(n < len);

        return (n == len);
    }

	void Utils::closeSocket(Poco::Net::StreamSocket &sock)
	{
		try
		{
			sock.shutdown();
		}
		catch (...)
		{
		}

		try
		{
			sock.close();
		}
		catch (...)
		{
		}
	}

    int Utils::compressData(Bytef *dst, uLongf &dstLen, const Bytef *src, uLong srcLen)
    {
        uLong rstLen = compressBound(srcLen);
        dst = new unsigned char[rstLen];
        return compress(dst, &dstLen, src, srcLen);
    }
}