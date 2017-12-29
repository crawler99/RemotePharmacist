#pragma once

#include "Poco/Thread.h"
#include "tbb/concurrent_queue.h"

namespace sserver { namespace server {

    class SimpleFileSaver : public Poco::Runnable
    {
    public:
        SimpleFileSaver(const std::string &basePath);
        
        struct File
        {
            File() : _relativeDir(""), _name(""), _pData(NULL), _len(0), _stopFlag(false) {}

            std::string    _relativeDir;
            std::string    _name;
            void           *_pData;
            Poco::UInt32   _len;
            bool           _stopFlag;
        };

        void start();
        void stop();
        void run();
        void addFileToSave(const File &file);

    private:
        std::string _basePath;
        Poco::Thread _thread;
        tbb::concurrent_bounded_queue<File> _queue;
    };

}}