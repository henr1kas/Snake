#pragma once
#include <cstdint>
#include <cmath>
#include <SDL3/SDL.h>
#include "App.hpp"
#include "Utils.hpp"

class Game {
    App* app;

    enum eTileType : std::uint8_t {
        NOTHING,
        SNAKE,
        APPLE
    };

    /* 16x12 grid */
    eTileType tiles[192]{};

    /* TODO: texture enum ?*/
    SDL_Texture *lostTextTexture = nullptr, *restartTextTexture = nullptr, *wonTextTexture = nullptr;

    /* red */
    std::uint8_t appleColor[4]{255, 0, 0, 255};

    /* dark green */
    std::uint8_t snakeColor[4]{0, 100, 16, 255};

    /* white */
    std::uint8_t gridColor[4]{255, 255, 255, 255};

    /* .front() = tail, .back() = head */
    std::vector<std::uint8_t> snake;

    /* movement */
    std::uint64_t lastUpdateTick = 0;

    /* direction snake is moving. -1 = starting position */
    std::int8_t dir = -1;

  public:
    Game(App* pApp) : app(pApp) {}

    std::uint8_t ScreenToWorld(const float posX, const float posY) {
        return static_cast<std::uint8_t>(std::floor(posY / 50) * 16 + std::floor(posX / 50));
    }

    std::pair<float, float> WorldToScreen(const std::uint8_t tileX, const std::uint8_t tileY) {
        return {tileX * 50.f, tileY * 50.f};
    }

    std::pair<float, float> WorldToScreen(const std::uint8_t tile) {
        return WorldToScreen(tile % 16, tile / 16);
    }

    void InitGame() {
        for (std::uint8_t i = 0; i < 192; ++i) {
            tiles[i] = NOTHING;
        }
        tiles[utils::random(0, 191)] = APPLE;

        std::uint8_t snakeTile = static_cast<std::uint8_t>(utils::random(0, 191));
        while (tiles[snakeTile] != NOTHING) {
            snakeTile = static_cast<std::uint8_t>(utils::random(0, 191));
        }
        snake.clear();
        snake.emplace_back(snakeTile);
        tiles[snakeTile] = SNAKE;
    }

    void HandleInput(SDL_Event* ev) {
        switch (ev->key.keysym.sym) {
            case SDLK_LEFT:
            case SDLK_a: {
                if (dir != 1 && !snake.empty() && snake.size() != 192)
                    dir = 0;
                break;
            }
            case SDLK_RIGHT:
            case SDLK_d: {
                if (dir != 0 && !snake.empty() && snake.size() != 192)
                    dir = 1;
                break;
            }
            case SDLK_UP:
            case SDLK_w: {
                if (dir != 3 && !snake.empty() && snake.size() != 192)
                    dir = 2;
                break;
            }
            case SDLK_DOWN:
            case SDLK_s: {
                if (dir != 2 && !snake.empty() && snake.size() != 192)
                    dir = 3;
                break;
            }
            case SDLK_r: {
                if (dir == -1) {
                    InitGame();
                }
                break;
            }
        }
    }

    void Update(const std::uint64_t tick) {
        if (tick - lastUpdateTick >= 1e8) {
            lastUpdateTick = tick;
            if (dir == -1)
                return;
            constexpr std::uint8_t dx[4]{-1, 1, 0, 0};
            constexpr std::uint8_t dy[4]{0, 0, -1, 1};

            std::uint8_t tileY = snake.back() / 16, tileX = snake.back() - tileY * 16;
            tileX += dx[dir];
            tileY += dy[dir];

            const std::uint8_t tileIndex = tileY * 16 + tileX;

            if (tileX < 0 || tileX > 15 || tileY < 0 || tileY > 11) {
                snake.clear();
                return;
            }

            switch (tiles[tileIndex]) {
                case NOTHING: {
                    tiles[tileIndex] = SNAKE;
                    tiles[snake.front()] = NOTHING;
                    for (std::uint8_t i = 0; i < snake.size() - 1; ++i) {
                        snake[i] = snake[i + 1];
                    }
                    snake[snake.size() - 1] = tileIndex;
                    break;
                }
                case SNAKE: {
                    snake.clear();
                    break;
                }
                case APPLE: {
                    tiles[tileIndex] = SNAKE;
                    snake.emplace_back(tileIndex);

                    std::uint8_t newIndex = static_cast<std::uint8_t>(utils::random(0, 191));
                    while (tiles[newIndex] != NOTHING) {
                        newIndex = static_cast<std::uint8_t>(utils::random(0, 191));
                    }
                    tiles[newIndex] = APPLE;
                    break;
                }       
            }
        }
    }

    void Draw(const std::uint64_t tick) {
        if (tick != lastUpdateTick)
            return;
        SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
        SDL_RenderClear(app->renderer);

        switch (snake.size()) {
            case 0: {
                SDL_FRect destinationRect = {300, 275, 200, 50};
                SDL_RenderTexture(app->renderer, lostTextTexture, nullptr, &destinationRect);
                destinationRect = {300, 325, 200, 15};
                SDL_RenderTexture(app->renderer, restartTextTexture, nullptr, &destinationRect);
                dir = -1;
                break;
            }
            case 192: {
                SDL_FRect destinationRect = {300, 275, 200, 50};
                SDL_RenderTexture(app->renderer, wonTextTexture, nullptr, &destinationRect);
                destinationRect = {300, 325, 200, 15};
                SDL_RenderTexture(app->renderer, restartTextTexture, nullptr, &destinationRect);
                dir = -1;
                break;
            }
            default: {
                SDL_SetRenderDrawColor(app->renderer, appleColor[0], appleColor[1], appleColor[2], appleColor[3]);
                SDL_FRect appleRects[192];
                SDL_FRect snakeRects[192];
                SDL_FRect gridRects[192];
                std::uint8_t appleCount = 0;
                std::uint8_t snakeCount = 0;
                std::uint8_t gridCount = 0;

                for (std::uint8_t i = 0; i < 192; ++i) {
                    SDL_FRect position{(i % 16) * 50.f, (i / 16) * 50.f, 50.f, 50.f};

                    if (tiles[i] == APPLE) {
                        appleRects[appleCount++] = position;
                    }
                    gridRects[gridCount++] = position;
                }

                SDL_SetRenderDrawColor(app->renderer, appleColor[0], appleColor[1], appleColor[2], appleColor[3]);
                SDL_RenderFillRects(app->renderer, appleRects, appleCount);

                SDL_SetRenderDrawColor(app->renderer, gridColor[0], gridColor[1], gridColor[2], gridColor[3]);
                SDL_RenderRects(app->renderer, gridRects, gridCount);

                for (const std::uint8_t tile : snake) {
                    std::pair<float, float> pos = WorldToScreen(tile);
                    SDL_FRect position{pos.first, pos.second, 50.f, 50.f};
                    snakeRects[snakeCount++] = position;
                }

                SDL_SetRenderDrawColor(app->renderer, snakeColor[0], snakeColor[1], snakeColor[2], snakeColor[3]);
                SDL_RenderFillRects(app->renderer, snakeRects, snakeCount);
                break;
            }
        }

        SDL_RenderPresent(app->renderer);
    }

    bool InitTextures() {
        lostTextTexture = app->CreateTextTexture(0, "You lost!", 255, 0, 0);
        if (!lostTextTexture) {
            return false;
        }

        wonTextTexture = app->CreateTextTexture(0, "You won!", 0, 255, 0);
        if (!wonTextTexture) {
            return false;
        }

        restartTextTexture = app->CreateTextTexture(1, "Press R to restart", 255, 0, 0);
        if (!restartTextTexture) {
            return false;
        }

        return true;
    }

    bool Init() {
        if (!InitTextures())
            return false;
        InitGame();
        return true;
    }

    void Destroy() {
        SDL_DestroyTexture(restartTextTexture);
        SDL_DestroyTexture(lostTextTexture);
        SDL_DestroyTexture(wonTextTexture);
        app->Destroy();
    }
};