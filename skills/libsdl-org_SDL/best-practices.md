# Best Practices

**RAII Wrapper Pattern**

```cpp
class SDLWindow {
    SDL_Window* window;
public:
    SDLWindow(const char* title, int w, int h) 
        : window(SDL_CreateWindow(title, w, h, 0)) {
        if (!window) throw std::runtime_error(SDL_GetError());
    }
    ~SDLWindow() { if (window) SDL_DestroyWindow(window); }
    SDLWindow(const SDLWindow&) = delete;
    SDLWindow& operator=(const SDLWindow&) = delete;
    SDLWindow(SDLWindow&& other) noexcept : window(other.window) {
        other.window = nullptr;
    }
    SDL_Window* get() const { return window; }
};
```

**Error Handling with Logging**

```cpp
#define SDL_CHECK(expr) do { \
    if (!(expr)) { \
        SDL_Log("SDL Error at %s:%d: %s", __FILE__, __LINE__, SDL_GetError()); \
        return false; \
    } \
} while(0)

bool init() {
    SDL_CHECK(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO));
    SDL_CHECK(window = SDL_CreateWindow("App", 800, 600, 0));
    SDL_CHECK(renderer = SDL_CreateRenderer(window, NULL));
    return true;
}
```

**Frame Rate Independent Movement**

```cpp
class Game {
    Uint64 lastTime = SDL_GetTicks();
    float accumulator = 0.0f;
    const float fixedDelta = 1.0f / 60.0f;
    
    void update() {
        Uint64 now = SDL_GetTicks();
        float delta = (now - lastTime) / 1000.0f;
        lastTime = now;
        
        accumulator += delta;
        while (accumulator >= fixedDelta) {
            fixedUpdate(fixedDelta); // Physics at fixed rate
            accumulator -= fixedDelta;
        }
    }
};
```

**Texture Atlas for Performance**

```cpp
// Load all sprites into one texture
SDL_Surface* atlas = SDL_LoadBMP("spritesheet.bmp");
SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, atlas);
SDL_DestroySurface(atlas);

// Define source rectangles
SDL_FRect srcRects[] = {
    {0, 0, 32, 32},   // Player
    {32, 0, 32, 32},  // Enemy
    {64, 0, 32, 32}   // Bullet
};

// Render using source rects
SDL_RenderTexture(renderer, texture, &srcRects[0], &destRect);
```