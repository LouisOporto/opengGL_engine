#ifndef _AUDIO_ENGINE_H_
#define _AUDIO_ENGINE_H_

#include <map>
#include <string>

#include "../../FMOD/api/studio/inc/fmod_studio.h"
#include "../../FMOD/api/core/inc/fmod.h"
#include "../engine/logger.hpp"

class AudioEngine {
    public:
        static AudioEngine* getInstance() { return m_instance = m_instance != nullptr ? m_instance : new AudioEngine(); }
        bool init();

        void loadBank(std::string filename, const std::string& directory);
        void dropBank(std::string filename);

        void playByIndex(std::string filename, int numberFiles, unsigned int index);
        void playByPath(std::string path);
        void playTest(std::string filename) { playByIndex(filename, 10, 0); }
        void stop(std::string filename);
        void update();

        void clean();
    private:
        AudioEngine() {}
        static AudioEngine* m_instance;
        FMOD_STUDIO_SYSTEM* m_system = nullptr;
        std::map<std::string, FMOD_STUDIO_BANK*> m_banks;
};

#endif // _AUDIO_ENGINE_H_