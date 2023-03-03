#pragma once
#include<soloud.h>
#include<soloud_wav.h>
#include<glm/vec3.hpp>

//AudioClip - thin wrapper over SoLoud::Wav
// typedef SoLoud::Wav AudioClip;
struct AudioClip: SoLoud::AudioSource {
    SoLoud::Wav wav;
    SoLoud::AudioSourceInstance* createInstance() {
        return wav.createInstance();
    }
    SoLoud::result load(const char* path) {
        SoLoud::result res = wav.load(path);
        return res;
    }
};

// AudioEngine - singleton, manages all audio events
struct AudioEngine {
private:
    SoLoud::Soloud* pSoloud;
    int playbackHandle;
    glm::vec3 position = glm::vec3(.0f);
public:
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
        pSoloud->stopAll();
        pSoloud->deinit();
        delete pSoloud;
    }
    //play the selected audioclip
    void play(AudioClip& clip) {
        playbackHandle = pSoloud->play3d(clip, position.x,
            position.y, position.z);
    }
    void setPosition(const glm::vec3& pos) {
        position = pos;
        pSoloud->set3dSourcePosition(playbackHandle, position.x,
            position.y, position.z);
        pSoloud->update3dAudio();
    }
};

