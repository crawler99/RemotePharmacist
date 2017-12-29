#include "SimpleFileSaver.h"
#include "../logger/Logger.h"
#include "Poco/Path.h"
#include "Poco/File.h"
#include "Poco/FileStream.h"

namespace sserver { namespace server {

    SimpleFileSaver::SimpleFileSaver(const std::string &basePath) 
        : _basePath(basePath)
    {
    }

    void SimpleFileSaver::addFileToSave(const File &file)
    {
        _queue.push(file);
    }

    void SimpleFileSaver::run()
    {
        File mf;
        _queue.pop(mf);

        while (mf._stopFlag == false)
        {
            std::string absDir = _basePath + Poco::Path::separator() + mf._relativeDir;
            std::string absFile = absDir + Poco::Path::separator() + mf._name;
            try
            {
                // create directories
                Poco::File p(absDir);
                p.createDirectories();

                // save file
                /*
                std::locale oldLoc = std::locale::global(std::locale(""));
                std::string path(mf._dir + "/" + mf._name);
                std::ofstream outfile(path.c_str(), std::ofstream::binary);
                outfile.write((const char*)(mf._pData), mf._len);
                outfile.flush();
                outfile.close();
                std::locale::global(std::locale(oldLoc));
                */

				Poco::FileOutputStream fos(absFile);
                fos.write((const char*)(mf._pData), mf._len);
				fos.flush();
                fos.close();

                delete [] mf._pData;   // should delete the source pointer here
            }
            catch (...)
            {
                LOG_ERROR("Failed to save file: " + absFile);
            }

            _queue.pop(mf);
        }
    }

    void SimpleFileSaver::start()
    {
        _thread.start(*this);
    }

    void SimpleFileSaver::stop()
    {
        File f;
        f._stopFlag = true;
        _queue.push(f);
        _thread.join();
    }

}}
