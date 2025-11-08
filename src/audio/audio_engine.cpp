#include "audio_engine.hpp"

AudioEngine* AudioEngine::m_instance = nullptr;

bool AudioEngine::init() {
    FMOD_RESULT result;

    result = FMOD_Studio_System_Create(&m_studioSystem, FMOD_VERSION);
    if (result != FMOD_OK) {
        Logger::Error("FMOD::ERROR::(%d)", result);
        return false;
    }

    result = FMOD_Studio_System_Initialize(m_studioSystem, 512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
    if (result != FMOD_OK) {
        Logger::Error("FMOD::ERROR::(%d)", result);
        return false;
    }

    return true;
}

void AudioEngine::loadSound(const char* filename) {
    FMOD_RESULT result;
    result = FMOD_Studio_System_LoadBankFile(m_studioSystem, filename, FMOD_STUDIO_LOAD_BANK_NORMAL, &m_banks[0]);

    if (result != FMOD_OK) {
        Logger::Error("Error loading bank: %s", filename);
    }

    FMOD_STUDIO_EVENTDESCRIPTION* descript[10];
    int numberLoaded = 0;
    FMOD_Studio_Bank_GetEventList(m_banks[0], descript, 10, &numberLoaded);
    Logger::Warn("Number of events in given file: %d", numberLoaded);

    // if(FMOD_Studio_System_GetEvent(m_studioSystem, 0, &descript[0]) != FMOD_OK) {
    //     Logger::Error("Failed get Event");
    // }

    FMOD_STUDIO_EVENTINSTANCE* instance;
    if(FMOD_Studio_EventDescription_CreateInstance(descript[0], &instance) != FMOD_OK) {
        Logger::Error("Failed to create instance");
    }

    if(FMOD_Studio_EventInstance_Start(instance) != FMOD_OK) {
        Logger::Error("Failed to start instance");
    }
}
void AudioEngine::update() {
    if (FMOD_Studio_System_Update(m_studioSystem) != FMOD_OK) {
        Logger::Error("Failed to update sound");
    }
}

void AudioEngine::clean() {
    FMOD_Studio_System_Release(m_studioSystem);
}