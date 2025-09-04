#include "engine/engine.hpp"

int main(int argc, char* argv[]) {
    printf("Starting engine\n");
    if (!Engine::getInstance()->init(argc, argv)) {
        printf("Failed to init engine\n");
        return -1;
    }

    while (Engine::getInstance()->isRunning()) {
        Engine::getInstance()->event();
        Engine::getInstance()->update();
        Engine::getInstance()->render();
    }

    printf("Exiting\n");
    Engine::getInstance()->clean();
    return 0;
}