#include "AudioSourceComponent.h"

#include "../../asset_import/audio.h"

void AudioSourceComponent::start() {
    if (playOnStart)
        startSound();
}
void AudioSourceComponent::update(float dt, EntityState& state) {
    // update position
}
void AudioSourceComponent::stop() {
}

void AudioSourceComponent::startSound() {
    assert(clipDescriptor);
    audio::audioSetLoop(clipDescriptor->idx, loop);
    audio::audioPlay(clipDescriptor->idx);
}
