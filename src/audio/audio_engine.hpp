#ifndef _AUDIO_ENGINE_H_
#define _AUDIO_ENGINE_H_

#include <map>
#include <vector>
#include <string>

#include "../../FMOD/api/studio/inc/fmod_studio.h"
#include "../../FMOD/api/core/inc/fmod.h"
#include "../engine/logger.hpp"



struct Event {
    Event(std::string songName = "", int total = 0, int current = 0, int volume = 0, bool paused = false, bool stop = false, bool released = false) {
        name = songName;
        totalPos = total;
        currentPos = current;
        isPaused = paused;
        isStop = stop;
        isReleased = released;

        FMOD_STUDIO_EVENTINSTANCE* instance = nullptr;
    }

    std::string name;
    int totalPos;
    int currentPos;
    int volume;
    bool isPaused;
    bool isStop;
    bool isReleased;
    FMOD_STUDIO_EVENTINSTANCE* instance;
};

class AudioEngine {
    public:
        static AudioEngine* getInstance() { return m_instance = m_instance != nullptr ? m_instance : new AudioEngine(); }
        bool init();

        void loadBank(std::string bankName, const std::string& directory);
        void dropBank(std::string bankName);

        bool checkInstance(std::string eventName);
        Event getEvent(std::string eventName);

        void playByIndex(std::string bankName, std::string name, unsigned int index = 0, bool release = false);
        void playByPath(std::string path, std::string name, bool release = false);
        void playTest(std::string bankName) { playByIndex(bankName, "Test"); }

        void setInstanceParemeter(std::string name, std::string parameter, float value);
        void setTimelinePosition(std::string eventName, int value);
        void setSoundVolume(std::string eventName, int value);
        void readTimelinePosition(std::string eventName);
        void updateCurrentPosition(std::string eventName);
        void releaseInstance(std::string eventName);
        void stop(std::string eventName);
        void update();

        void clean();
    private:
        AudioEngine() {}
        static AudioEngine* m_instance;
        FMOD_STUDIO_SYSTEM* m_system = nullptr;
        std::map<std::string, FMOD_STUDIO_BANK*> m_banks;
        std::map<std::string, Event> m_eventInstances; // Maybe inefficient if we need to remember all strin names. // Standard "effect1, effect2, background1, background2"

        // An idea would be to have an actively selected bank, actively selected event when handling playing or parameter adjustments.
        // Need a way to know if a event needs to be released and if the song is both released and stopped we can go ahead and remove it from the map of events.
        // Current design has a boolean for both to check isTrue status, but waste another to set it to release.
        // std::map<std::string, std::vector<unsigned int>> BANKMAP; // Relative bank directory
};

#endif // _AUDIO_ENGINE_H_