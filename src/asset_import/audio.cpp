#include "audio.h"
#include <soloud.h>
#include <soloud_wav.h>
#include <string>
#include <map>
#include <vector>
#include <memory>

#include "../logging.h"

namespace audio {
    // --- Internal State ---
    static SoLoud::Soloud* pSoloud;
    static SoLoud::handle playbackHandle;
    static glm::vec3 position = glm::vec3(.0f);
    // TODO - custom allocator
    static std::unordered_map<std::string, int> uuidToIdx;

    struct AudioInfo {
        // redundant info for easy deletion
        std::string uuid;
        std::string path;
        std::shared_ptr<SoLoud::Wav> wav;
        std::weak_ptr<AudioDescriptor> ref;

        // brace init constructor
        AudioInfo(const std::string& _uuid, const std::string& _path,
            std::shared_ptr<SoLoud::Wav> _wav, std::weak_ptr<AudioDescriptor> _ref)
            : uuid(_uuid), path(_path), wav(_wav), ref(_ref) {
        }

        // copy constructor
        AudioInfo(AudioInfo& aInfo) {
            // copy as normal
            uuid = aInfo.uuid;
            path = aInfo.path;
            wav = aInfo.wav;
            ref = aInfo.ref;

            // fix path reference
            if (ref.lock())
                ref.lock()->path = &path;
        }

        AudioInfo& operator=(AudioInfo& aInfo) {
            // copy as normal
            uuid = aInfo.uuid;
            path = aInfo.path;
            wav = aInfo.wav;
            ref = aInfo.ref;

            // fix path reference
            if (ref.lock())
                ref.lock()->path = &path;

            return *this;
        }

        // move constructor
        AudioInfo(AudioInfo&& aInfo) {
            // copy as normal
            uuid = std::move(aInfo.uuid);
            path = std::move(aInfo.path);
            wav = std::move(aInfo.wav);
            ref = std::move(aInfo.ref);

            // fix path reference
            if (ref.lock())
                ref.lock()->path = &path;
        }

        // move assignment
        AudioInfo& operator=(AudioInfo&& aInfo) {
            // copy as normal
            uuid = std::move(aInfo.uuid);
            path = std::move(aInfo.path);
            wav = std::move(aInfo.wav);
            ref = std::move(aInfo.ref);

            // fix path reference
            if (ref.lock())
                ref.lock()->path = &path;

            return *this;
        }
    };
    static std::vector<AudioInfo> loadedClips;

    // --- Module RAII ---
    AudioEngine::AudioEngine() {
        // TODO - custom allocator
        pSoloud = new SoLoud::Soloud();
        pSoloud->init();
    }

    void clearAudio() {
        audioStopAll();
        uuidToIdx.clear();
        loadedClips.clear();
    }

    AudioEngine::~AudioEngine() {
        // TODO - custom allocator
        clearAudio();
        pSoloud->deinit();
        delete pSoloud;
    }

    // --- AudioDescriptor Methods ---
    AudioDescriptor::AudioDescriptor(int _idx, std::string* _path) : idx(_idx), path(_path) {
    }

    AudioDescriptor::~AudioDescriptor() {
        // stop everything to avoid issues
        audioStopAll();

        // skip if bulk clear has been performed
        if (loadedClips.size() == 0)
            return;

        // clear related index
        uuidToIdx.erase(loadedClips[idx].uuid);

        // move related audio clip to back
        for (unsigned int i = idx; i < loadedClips.size() - 1; i++) {
            // update pointer for all related references
            loadedClips[i + 1].ref.lock()->idx--;
            std::swap(loadedClips[i], loadedClips[i + 1]);
        }

        // clear audio clip
        loadedClips.pop_back();
    }

    // --- Module Functions ---

    std::shared_ptr<AudioDescriptor> audioLoad(const char* path, const std::string& uuid) {
        // try to add new clip
        if (uuidToIdx.count(uuid) == 0) {
            // construct new info element
            loadedClips.emplace_back(AudioInfo{uuid, std::string(path),
                std::make_shared<SoLoud::Wav>(), std::weak_ptr<AudioDescriptor>()});

            // try to load clip
            SoLoud::result res = loadedClips.back().wav->load(path);

            if (res == SoLoud::SO_NO_ERROR) {
                // loading succeeded
                uuidToIdx[uuid] = loadedClips.size() - 1;
                std::shared_ptr<AudioDescriptor> ptr = std::make_shared<AudioDescriptor>(
                    loadedClips.size() - 1, &loadedClips.back().path);
                loadedClips.back().ref = std::weak_ptr<AudioDescriptor>(ptr);
                return ptr;
            }

            // loading failed
            logging::logErr("Failed to load audio file {}\n", path);
            // remove failed element
            loadedClips.pop_back();
            // return empty descriptor
            return std::shared_ptr<AudioDescriptor>();
        }

        // replace old clip otherwise
        int idx = uuidToIdx[uuid];
        loadedClips[idx].wav->load(path);
        loadedClips[idx].path = std::string(path);
        return std::shared_ptr<AudioDescriptor>(loadedClips[idx].ref);
    }

    std::shared_ptr<AudioDescriptor> audioGetByUuid(const std::string& uuid) {
        int idx = uuidToIdx.count(uuid) ? uuidToIdx[uuid] : -1;
        if (idx >= 0)
            return std::shared_ptr<AudioDescriptor>(loadedClips[idx].ref);

        return std::shared_ptr<AudioDescriptor>();
    }

    void audioPlay(int id) {
        playbackHandle = pSoloud->play3d(*loadedClips[id].wav, position.x, position.y, position.z);
    }

    void audioPlay(const std::string& uuid) {
        int id = uuidToIdx[uuid];
        playbackHandle = pSoloud->play3d(*loadedClips[id].wav, position.x, position.y, position.z);
    }

    void audioSetPosition(const glm::vec3& pos) {
        position = pos;
        pSoloud->set3dSourcePosition(playbackHandle, position.x, position.y, position.z);
        pSoloud->update3dAudio();
    }

    void audioStop() {
        pSoloud->stop(playbackHandle);
    }

    void audioStopAll() {
        pSoloud->stopAll();
    }

    int getAudioClipCount() {
        return loadedClips.size();
    }
}
