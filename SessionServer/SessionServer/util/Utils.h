#pragma once

#include "Poco/ByteOrder.h"
#include "Poco/Net/StreamSocket.h"
#include "zlib.h"
#include <sstream>

namespace sserver {

    const static char DEFAULT_PADDING_CHAR = 0x00;

    class Utils
    {
    public:

        // Read utilities
        // ================================================================================================
        static std::string readRightPaddedStr(const char *src, unsigned int srcLen, 
                                              char padChar = DEFAULT_PADDING_CHAR);

        template <class numT>
        static numT readNumericField(const numT &src)
        {
            return Poco::ByteOrder::fromBigEndian(src);
        }

        // Write utilities
        // ================================================================================================
        static void writeRightPaddedStr(char *dst, const char *src, unsigned int dstLen, 
                                        unsigned int srcLen, char padChar = DEFAULT_PADDING_CHAR);

        template <class numT>
        static void writeNumericField(numT &dst, numT numVal)
        {
            dst = Poco::ByteOrder::toBigEndian(numVal);
        }

        // Convert utilities
        // ================================================================================================
        template <class numT>
        static bool string2Num(const std::string &str, numT &val)
        {
            Poco::UInt64 temp;
            std::stringstream ss;
            ss << str;
            bool succ = !((ss >> temp).fail());
            if (succ)
            {
                val = static_cast<numT>(temp);
            }
            return succ;
        }

        template <class numT>
        static void num2String(numT val, std::string &str)
        {
            std::stringstream ss;
            ss << val;
            str = ss.str();
        }

        static std::string genCredential(const std::string &str);

        // Socket utilities
        // ================================================================================================
        static bool sendBytes(Poco::Net::StreamSocket &sock, const void *buffer, unsigned int len);
        
        static bool recvBytes(Poco::Net::StreamSocket &sock, char *buffer, unsigned int len);

		static void closeSocket(Poco::Net::StreamSocket &sock);

        // Compress utilities
        // ==================
        // NOTICE: dst will be newed from inside and should be deleted from outside as unsigned char array
        // ================================================================================================
        static int compressData(unsigned char *dst, uLongf &dstLen, const unsigned char *src, uLong srcLen);
    };

}