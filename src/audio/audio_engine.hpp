#pragma once
#include "../../FMOD/FMOD Studio API Windows/api/studio/inc/fmod_studio.hpp"
#include "../engine/logger.hpp"

class AudioEngine {
    public:
        static AudioEngine* getInstance() { return m_instance = m_instance != nullptr ? m_instance : new AudioEngine(); }
        bool init();
    private:
        AudioEngine() {}
        FMOD::Studio::System* m_system = nullptr;
        static AudioEngine* m_instance;
};