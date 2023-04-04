#include "audio.h"
#include<soloud.h>
#include<soloud_wav.h>
#include<vector>
#include<memory>

#include "../logging.h"

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

    void audioClearAll() {
        audioStopAll();
        loadedClips.clear();
    }

    AudioEngine::~AudioEngine() {
        //TODO - custom allocator
        audioClearAll();
        pSoloud->deinit();
        delete pSoloud;
    }

    int audioLoad(const char* path) {
        loadedClips.emplace_back(std::make_shared<SoLoud::Wav>());
        SoLoud::result res = loadedClips.back()->load(path);
        if (res == SoLoud::SO_NO_ERROR) {
            return (loadedClips.size() - 1);
        }
        else {
            logging::logErr("Failed to load audio file {}\n", path);
        };

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