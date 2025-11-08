#pragma once
#include "../../FMOD/FMOD Studio API Windows/api/studio/inc/fmod_studio.h"
#include "../../FMOD/FMOD Studio API Windows/api/core/inc/fmod.h"
#include "../engine/logger.hpp"

class AudioEngine {
    public:
        static AudioEngine* getInstance() { return m_instance = m_instance != nullptr ? m_instance : new AudioEngine(); }
        bool init();
        void update();
        void clean();

        void loadSound(const char* filename);
        
    private:
        AudioEngine() {}
        FMOD_STUDIO_SYSTEM* m_studioSystem = nullptr;
        FMOD_STUDIO_BANK* m_banks[4];
        static AudioEngine* m_instance;
};