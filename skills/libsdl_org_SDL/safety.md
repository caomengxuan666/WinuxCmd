# Safety

**Red Line 1: Never call SDL functions after SDL_Quit()**

BAD:
```cpp
SDL_Init(SDL_INIT_VIDEO);
SDL_Window* w = SDL_CreateWindow("", 100, 100, 0);
SDL_Quit();
SDL_DestroyWindow(w); // CRASH: SDL already shut down
```

GOOD:
```cpp
SDL_Init(SDL_INIT_VIDEO);
SDL_Window* w = SDL_CreateWindow("", 100, 100, 0);
SDL_DestroyWindow(w); // Destroy before quit
SDL_Quit();
```

**Red Line 2: Never pass NULL to SDL functions that expect valid pointers**

BAD:
```cpp
SDL_Renderer* r = NULL;
SDL_RenderClear(r); // CRASH: NULL pointer dereference
```

GOOD:
```cpp
SDL_Renderer* r = SDL_CreateRenderer(window, NULL);
if (r) {
    SDL_RenderClear(r);
}
```

**Red Line 3: Never modify SDL_Surface pixels while it's being used by the renderer**

BAD:
```cpp
SDL_Surface* s = SDL_LoadBMP("image.bmp");
SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
// s is still referenced by t internally
SDL_LockSurface(s);
// Modify pixels - UNDEFINED BEHAVIOR
SDL_UnlockSurface(s);
```

GOOD:
```cpp
SDL_Surface* s = SDL_LoadBMP("image.bmp");
SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
SDL_DestroySurface(s); // Safe: texture has its own copy
// Now modify a new surface if needed
```

**Red Line 4: Never use SDL objects after they've been destroyed**

BAD:
```cpp
SDL_Window* w = SDL_CreateWindow("", 100, 100, 0);
SDL_DestroyWindow(w);
SDL_SetWindowTitle(w, "New Title"); // CRASH: use after free
```

GOOD:
```cpp
SDL_Window* w = SDL_CreateWindow("", 100, 100, 0);
SDL_SetWindowTitle(w, "New Title"); // Use before destroy
SDL_DestroyWindow(w);
w = NULL; // Prevent accidental reuse
```

**Red Line 5: Never call SDL_Init() multiple times without matching SDL_Quit() calls**

BAD:
```cpp
SDL_Init(SDL_INIT_VIDEO);
SDL_Init(SDL_INIT_AUDIO); // Second init without first quit
// ...
SDL_Quit(); // Only quits once, resources leaked
```

GOOD:
```cpp
SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO); // Single init with flags
// ...
SDL_Quit();
```