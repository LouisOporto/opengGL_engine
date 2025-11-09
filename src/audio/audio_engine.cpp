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

void AudioEngine::loadBank(std::string filename, const std::string& directory) {
    if (m_banks.count(filename) == 0) {
        std::string path = directory + "/" + filename;
        if (FMOD_Studio_System_LoadBankFile(m_system, path.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &m_banks[filename]) != FMOD_OK) {
            Logger::Error("Error loading bank from directory: %s", path.c_str());
        }
        Logger::Log("Loading bank file: %s", filename.c_str());
    }
}

void AudioEngine::dropBank(std::string filename) {
    if (m_banks.count(filename) != 0) {
        FMOD_Studio_Bank_Unload(m_banks[filename]);
        m_banks.erase(filename);
        Logger::Log("Drop file: %s", filename.c_str());
    }
}

void AudioEngine::playByIndex(std::string filename, int numberFiles, unsigned int index = 0) {
    FMOD_STUDIO_EVENTDESCRIPTION** descript = new FMOD_STUDIO_EVENTDESCRIPTION*[numberFiles];
    int numberLoaded = 0;

    FMOD_Studio_Bank_GetEventList(m_banks[filename], descript, numberFiles, &numberLoaded);
    Logger::Warn("Number of events in given bank file: %d", numberLoaded);

    FMOD_STUDIO_EVENTINSTANCE* instance;
    if (FMOD_Studio_EventDescription_CreateInstance(descript[index], &instance) != FMOD_OK) {
        Logger::Error("Failed to create instance");
    }

    if (FMOD_Studio_EventInstance_Start(instance) != FMOD_OK) {
        Logger::Error("Failed to start instance");
    }

    // Do this only if you are not planning to reuse this instance again (ie playing) or will not be manipulating in game parameters
    if (FMOD_Studio_EventInstance_Release(instance) != FMOD_OK) {
        Logger::Error("Failed to release instance");
    }

    delete[] descript;
}

void AudioEngine::playByPath(std::string index) {
    FMOD_STUDIO_EVENTDESCRIPTION* descript;

    if (FMOD_Studio_System_GetEvent(m_system, index.c_str(), &descript) != FMOD_OK) {
        Logger::Error("Failed to retrieve event named: %s", index.c_str());
    }

    FMOD_STUDIO_EVENTINSTANCE* instance;
    if (FMOD_Studio_EventDescription_CreateInstance(descript, &instance) != FMOD_OK) {
        Logger::Error("Failed to create instance");
    }

    if (FMOD_Studio_EventInstance_Start(instance) != FMOD_OK) {
        Logger::Error("Failed to start instance");
    }

    // Do this only if you are not planning to reuse this instance again (ie playing) or will not be manipulating in game parameters
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