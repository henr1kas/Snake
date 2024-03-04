#include <cmath>
#include "Game.hpp"

int main() {
    Game game;

    if (!game.Init()) {
        return 1;
    }

    App* app = game.GetApp();

    bool running = true;

    while (running) {
        app->frameStartTime = SDL_GetTicksNS();

        SDL_Event ev;
        while (SDL_PollEvent(&ev) != 0) {
            switch (ev.type) {
                case SDL_EVENT_QUIT: {
                    running = false;
                    break;
                }
                case SDL_EVENT_KEY_DOWN: {
                    game.HandleInput(&ev);
                    break;
                }
            }
        }

        std::uint64_t ticks = SDL_GetTicksNS();

        game.Handle(ticks);
        game.Draw(ticks);

        app->frameEndTime = SDL_GetTicksNS();
        app->frameTime = app->frameEndTime - app->frameStartTime;

        if (app->frameTime < app->minFrameTime) {
            //std::cout << "fps: " << (1.0 / (app->minFrameTime - app->frameTime)) * 1e9 << '\n';
            SDL_DelayNS(static_cast<Uint64>((app->minFrameTime - app->frameTime)));
        }
    }

    game.Destroy();
    return 0;
}