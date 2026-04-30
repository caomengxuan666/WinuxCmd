# Performance

**Texture Creation and Management**

```cpp
// BAD: Creating textures every frame
void renderFrame() {
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_RenderTexture(renderer, tex, NULL, NULL);
    SDL_DestroyTexture(tex); // Expensive: GPU upload every frame
}

// GOOD: Create textures once, reuse them
SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
// ... later in render loop ...
SDL_RenderTexture(renderer, texture, NULL, NULL);
// Destroy only when done
```

**Batch Rendering**

```cpp
// BAD: Many individual draw calls
for (auto& sprite : sprites) {
    SDL_RenderTexture(renderer, sprite.texture, &sprite.src, &sprite.dst);
}

// GOOD: Batch by texture
auto byTexture = std::unordered_map<SDL_Texture*, std::vector<Sprite>>();
for (auto& sprite : sprites) {
    byTexture[sprite.texture].push_back(sprite);
}
for (auto& [tex, batch] : byTexture) {
    for (auto& sprite : batch) {
        SDL_RenderTexture(renderer, tex, &sprite.src, &sprite.dst);
    }
}
```

**Pixel Format and Surface Operations**

```cpp
// BAD: Slow pixel access with SDL_GetRGBA
for (int y = 0; y < surface->h; y++) {
    for (int x = 0; x < surface->w; x++) {
        Uint32 pixel = ((Uint32*)surface->pixels)[y * surface->w + x];
        Uint8 r, g, b, a;
        SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);
    }
}

// GOOD: Direct pixel access with known format
SDL_Surface* converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888);
Uint32* pixels = (Uint32*)converted->pixels;
for (int i = 0; i < converted->w * converted->h; i++) {
    Uint32 pixel = pixels[i];
    Uint8 r = (pixel >> 24) & 0xFF;
    Uint8 g = (pixel >> 16) & 0xFF;
    Uint8 b = (pixel >> 8) & 0xFF;
    Uint8 a = pixel & 0xFF;
}
```

**Render Target Optimization**

```cpp
// BAD: Creating/destroying render targets frequently
for (int i = 0; i < 100; i++) {
    SDL_Texture* target = SDL_CreateTexture(renderer, format, 
                                            SDL_TEXTUREACCESS_TARGET, 100, 100);
    SDL_SetRenderTarget(renderer, target);
    // ... render to target ...
    SDL_SetRenderTarget(renderer, NULL);
    SDL_DestroyTexture(target);
}

// GOOD: Reuse render targets
SDL_Texture* target = SDL_CreateTexture(renderer, format, 
                                        SDL_TEXTUREACCESS_TARGET, 100, 100);
for (int i = 0; i < 100; i++) {
    SDL_SetRenderTarget(renderer, target);
    // ... render to target ...
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderTexture(renderer, target, NULL, &destRect);
}
SDL_DestroyTexture(target);
```