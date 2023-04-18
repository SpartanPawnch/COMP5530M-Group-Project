#pragma once
#include <glm/vec3.hpp>

#include <memory>
#include <string>

namespace audio {
    // AudioEngine - singleton, manages all audio events
    struct AudioEngine {
        AudioEngine();
        ~AudioEngine();
    };

    // Object representing internal audio clip.
    // Returned as shared pointer and to be used as such
    struct AudioDescriptor {
        int idx;
        std::string* path;
        AudioDescriptor(int _idx, std::string* _path);
        ~AudioDescriptor();
    };

    // load file with selected path. Returns non-negative index on success
    std::shared_ptr<AudioDescriptor> audioLoad(const char* path, const std::string& uuid);

    // get descriptor corresponding to uuid
    std::shared_ptr<AudioDescriptor> audioGetByUuid(const std::string& uuid);

    // play the selected audioclip
    void audioPlay(int id);

    // convenience function - directly play clip by uuid
    void audioPlay(std::string uuid);

    // set position for selected audio clip
    void audioSetPosition(const glm::vec3& pos);

    // stop selected clip
    void audioStop();

    // stop all clips
    void audioStopAll();

    // clear all loaded clips
    void clearAudio();

    // get clips count
    int getAudioClipCount();
}
