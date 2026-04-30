# Quickstart

```cpp
// 1. Initialize SDL
#include <SDL3/SDL.h>

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }
    // ... use SDL ...
    SDL_Quit();
    return 0;
}
```

```cpp
// 2. Create a window and renderer
SDL_Window* window = SDL_CreateWindow("My App", 800, 600, 0);
if (!window) {
    SDL_Log("Window creation failed: %s", SDL_GetError());
    return 1;
}

SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
if (!renderer) {
    SDL_Log("Renderer creation failed: %s", SDL_GetError());
    SDL_DestroyWindow(window);
    return 1;
}
```

```cpp
// 3. Event loop
SDL_Event event;
bool running = true;

while (running) {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            running = false;
        }
        if (event.type == SDL_EVENT_KEY_DOWN) {
            if (event.key.key == SDLK_ESCAPE) {
                running = false;
            }
        }
    }
    // Update game logic
    // Render
    SDL_RenderPresent(renderer);
}
```

```cpp
// 4. Load and render a texture
SDL_Surface* surface = SDL_LoadBMP("image.bmp");
if (!surface) {
    SDL_Log("Failed to load image: %s", SDL_GetError());
    return 1;
}

SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
SDL_DestroySurface(surface);

if (!texture) {
    SDL_Log("Failed to create texture: %s", SDL_GetError());
    return 1;
}

SDL_RenderTexture(renderer, texture, NULL, NULL);
```

```cpp
// 5. Audio playback
SDL_AudioSpec spec;
spec.freq = 44100;
spec.format = SDL_AUDIO_F32;
spec.channels = 2;

SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
if (!stream) {
    SDL_Log("Audio open failed: %s", SDL_GetError());
    return 1;
}

SDL_ResumeAudioStreamDevice(stream);
// Feed audio data: SDL_PutAudioStreamData(stream, data, len);
```

```cpp
// 6. Timer and frame rate control
Uint64 start = SDL_GetTicks();
// ... do work ...
Uint64 elapsed = SDL_GetTicks() - start;

// Fixed timestep
const Uint64 frameDelay = 16; // ~60 FPS
Uint64 frameStart = SDL_GetTicks();
// ... render ...
Uint64 frameTime = SDL_GetTicks() - frameStart;
if (frameDelay > frameTime) {
    SDL_Delay(frameDelay - frameTime);
}
```