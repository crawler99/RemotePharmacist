#pragma once

#include "Poco/Foundation.h"

namespace sserver { namespace message { 

#pragma pack(push, 1)

    /* 
     * Header for every tcp message
     * 1. _pktVersion and _pktType are automatically set.
     * 2. _pktLen is the total number of bytes for each concrete message, including this header.
     * 3. When login succeeded, the _sessionId will be assigned by the session server.
     *    This non-zero value must be present in all the succeeding message exchanges.
     */
    struct Header
    {
    public:
        Header();
        // --------------------------------------------------------
        Poco::UInt32   getPktLen() const;
        Poco::UInt16   getPktVersion() const;
        Poco::UInt16   getPktType() const;
        Poco::UInt32   getSessionId() const;
        // --------------------------------------------------------
        void           setPktLen(Poco::UInt32 pktLen);
        void           setPktVersion(Poco::UInt16 pktVersion);
        void           setPktType(Poco::UInt16 pktType);
        void           setSessionId(Poco::UInt32 sessionId);

    private:
        Poco::UInt32   _pktLen;
        Poco::UInt16   _pktVersion;
        Poco::UInt16   _pktType;
        Poco::UInt32   _sessionId;
    };

    /*
     * Binary block which could be inserted into any other message
     */
    struct BinaryBlock
    {
        Poco::UInt32  getContentLen() const;
        void          setContentLen(Poco::UInt32 contentLen);

    private:
        Poco::UInt32  _contentlen;
    };

    /* 
     * Message to transfer the prescription between pharmacist and patient
     */
    struct Prescription
    {
        Prescription();
        // --------------------------------------------------------
        Header        _header;
        // --------------------------------------------------------
        
        // prescription body should be appended as binary block
    };

    /* 
    * Message to transfer the video between pharmacist and patient
    */
    struct Video
    {
        Video();
        // --------------------------------------------------------
        Header        _header;
        // --------------------------------------------------------
        
        // video body should be appended as binary block
    };

    /* 
    * Message to transfer the audio between pharmacist and patient
    */
    struct Audio
    {
        Audio();
        // --------------------------------------------------------
        Header        _header;
        // --------------------------------------------------------

        // audio body should be appended as binary block
    };

#pragma pack(pop)

} }
    