#ifndef _AUDIO_ENGINE_H_
#define _AUDIO_ENGINE_H_

#include <map>
#include <vector>
#include <string>

#include "../../FMOD/api/studio/inc/fmod_studio.h"
#include "../../FMOD/api/core/inc/fmod.h"
#include "../engine/logger.hpp"



struct Music {
    Music(std::string songName, int totalMins, int totalSecs, int currentMins = 0, int currentSecs = 0, bool paused = false, bool stop = false, bool released = false) {
        name = songName;
        totalMin = totalMins;
        totalSec = totalSecs;
        curMin = currentMins;
        curSec = currentSecs;
        isPaused = paused;
        isStop = stop;
        isReleased = released;
    }

    std::string name;
    int totalMin;
    int totalSec;
    int curMin;
    int curSec;
    bool isPaused;
    bool isStop;
    bool isReleased;
};

class AudioEngine {
    public:
        static AudioEngine* getInstance() { return m_instance = m_instance != nullptr ? m_instance : new AudioEngine(); }
        bool init();

        void loadBank(std::string bankName, const std::string& directory);
        void dropBank(std::string bankName);

        void playByIndex(std::string bankName, unsigned int index = 0);
        void playByPath(std::string path);
        void playTest(std::string bankName) { playByIndex(bankName); }

        void setInstanceParemeter(std::string parameter, float value);
        void setTimelinePosition(int value);
        void readTimelinePosition();
        void release();
        void stop(std::string filename);
        void update();

        void clean();
    private:
        AudioEngine() {}
        static AudioEngine* m_instance;
        FMOD_STUDIO_SYSTEM* m_system = nullptr;
        std::map<std::string, FMOD_STUDIO_BANK*> m_banks;
        std::map<std::string, FMOD_STUDIO_EVENTINSTANCE*> m_eventInstances;
        FMOD_STUDIO_EVENTINSTANCE* instance;
        std::map<std::string, std::vector<unsigned int>> BANKMAP; // Relative bank directory
};

#endif // _AUDIO_ENGINE_H_