# Lifecycle

**Construction and Initialization**

```cpp
// Always initialize before creating SDL objects
if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
    // Handle error
}

// Create resources in order: window -> renderer -> textures
SDL_Window* window = SDL_CreateWindow("Game", 800, 600, SDL_WINDOW_RESIZABLE);
SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, 
                                          SDL_TEXTUREACCESS_STATIC, 800, 600);
```

**Destruction Order**

```cpp
// Destroy in reverse order of creation
void cleanup() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit(); // Last: shutdown SDL
}
```

**Move Semantics (C++ Wrapper)**

```cpp
class SDLTexture {
    SDL_Texture* tex;
public:
    SDLTexture(SDL_Renderer* r, int w, int h) 
        : tex(SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, 
                                SDL_TEXTUREACCESS_STATIC, w, h)) {}
    
    ~SDLTexture() { if (tex) SDL_DestroyTexture(tex); }
    
    // Move constructor
    SDLTexture(SDLTexture&& other) noexcept : tex(other.tex) {
        other.tex = nullptr;
    }
    
    // Move assignment
    SDLTexture& operator=(SDLTexture&& other) noexcept {
        if (this != &other) {
            if (tex) SDL_DestroyTexture(tex);
            tex = other.tex;
            other.tex = nullptr;
        }
        return *this;
    }
    
    // No copy allowed
    SDLTexture(const SDLTexture&) = delete;
    SDLTexture& operator=(const SDLTexture&) = delete;
};
```

**Resource Management with Smart Pointers**

```cpp
// Custom deleter for SDL objects
struct SDLWindowDeleter {
    void operator()(SDL_Window* w) { if (w) SDL_DestroyWindow(w); }
};
struct SDLRendererDeleter {
    void operator()(SDL_Renderer* r) { if (r) SDL_DestroyRenderer(r); }
};

using UniqueWindow = std::unique_ptr<SDL_Window, SDLWindowDeleter>;
using UniqueRenderer = std::unique_ptr<SDL_Renderer, SDLRendererDeleter>;

UniqueWindow window(SDL_CreateWindow("App", 800, 600, 0));
UniqueRenderer renderer(SDL_CreateRenderer(window.get(), NULL));
// Automatic cleanup when unique_ptrs go out of scope
```