# Threading

**Thread Safety Guarantees**

```cpp
// SDL's thread safety rules:
// 1. Most SDL functions are NOT thread-safe
// 2. Only call SDL functions from the main thread (where SDL_Init was called)
// 3. Exception: SDL_Atomic*, SDL_Mutex*, SDL_Semaphore*, SDL_Thread* are thread-safe

// BAD: Rendering from worker thread
std::thread worker([&]() {
    SDL_RenderClear(renderer); // UNDEFINED BEHAVIOR
});

// GOOD: All rendering on main thread
std::thread worker([&]() {
    // Do computation, then signal main thread
    SDL_AtomicIncRef(&workDone);
});
```

**Thread-Safe Data Transfer**

```cpp
// Safe pattern: Use atomic flags or mutexes for data sharing
std::atomic<bool> newDataReady{false};
std::vector<Vertex> sharedData;
SDL_Mutex* dataMutex = SDL_CreateMutex();

// Worker thread
std::thread worker([&]() {
    std::vector<Vertex> localData = computeVertices();
    SDL_LockMutex(dataMutex);
    sharedData = std::move(localData);
    SDL_UnlockMutex(dataMutex);
    newDataReady.store(true);
});

// Main thread (rendering)
while (running) {
    if (newDataReady.load()) {
        SDL_LockMutex(dataMutex);
        renderVertices(sharedData);
        SDL_UnlockMutex(dataMutex);
        newDataReady.store(false);
    }
    SDL_RenderPresent(renderer);
}
```

**Audio Callback Thread Safety**

```cpp
// Audio callbacks run on a separate thread
// Only use atomic operations and lock-free data structures

std::atomic<double> frequency{440.0};
std::atomic<float> volume{0.5f};

void audioCallback(void* userdata, SDL_AudioStream* stream, int additional, int total) {
    // This runs on audio thread - be careful!
    float freq = frequency.load();
    float vol = volume.load();
    
    // Generate audio samples
    float samples[256];
    for (int i = 0; i < 256; i++) {
        samples[i] = vol * sin(2 * M_PI * freq * i / 44100.0);
    }
    SDL_PutAudioStreamData(stream, samples, sizeof(samples));
}

// Main thread can safely update these atomics
void setFrequency(double freq) {
    frequency.store(freq); // Atomic, safe from any thread
}
```

**Event Queue Thread Safety**

```cpp
// SDL_PushEvent is thread-safe
// SDL_PollEvent should only be called from main thread

// Worker thread can push custom events
std::thread worker([&]() {
    SDL_Event event;
    event.type = SDL_EVENT_USER;
    event.user.code = 1;
    event.user.data1 = someData;
    SDL_PushEvent(&event); // Thread-safe
});

// Main thread processes events
SDL_Event event;
while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_USER) {
        // Handle user event
        void* data = event.user.data1;
    }
}
```