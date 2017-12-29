#pragma once

#include "../ClientAPI/User.h"
#include "Poco/Thread.h"

namespace client {

    class Pharmacist : public User, public Poco::Runnable
    {
    public:
        Pharmacist(const std::string &username, 
                   const std::string &password,
                   UserCallback *userCallback);

        void run();
        void start();
        void stop();

        void startVideoReceiver();
        void stopVideoReceiver();
        void startAudioReceiver();
        void stopAudioReceiver();

    private:
        // -----------------------------------------------------
        class VideoReceiver : public Poco::Runnable
        {
        public:
            VideoReceiver(Pharmacist &pharmacist) : _pharmacist(pharmacist) {}

            void run()
            {
                _pharmacist.startReceivingVideo();
            }

            void start()
            {
                _thread.start(*this);
            }

            void stop()
            {
                _pharmacist.stopReceivingVideo();
                _thread.join();
            }

        private:
            Pharmacist      &_pharmacist;
            Poco::Thread    _thread;
        };

        // -----------------------------------------------------
        class AudioReceiver : public Poco::Runnable
        {
        public:
            AudioReceiver(Pharmacist &pharmacist) : _pharmacist(pharmacist) {}

            void run()
            {
                _pharmacist.startReceivingAudio();
            }

            void start()
            {
                _thread.start(*this);
            }

            void stop()
            {
                _pharmacist.stopReceivingAudio();
                _thread.join();
            }

        private:
            Pharmacist      &_pharmacist;
            Poco::Thread    _thread;
        };

        Poco::Thread  _thread;
        UserCallback  *_userCallback;
        VideoReceiver *_videoReceiver;
        AudioReceiver *_audioReceiver;
    };
}
