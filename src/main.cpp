#define STB_IMAGE_IMPLEMENTATION
#include "engine/engine.hpp"

int main(int argc, char* argv[]) {
    printf("Starting engine\n");
    if (!Engine::getInstance()->init(argc, argv)) {
        Logger::Error("Failed to init engine");
        return -1;
    }

    while (Engine::getInstance()->isRunning()) {
        Engine::getInstance()->event();
        Engine::getInstance()->update();
        Engine::getInstance()->render();
    }

    Logger::Log("Exiting");
    Engine::getInstance()->clean();
    return 0;
}