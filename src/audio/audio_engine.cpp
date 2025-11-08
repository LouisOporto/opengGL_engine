#include "audio_engine.hpp"

AudioEngine* AudioEngine::m_instance = nullptr;

bool AudioEngine::init() {
    FMOD_RESULT result;

    FMOD::Studio::System::create(&m_system);
    if (result != FMOD_OK) {
        Logger::Error("FMOD::ERROR::(%d)", result);
        return false;
    }

    result = m_system->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
    if (result != FMOD_OK) {
        Logger::Error("FMOD::ERROR::(%d)", result);
        return false;
    }

    return true;
}