#pragma once
#include<glm/vec3.hpp>
#include<string>


namespace audio {
    // AudioEngine - singleton, manages all audio events
    struct AudioEngine {
        AudioEngine();
        ~AudioEngine();
    };

    //load file with selected path. Returns non-negative index on success
    int audioLoad(const char* path, std::string uuid);

    //get internal id corresponding to uuid
    inline int audioGetId(const std::string& uuid);

    //play the selected audioclip
    void audioPlay(int id);

    //convenience function - directly play clip by uuid
    void audioPlay(std::string uuid);

    //set position for selected audio clip
    void audioSetPosition(const glm::vec3& pos);



    //stop selected clip
    void audioStop();

    //stop all clips
    void audioStopAll();

    void audioClearAll();
}
