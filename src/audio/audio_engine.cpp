#include "audio_engine.hpp"

AudioEngine* AudioEngine::m_instance = nullptr;

bool AudioEngine::init() {
    FMOD_RESULT result;

    result = FMOD_Studio_System_Create(&m_system, FMOD_VERSION);
    if (result != FMOD_OK) {
        Logger::Error("FMOD::ERROR::(%d)", result);
        return false;
    }

    result = FMOD_Studio_System_Initialize(m_system, 512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
    if (result != FMOD_OK) {
        Logger::Error("FMOD::ERROR::(%d)", result);
        return false;
    }

    return true;
}

void AudioEngine::loadBank(std::string bankName, const std::string& directory) {
    // The filename is used to locate the bank location, but the actual string that maps is just the concatenated to the file name
    // Ex. Filename: "Master.bank" String Mapping: "Master"
    std::string filename = bankName + ".bank";
    if (m_banks.count(bankName) == 0) {
        std::string path = directory + "/" + filename;
        if (FMOD_Studio_System_LoadBankFile(m_system, path.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &m_banks[bankName]) != FMOD_OK) {
            Logger::Error("Error loading bank from directory: %s", path.c_str());
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
        Logger::Log("Drop file: %s", bankName.c_str());
    }
}

bool AudioEngine::checkInstance(std::string eventName) {
    if (m_eventInstances.count(eventName) == 0) return false;
    else return true;
}

Event AudioEngine::getEvent(std::string eventName) {
    if (checkInstance(eventName)) return m_eventInstances[eventName];
    else return Event();
}

void AudioEngine::playByIndex(std::string bankName, std::string name, unsigned int index, bool release) {
    if (checkInstance(name)) {
        Logger::Warn("Event instance with name: %s already exists", name.c_str());
        return;
    }
    
    // If we want index i. Get event list of i + 1
    FMOD_STUDIO_EVENTDESCRIPTION** descript = new FMOD_STUDIO_EVENTDESCRIPTION*[index + 1];
    int numberLoaded = 0;
    int length = 0;
    Event event;

    FMOD_Studio_Bank_GetEventList(m_banks[bankName], descript, index + 1, &numberLoaded);
    Logger::Warn("Number of events in given bank file: %d", numberLoaded);

    if (FMOD_Studio_EventDescription_GetLength(descript[index], &length) != FMOD_OK) {
        Logger::Error("Failed to retrieve length");
    }

    event = Event(name, length);
    Logger::Warn("Total length of event: %d:%d", length / 60000, length / 1000 % 60);

    if (FMOD_Studio_EventDescription_CreateInstance(descript[index], &event.instance) != FMOD_OK) {
        Logger::Error("Failed to create instance");
    }

    if (FMOD_Studio_EventInstance_Start(event.instance) != FMOD_OK) {
        Logger::Error("Failed to start instance");
    }

    // Insert event into map
    event.toRelease = release;
    m_eventInstances[name] = event;

    delete[] descript;
}

void AudioEngine::playByPath(std::string index, std::string name, bool release) {
    if (checkInstance(name)) {
        Logger::Warn("Event instance with name: %s already exists", name.c_str());
        return;
    }
    FMOD_STUDIO_EVENTDESCRIPTION* descript;
    Event event;

    if (FMOD_Studio_System_GetEvent(m_system, index.c_str(), &descript) != FMOD_OK) {
        Logger::Error("Failed to retrieve event named: %s", index.c_str());
    }

    int length = 0;
    if (FMOD_Studio_EventDescription_GetLength(descript, &length) != FMOD_OK) {
        Logger::Error("Failed to retrieve length");
    }
    
    event = Event(name, length);
    Logger::Warn("Total length of event: %d:%d", length / 60000, length / 1000 % 60);

    // FMOD_STUDIO_EVENTINSTANCE* instance;
    if (FMOD_Studio_EventDescription_CreateInstance(descript, &event.instance) != FMOD_OK) {
        Logger::Error("Failed to create instance");
    }

    if (FMOD_Studio_EventInstance_Start(event.instance) != FMOD_OK) {
        Logger::Error("Failed to start instance");
    }
    m_eventInstances[name] = event;

    if (release) setToRelease(name);
}

void AudioEngine::setToRelease(std::string eventName) {
    if (checkInstance(eventName)) {
        m_eventInstances[eventName].toRelease = true;
    }
}

void AudioEngine::releaseInstance(std::string eventName) {
    if (checkInstance(eventName)) {
        if (FMOD_Studio_EventInstance_Release(m_eventInstances[eventName].instance) != FMOD_OK) {
            Logger::Error("Failed to release instance");
        }
        m_eventInstances[eventName].isReleased = true;
    }
}

void AudioEngine::stop(std::string eventName, bool release) {
    if (checkInstance(eventName) && !m_eventInstances[eventName].isReleased) {
        if (FMOD_Studio_EventInstance_Stop(m_eventInstances[eventName].instance, FMOD_STUDIO_STOP_IMMEDIATE) != FMOD_OK) {
            Logger::Error("Failed to stop event");
        }
        m_eventInstances[eventName].toRelease = release;
    }
}

void AudioEngine::update() {
    if (FMOD_Studio_System_Update(m_system) != FMOD_OK) {
        Logger::Error("Failed to update FMOD");
    }

    for (auto iter = m_eventInstances.begin(); iter != m_eventInstances.end(); iter++) {
        if (iter->second.toRelease) releaseInstance(iter->first);

        if (iter->second.isStop && iter->second.isReleased) {
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

void AudioEngine::setInstanceParemeter(std::string eventName, std::string parameter, float value) {
    if (checkInstance(eventName)) {
        if (FMOD_Studio_EventInstance_SetParameterByName(m_eventInstances[eventName].instance, parameter.c_str(), value, false) != FMOD_OK) {
            Logger::Error("Failed to set parameter");
        }
        setTimelinePosition(eventName, value * 15000);
    }
}

void AudioEngine::setTimelinePosition(std::string eventName, int value) {
    if (checkInstance(eventName)) {
        if (FMOD_Studio_EventInstance_SetTimelinePosition(m_eventInstances[eventName].instance, value) != FMOD_OK) {
            Logger::Error("Failed to set FMOD value");
        }
        readTimelinePosition(eventName);
    }
}

void AudioEngine::readTimelinePosition(std::string eventName) {
    if (checkInstance(eventName)) {
        int value = 0;
        if (FMOD_Studio_EventInstance_GetTimelinePosition(m_eventInstances[eventName].instance, &value) != FMOD_OK) {
            Logger::Error("Error reading");
        }
        Logger::Log("TimelinePosition: %d:%d", value / 60000, value / 1000 % 60);
    }
}

void AudioEngine::updateCurrentPosition(std::string eventName) {
    if (checkInstance(eventName)) {
        int value = 0;
        if (FMOD_Studio_EventInstance_GetTimelinePosition(m_eventInstances[eventName].instance, &value) != FMOD_OK) {
            Logger::Error("Error reading");
        }

        m_eventInstances[eventName].currentPos = value;
    }
}

void AudioEngine::setSoundVolume(std::string eventName, int volume) {
    if (checkInstance(eventName)) {
        if (FMOD_Studio_EventInstance_SetVolume(m_eventInstances[eventName].instance, volume / 100.f) != FMOD_OK) {
            Logger::Error("Error setting volume");
        }
        Logger::Log("Setting volume");
    }
}