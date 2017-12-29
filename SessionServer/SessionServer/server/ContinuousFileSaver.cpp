#include "ContinuousFileSaver.h"
#include "../logger/Logger.h"
#include "Poco/Path.h"
#include "Poco/File.h"
#include "Poco/FileStream.h"
#include <sstream>

namespace sserver { namespace server {

    ContinuousFileSaver::ContinuousFileSaver(const std::string &basePath) 
        : _basePath(basePath)
    {
    }

    void ContinuousFileSaver::addFileToSave(const File &file)
    {
        _queue.push(file);
    }

    void ContinuousFileSaver::run()
    {
        File mf;
        _queue.pop(mf);

		while (mf._type != File::CHUNK_GLOBAL_END)
        {
			switch (mf._type)
			{
			case File::CHUNK_FILE_START:
				{
					std::string &date = mf._date;
					std::string &time = mf._time;

					std::stringstream ss;
					ss << _basePath << Poco::Path::separator() << date 
						<< Poco::Path::separator() << mf._generatorId;
					std::string absDir = ss.str();

					ss.str("");
					ss << absDir << Poco::Path::separator() << time;
					std::string absFile =  ss.str();

					try
					{
						// create directories
						Poco::File p(absDir);
						p.createDirectories();

						// create fos
						_saverMap.insert(std::make_pair(mf._generatorId, new sserver::BufferedOutputStream(1024 * 1024 * 4, absFile)));
					}
					catch (...)
					{
						LOG_ERROR("Failed to create file writer: " + absFile);
					}

					break;
				}
			case File::CHUNK_FILE_END:
				{
					FOSMap::accessor acc;
					if (_saverMap.find(acc, mf._generatorId))
					{
						_saverMap.erase(acc);
					}
					break;
				}
			default:
				{
					if (mf._len > 0) 
					{
						FOSMap::accessor acc;
						if (_saverMap.find(acc, mf._generatorId))
						{
							try
							{
								acc->second->write((const char*)(mf._pData), mf._len);
								delete [] mf._pData;  // should delete the source pointer here
							}
							catch (...)
							{
								LOG_ERROR("Failed to save file id: " + mf._generatorId);
							}
						}	
					}
					break;		
				}
			}

			// get next
            _queue.pop(mf);
        }
    }

    void ContinuousFileSaver::start()
    {
        _thread.start(*this);
    }

    void ContinuousFileSaver::stop()
    {
        File f;
        f._type = File::CHUNK_GLOBAL_END;
        _queue.push(f);
        _thread.join();
    }

}}
