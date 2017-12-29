#pragma once

#include "../ClientAPI/User.h"
#include "Poco/Thread.h"

namespace client {

    class Patient : public User, public Poco::Runnable
    {
    public:
        Patient(const std::string &username, 
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
            VideoReceiver(Patient &patient) : _patient(patient) {}

            void run()
            {
                _patient.startReceivingVideo();
            }

            void start()
            {
                _thread.start(*this);
            }
            
            void stop()
            {
                _patient.stopReceivingVideo();
                _thread.join();
            }

        private:
            Patient         &_patient;
            Poco::Thread    _thread;
        };

        // -----------------------------------------------------
        class AudioReceiver : public Poco::Runnable
        {
        public:
            AudioReceiver(Patient &patient) : _patient(patient) {}

            void run()
            {
                _patient.startReceivingAudio();
            }

            void start()
            {
                _thread.start(*this);
            }

            void stop()
            {
                _patient.stopReceivingAudio();
                _thread.join();
            }

        private:
            Patient         &_patient;
            Poco::Thread    _thread;
        };

        Poco::Thread  _thread;
        UserCallback  *_userCallback;
        VideoReceiver *_videoReceiver;
        AudioReceiver *_audioReceiver;
    };
}
