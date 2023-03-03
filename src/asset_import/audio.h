#pragma once
#include<soloud.h>
#include<soloud_wav.h>

//AudioClip - wrapper over SoLoud::Wav
typedef SoLoud::Wav AudioClip;

// AudioEngine - singleton, manages all audio events
struct AudioEngine {
    //private:
    SoLoud::Soloud* pSoloud;
    int playbackHandle;
    //public:
    AudioEngine() {
        static SoLoud::Soloud* hndl = NULL;
        if (hndl == NULL) {
            //TODO - custom allocator
            hndl = new SoLoud::Soloud();
            hndl->init();
        }
        pSoloud = hndl;
    }
    ~AudioEngine() {
        //TODO - custom allocator
        delete pSoloud;
    }
    //play the selected audioclip
    void play(AudioClip& clip) {
        playbackHandle = pSoloud->play(clip);
    }
};

