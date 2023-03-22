#include "audio.h"
#include<soloud.h>
#include<soloud_wav.h>
#include<vector>
#include<memory>


namespace audio {
    static SoLoud::Soloud* pSoloud;
    static SoLoud::handle playbackHandle;
    static glm::vec3 position = glm::vec3(.0f);
    //TODO - custom allocator
    static std::vector<std::shared_ptr<SoLoud::Wav>> loadedClips;

    AudioEngine::AudioEngine() {
        //TODO - custom allocator
        pSoloud = new SoLoud::Soloud();
        pSoloud->init();
    }

    AudioEngine::~AudioEngine() {
        //TODO - custom allocator
        audioStopAll();
        loadedClips.clear();
        pSoloud->deinit();
        delete pSoloud;
    }

    int audioLoad(const char* path) {
        loadedClips.emplace_back(std::make_shared<SoLoud::Wav>());
        SoLoud::result res = loadedClips.back()->load(path);
        if (res == SoLoud::SO_NO_ERROR) {
            return (loadedClips.size() - 1);
        }

        loadedClips.pop_back();

        return -1;
    }

    void audioPlay(int id) {
        playbackHandle = pSoloud->play3d(*loadedClips[id], position.x,
            position.y, position.z);
    }

    void audioSetPosition(const glm::vec3& pos) {
        position = pos;
        pSoloud->set3dSourcePosition(playbackHandle, position.x,
            position.y, position.z);
        pSoloud->update3dAudio();
    }

    void audioStop() {
        pSoloud->stop(playbackHandle);
    }

    void audioStopAll() {
        pSoloud->stopAll();
    }

}