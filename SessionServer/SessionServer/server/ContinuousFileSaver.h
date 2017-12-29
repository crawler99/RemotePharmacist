#pragma once

#include "Poco/Thread.h"
#include "Poco/SharedPtr.h"
#include "tbb/concurrent_queue.h"
#include "tbb/concurrent_hash_map.h"
#include "../util/BufferedOutputStream.h"

namespace sserver { namespace server {

    class ContinuousFileSaver : public Poco::Runnable
    {
    public:
        ContinuousFileSaver(const std::string &basePath);

        struct File
        {
            enum ChunkType
            {
                CHUNK_NORMAL,
                CHUNK_GLOBAL_END,
                CHUNK_FILE_START,
                CHUNK_FILE_END
            };

            File() 
                : _generatorId(0)
                , _pData(NULL)
                , _len(0)
                , _type(CHUNK_NORMAL)
            {}

            Poco::UInt32             _generatorId;   // some kind of numeric id of the content generator
                                                     // (typically the client session id)
            void                     *_pData;
            Poco::UInt32             _len;
            ChunkType                _type;
			std::string              _date;
			std::string              _time;
        };

        void start();
        void stop();
        void run();
        void addFileToSave(const File &file);

    private:
        std::string _basePath;
        Poco::Thread _thread;
        tbb::concurrent_bounded_queue<File> _queue;

        // <id, output_stream>
        typedef 
            tbb::concurrent_hash_map<Poco::UInt32, Poco::SharedPtr<sserver::BufferedOutputStream> > FOSMap;
        FOSMap _saverMap;
    };

}}