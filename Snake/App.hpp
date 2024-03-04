#pragma once
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

class App {
  public:
    /* move to fonts? */
    int width = 0, height = 0;
    TTF_Font* fonts[2]{};
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    std::uint32_t minFrameTime = 0;
    Uint64 frameStartTime = 0, frameEndTime = 0, frameTime = 0;

    void Destroy() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_CloseFont(fonts[0]);
        TTF_CloseFont(fonts[1]);
        TTF_Quit();
        SDL_Quit();
    }

    bool Init(const std::string& name, int w, int h, Uint32 flags = 0) {
        width = w;
        height = h;

        if (SDL_Init(SDL_INIT_VIDEO)) {
            std::cerr << "Failed to initialize SDL: " << SDL_GetError() << '\n';
            return false;
        }

        if (TTF_Init()) {
            std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError() << '\n';
            SDL_Quit();
            return false;
        }

        fonts[0] = TTF_OpenFont("C:\\Windows\\Fonts\\Arial.ttf", 24);
        if (!fonts[0]) {
            std::cerr << "Failed to load font Arial.ttf: " << TTF_GetError() << '\n';
            TTF_Quit();
            SDL_Quit();
            return false;
        }

        fonts[1] = TTF_OpenFont("C:\\Windows\\Fonts\\Arial.ttf", 12);
        if (!fonts[1]) {
            std::cerr << "Failed to load font Arial.ttf: " << TTF_GetError() << '\n';
            TTF_Quit();
            SDL_Quit();
            return false;
        }

        window = SDL_CreateWindow(name.data(), w, h, flags);
        if (!window) {
            std::cerr << "Failed to create window: " << SDL_GetError() << '\n';
            TTF_CloseFont(fonts[0]);
            TTF_CloseFont(fonts[1]);
            TTF_Quit();
            SDL_Quit();
            return false;
        }

        renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            std::cerr << "Failed to create renderer: " << SDL_GetError() << '\n';
            SDL_DestroyWindow(window);
            TTF_CloseFont(fonts[1]);
            TTF_CloseFont(fonts[0]);
            TTF_Quit();
            SDL_Quit();
            return false;
        }
        return true;
    }
    SDL_Texture* CreateTextTexture(const std::uint8_t fontIndex, const std::string& text, std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a = 255) {
        SDL_Surface* surface = TTF_RenderText_Solid(fonts[fontIndex], text.data(), {r, g, b, a});
        if (!surface) {
            std::cerr << "Failed to create surface for text rendering: " << SDL_GetError() << '\n';
            Destroy();
            return nullptr;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);

        if (!texture) {
            std::cerr << "Failed to create texture from surface: " << SDL_GetError() << '\n';
            Destroy();
        }

        return texture;
    }

    void SetFPSLimit(const double fps) {
        if (fps <= 1.0 || fps >= 4999.9) {
            minFrameTime = 200000;
        } else {
            minFrameTime = static_cast<std::uint32_t>(1000000000.0 / fps);
        }
    }
};