#include "Patient.h"

namespace client {

    Patient::Patient(const std::string &username, 
                     const std::string &password,
                     UserCallback *userCallback) 
        : User(username, password, userCallback) 
    {
        _videoReceiver = new VideoReceiver(*this);
        _audioReceiver = new AudioReceiver(*this);
    }

    void Patient::run()
    {
        startReceiving();
    }

    void Patient::start()
    {
        _thread.start(*this);
    }

    void Patient::stop()
    {
        stopReceiving();
        _thread.join();
    }

    void Patient::startVideoReceiver()
    {
        _videoReceiver->start();
    }

    void Patient::stopVideoReceiver()
    {
        _videoReceiver->stop();
    }

    void Patient::startAudioReceiver()
    {
        _audioReceiver->start();
    }

    void Patient::stopAudioReceiver()
    {
        _audioReceiver->stop();
    }

}