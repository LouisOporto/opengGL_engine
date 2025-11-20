#include "audio_engine.hpp"

AudioEngine* AudioEngine::m_instance = nullptr;

bool AudioEngine::init() {
    if (FMOD_Studio_System_Create(&m_system, FMOD_VERSION) != FMOD_OK) {
        Logger::Error("FMOD::ERROR::Create");
        return false;
    }

    if (FMOD_Studio_System_Initialize(m_system, 512, FMOD_STUDIO_INIT_NORMAL,
                                      FMOD_INIT_NORMAL, 0) != FMOD_OK) {
        Logger::Error("FMOD::ERROR::Intialize");
        return false;
    }

    m_activeBank = "";
    m_activeEvent = "";

    return true;
}

void AudioEngine::loadBank(std::string bankName, const std::string& directory) {
    // The filename is used to locate the bank location, but the actual string
    // that maps is just the concatenated to the file name Ex. Filename:
    // "Master.bank" String Mapping: "Master"
    std::string filename = bankName + ".bank";
    if (m_banks.count(bankName) == 0) {
        std::string path = directory + "/" + filename;
        if (FMOD_Studio_System_LoadBankFile(m_system, path.c_str(),
                                            FMOD_STUDIO_LOAD_BANK_NORMAL,
                                            &m_banks[bankName]) != FMOD_OK) {
            Logger::Error("Error loading bank from directory: %s",
                          path.c_str());
        }
        Logger::Log("Loading bank file: %s", filename.c_str());
    }
}

void AudioEngine::dropBank(std::string bankName) {
    if (m_banks.count(bankName) != 0) {
        if (FMOD_Studio_Bank_Unload(m_banks[bankName]) != FMOD_OK) {
            Logger::Error("Failed to unload bank: %s", bankName);
        }
        m_banks.erase(bankName);

        if (bankName == m_activeBank) m_activeBank = "";
        Logger::Log("Drop file: %s", bankName.c_str());
    }
}

bool AudioEngine::checkInstance(std::string eventName) {
    return (m_eventInstances.count(eventName) != 0 && !eventName.empty());
}

Event AudioEngine::getActiveEvent() {
    if (m_activeEvent.empty()) return Event();
    return m_eventInstances[m_activeEvent];
}

FMOD_STUDIO_BANK* AudioEngine::getActiveBank() {
    if (m_activeBank.empty()) return nullptr;
    return m_banks[m_activeBank];
}

void AudioEngine::setActiveEvent(std::string eventName) {
    if (checkInstance(eventName)) {
        m_activeEvent = eventName;
        Logger::Log("Setting active evnt to: %s", eventName.c_str());
    } else
        Logger::Error("No event with name: %s exists", eventName.c_str());
}

void AudioEngine::setActiveBank(std::string bankName) {
    if (m_banks.count(bankName) != 0) {
        m_activeBank = bankName;
        Logger::Log("Setting active bank to: %s", bankName.c_str());
    } else
        Logger::Error("No bank with name: %s exists", bankName.c_str());
}

void AudioEngine::playByIndex(std::string name, unsigned int index,
                              bool release) {
    FMOD_STUDIO_BANK* bank = getActiveBank();
    if (checkInstance(name) && bank) {
        Logger::Warn("Event instance with name: %s already exists",
                     name.c_str());
        return;
    }

    // If we want index i. Get event list of i + 1
    FMOD_STUDIO_EVENTDESCRIPTION** descript =
        new FMOD_STUDIO_EVENTDESCRIPTION*[index + 1];
    int numberLoaded = 0;
    int length = 0;
    Event event;

    FMOD_Studio_Bank_GetEventList(bank, descript, index + 1, &numberLoaded);
    Logger::Warn("Number of events in given bank file: %d", numberLoaded);

    if (FMOD_Studio_EventDescription_GetLength(descript[index], &length) !=
        FMOD_OK) {
        Logger::Error("Failed to retrieve length");
    }

    event = Event(name, length);
    Logger::Warn("Total length of event: %d:%d", length / 60000,
                 length / 1000 % 60);

    if (FMOD_Studio_EventDescription_CreateInstance(
            descript[index], &event.instance) != FMOD_OK) {
        Logger::Error("Failed to create instance");
    }

    if (FMOD_Studio_EventInstance_Start(event.instance) != FMOD_OK) {
        Logger::Error("Failed to start instance");
    }

    // Insert event into map
    m_eventInstances[name] = event;
    setActiveEvent(name);
    if (release) releaseInstance();

    delete[] descript;
}

void AudioEngine::playByPath(std::string name, std::string path, bool release) {
    if (checkInstance(name)) {
        Logger::Warn("Event instance with name: %s already exists",
                     name.c_str());
        return;
    }
    FMOD_STUDIO_EVENTDESCRIPTION* descript;
    Event event;

    if (FMOD_Studio_System_GetEvent(m_system, path.c_str(), &descript) !=
        FMOD_OK) {
        Logger::Error("Failed to retrieve event named: %s", path.c_str());
    }

    int length = 0;
    if (FMOD_Studio_EventDescription_GetLength(descript, &length) != FMOD_OK) {
        Logger::Error("Failed to retrieve length");
    }

    event = Event(name, length);
    Logger::Warn("Total length of event: %d:%d", length / 60000,
                 length / 1000 % 60);

    // FMOD_STUDIO_EVENTINSTANCE* instance;
    if (FMOD_Studio_EventDescription_CreateInstance(
            descript, &event.instance) != FMOD_OK) {
        Logger::Error("Failed to create instance");
    }

    if (FMOD_Studio_EventInstance_Start(event.instance) != FMOD_OK) {
        Logger::Error("Failed to start instance");
    }

    m_eventInstances[name] = event;
    setActiveEvent(name);
    if (release) releaseInstance();
}

void AudioEngine::releaseInstance() {
    std::string eventName = m_activeEvent;
    if (checkInstance(eventName)) {
        if (FMOD_Studio_EventInstance_Release(
                m_eventInstances[eventName].instance) != FMOD_OK) {
            Logger::Error("Failed to release instance");
        }
        Logger::Log("Release instance");
        m_eventInstances[eventName].isReleased = true;
    }
}

void AudioEngine::pause() {
    std::string eventName = m_activeEvent;
    if (checkInstance(eventName) && !m_eventInstances[eventName].isPaused) {
        if (FMOD_Studio_EventInstance_SetPaused(
                m_eventInstances[eventName].instance, true)) {
            Logger::Error("Failed to pause instance");
        }

        m_eventInstances[eventName].isPaused = true;
    }
}

void AudioEngine::resume() {
    std::string eventName = m_activeEvent;
    if (checkInstance(eventName) && m_eventInstances[eventName].isPaused) {
        if (FMOD_Studio_EventInstance_SetPaused(
                m_eventInstances[eventName].instance, false)) {
            Logger::Error("Failed to resume instance");
        }

        m_eventInstances[eventName].isPaused = false;
    }
}

void AudioEngine::stop() {
    std::string eventName = m_activeEvent;
    if (checkInstance(eventName)) {
        if (FMOD_Studio_EventInstance_Stop(m_eventInstances[eventName].instance,
                                           FMOD_STUDIO_STOP_IMMEDIATE) !=
            FMOD_OK) {
            Logger::Error("Failed to stop event");
        }
        Logger::Log("Stopping instance");
        if (!m_eventInstances[eventName].isReleased) releaseInstance();
        m_eventInstances[eventName].isStop = true;
    }
}

void AudioEngine::update() {
    if (FMOD_Studio_System_Update(m_system) != FMOD_OK) {
        Logger::Error("Failed to update FMOD");
    }

    updateCurrentPosition();

    for (auto iter = m_eventInstances.begin(); iter != m_eventInstances.end();
         iter++) {
        if (iter->second.isStop && iter->second.isReleased) {
            Logger::Warn("Removed event instance: %s", iter->first.c_str());
            if (iter->first == m_activeEvent) m_activeEvent = "";
            m_eventInstances.erase(iter);
        }
    }
}

void AudioEngine::clean() {
    if (FMOD_Studio_System_Release(m_system) != FMOD_OK) {
        Logger::Error("Failed to release FMOD files");
    }

    for (auto iter = m_banks.begin(); iter != m_banks.end(); iter++) {
        FMOD_Studio_Bank_Unload(iter->second);
        m_banks.erase(iter);
    }
    m_banks.clear();
    m_eventInstances.clear();
}

void AudioEngine::setInstanceParemeter(std::string parameter, float value) {
    std::string eventName = m_activeEvent;
    if (checkInstance(eventName)) {
        if (FMOD_Studio_EventInstance_SetParameterByName(
                m_eventInstances[eventName].instance, parameter.c_str(), value,
                false) != FMOD_OK) {
            Logger::Error("Failed to set parameter");
        }
    }
}

void AudioEngine::forward() {
    std::string eventName = m_activeEvent;
    if (checkInstance(eventName)) {
        int newPos = m_eventInstances[eventName].currentPos + SONGADVANCE;
        if (FMOD_Studio_EventInstance_SetTimelinePosition(
                m_eventInstances[eventName].instance, newPos) != FMOD_OK) {
            Logger::Error("Failed to forward player");
        }
    }
    readTimelinePosition();
}

void AudioEngine::rewind() {
    std::string eventName = m_activeEvent;
    if (checkInstance(eventName)) {
        int newPos = m_eventInstances[eventName].currentPos - SONGADVANCE;
        if (FMOD_Studio_EventInstance_SetTimelinePosition(
                m_eventInstances[eventName].instance, newPos) != FMOD_OK) {
            Logger::Error("Failed to rewind player");
        }
        readTimelinePosition();
    }
}

void AudioEngine::setTimelinePosition(float value) {
    std::string eventName = m_activeEvent;
        if (checkInstance(eventName)) {
        // Set time line in milliseconds (set for now to by seconds)
        // value = value * 1000; // one sec is 1000 ms
        Event event = getActiveEvent();
        int newPos = event.totalPos * value;
        if (FMOD_Studio_EventInstance_SetTimelinePosition(
                m_eventInstances[eventName].instance, newPos) != FMOD_OK) {
            Logger::Error("Failed to set FMOD value");
        }
        readTimelinePosition();
    }
}

void AudioEngine::readTimelinePosition() {
    std::string eventName = m_activeEvent;
    if (checkInstance(eventName)) {
        int value = 0;
        if (FMOD_Studio_EventInstance_GetTimelinePosition(
                m_eventInstances[eventName].instance, &value) != FMOD_OK) {
            Logger::Error("Error reading: %s", m_eventInstances[eventName].name.c_str());
        }
        Logger::Log("TimelinePosition: %2d:%2d", value / 60000,
                    value / 1000 % 60);
    }
}

void AudioEngine::updateCurrentPosition() {
    std::string eventName = m_activeEvent;
    if (checkInstance(eventName)) {
        int value = 0;
        if (FMOD_Studio_EventInstance_GetTimelinePosition(
                m_eventInstances[eventName].instance, &value) != FMOD_OK) {
            Logger::Error("Error reading update");
        }

        m_eventInstances[eventName].currentPos = value;
    }
}

void AudioEngine::setSoundVolume(int volume) {
    std::string eventName = m_activeEvent;
    if (checkInstance(eventName)) {
        if (FMOD_Studio_EventInstance_SetVolume(
                m_eventInstances[eventName].instance, volume / 100.f) !=
            FMOD_OK) {
            Logger::Error("Error setting volume");
        }
        Logger::Log("Setting volume");
    }
}