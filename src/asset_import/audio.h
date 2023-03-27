#pragma once
#include<glm/vec3.hpp>


namespace audio {
    // AudioEngine - singleton, manages all audio events
    struct AudioEngine {
        AudioEngine();
        ~AudioEngine();
    };

    //load file with selected path. Returns non-negative index on success
    int audioLoad(const char* path);

    //play the selected audioclip
    void audioPlay(int id);

    //set position for selected audio clip
    void audioSetPosition(const glm::vec3& pos);

    //stop selected clip
    void audioStop();

    //stop all clips
    void audioStopAll();
}
