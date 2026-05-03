# Pitfalls

**Pitfall 1: Forgetting to call SDL_Quit()**

BAD:
```cpp
int main() {
    SDL_Init(SDL_INIT_VIDEO);
    // ... use SDL ...
    return 0; // SDL_Quit() never called
}
```

GOOD:
```cpp
int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Init failed");
        return 1;
    }
    // ... use SDL ...
    SDL_Quit();
    return 0;
}
```

**Pitfall 2: Ignoring return values from SDL functions**

BAD:
```cpp
SDL_Window* window = SDL_CreateWindow("Test", 800, 600, 0);
// window might be NULL, but we use it anyway
SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
```

GOOD:
```cpp
SDL_Window* window = SDL_CreateWindow("Test", 800, 600, 0);
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

**Pitfall 3: Using SDL functions from multiple threads without synchronization**

BAD:
```cpp
std::thread t1([&]() { SDL_RenderClear(renderer); });
std::thread t2([&]() { SDL_RenderPresent(renderer); });
t1.join(); t2.join(); // Race condition!
```

GOOD:
```cpp
// All rendering on main thread
while (running) {
    SDL_RenderClear(renderer);
    // ... draw ...
    SDL_RenderPresent(renderer);
}
```

**Pitfall 4: Memory leaks from surfaces and textures**

BAD:
```cpp
SDL_Surface* surface = SDL_LoadBMP("image.bmp");
SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
// surface is never freed!
```

GOOD:
```cpp
SDL_Surface* surface = SDL_LoadBMP("image.bmp");
if (!surface) { /* error */ }
SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
SDL_DestroySurface(surface); // Free surface immediately
if (!texture) { /* error */ }
```

**Pitfall 5: Not checking SDL_GetError() after failures**

BAD:
```cpp
if (!SDL_Init(SDL_INIT_VIDEO)) {
    // No error message printed
    return 1;
}
```

GOOD:
```cpp
if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("SDL_Init failed: %s", SDL_GetError());
    return 1;
}
```

**Pitfall 6: Using SDL_Delay() in the event loop (blocks input)**

BAD:
```cpp
while (running) {
    SDL_Delay(16); // Blocks event processing!
    SDL_PollEvent(&event);
    // ...
}
```

GOOD:
```cpp
while (running) {
    while (SDL_PollEvent(&event)) {
        // Process events immediately
    }
    // Use SDL_GetTicks() for timing, not SDL_Delay()
    Uint64 frameTime = SDL_GetTicks() - lastFrame;
    lastFrame = SDL_GetTicks();
    // Update and render
}
```