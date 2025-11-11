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

void AudioEngine::playByIndex(std::string bankName, unsigned int index) {
    // If we want index i. Get event list of i + 1
    FMOD_STUDIO_EVENTDESCRIPTION** descript = new FMOD_STUDIO_EVENTDESCRIPTION*[index + 1];
    int numberLoaded = 0;

    FMOD_Studio_Bank_GetEventList(m_banks[bankName], descript, index + 1, &numberLoaded);
    Logger::Warn("Number of events in given bank file: %d", numberLoaded);

    if (FMOD_Studio_EventDescription_CreateInstance(descript[index], &instance) != FMOD_OK) {
        Logger::Error("Failed to create instance");
    }

    int length = 0;
    if (FMOD_Studio_EventDescription_GetLength(descript[index], &length) == FMOD_OK) {
        int minutes = length / 60000;
        int seconds = length / 1000 % 60;
        Logger::Warn("Total length of event: %d:%d", minutes, seconds);
    }

    if (FMOD_Studio_EventInstance_Start(instance) != FMOD_OK) {
        Logger::Error("Failed to start instance");
    }

    // release();

    delete[] descript;
}

void AudioEngine::playByPath(std::string index) {
    FMOD_STUDIO_EVENTDESCRIPTION* descript;

    if (FMOD_Studio_System_GetEvent(m_system, index.c_str(), &descript) != FMOD_OK) {
        Logger::Error("Failed to retrieve event named: %s", index.c_str());
    }

    // FMOD_STUDIO_EVENTINSTANCE* instance;
    if (FMOD_Studio_EventDescription_CreateInstance(descript, &instance) != FMOD_OK) {
        Logger::Error("Failed to create instance");
    }

    int length = 0;
    if (FMOD_Studio_EventDescription_GetLength(descript, &length) != FMOD_OK) {
        Logger::Warn("Total length of event: %d", length);
    }

    if (FMOD_Studio_EventInstance_Start(instance) != FMOD_OK) {
        Logger::Error("Failed to start instance");
    }

    // release();
}

void AudioEngine::release() {
    if (FMOD_Studio_EventInstance_Release(instance) != FMOD_OK) {
        Logger::Error("Failed to release instance");
    }
}

void AudioEngine::stop(std::string filename) {
    // if (FMOD_Studio_EventInstance_Stop())
}

void AudioEngine::update() {
    if (FMOD_Studio_System_Update(m_system) != FMOD_OK) {
        Logger::Error("Failed to update FMOD");
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
}

void AudioEngine::setInstanceParemeter(std::string parameter, float value) {
    if (FMOD_Studio_EventInstance_SetParameterByName(instance, parameter.c_str(), value, false) != FMOD_OK) {
        Logger::Error("Failed to set parameter");
    }

    setTimelinePosition(value * 15000);
}

void AudioEngine::setTimelinePosition(int value) {
    if (FMOD_Studio_EventInstance_SetTimelinePosition(instance, value) != FMOD_OK) {
        Logger::Error("Failed to set FMOD value");
    }
    readTimelinePosition();
}

void AudioEngine::readTimelinePosition() {
    int value = 0;
    if (FMOD_Studio_EventInstance_GetTimelinePosition(instance, &value) != FMOD_OK) {
        Logger::Error("Error reading");
    }
    Logger::Log("TimelinePosition: %f", value / 1000.f);
}