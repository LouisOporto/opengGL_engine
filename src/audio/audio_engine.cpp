#include "audio_engine.hpp"

AudioEngine* AudioEngine::m_instance = nullptr;

bool AudioEngine::init() {
    FMOD_RESULT result;

    result = FMOD_Studio_System_Create(&m_studioSystem, FMOD_VERSION);
    if (result != FMOD_OK) {
        Logger::Error("FMOD::ERROR::(%d)", result);
        return false;
    }

    result = FMOD_Studio_System_Initialize(m_studioSystem, 512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
    if (result != FMOD_OK) {
        Logger::Error("FMOD::ERROR::(%d)", result);
        return false;
    }

    return true;
}