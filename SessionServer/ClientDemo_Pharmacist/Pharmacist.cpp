#include "Pharmacist.h"

namespace client {

    Pharmacist::Pharmacist(const std::string &username, 
                           const std::string &password,
                           UserCallback *userCallback) 
        : User(username, password, userCallback) 
    {
        _videoReceiver = new VideoReceiver(*this);
        _audioReceiver = new AudioReceiver(*this);
    }

    void Pharmacist::run()
    {
        startReceiving();
    }

    void Pharmacist::start()
    {
        _thread.start(*this);
    }

    void Pharmacist::stop()
    {
        stopReceiving();
        _thread.join();
    }

    void Pharmacist::startVideoReceiver()
    {
        _videoReceiver->start();
    }

    void Pharmacist::stopVideoReceiver()
    {
        _videoReceiver->stop();
    }

    void Pharmacist::startAudioReceiver()
    {
        _audioReceiver->start();
    }

    void Pharmacist::stopAudioReceiver()
    {
        _audioReceiver->stop();
    }

}