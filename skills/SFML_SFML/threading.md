# Threading


**SFML is not thread-safe by default - use mutex protection**
```cpp
#include <mutex>

class ThreadSafeTexture {
    sf::Texture texture;
    std::mutex mutex;
    
public:
    void loadFromFile(const std::string& filename) {
        std::lock_guard<std::mutex> lock(mutex);
        texture.loadFromFile(filename);
    }
    
    void draw(sf::RenderWindow& window, sf::Sprite& sprite) {
        std::lock_guard<std::mutex> lock(mutex);
        sprite.setTexture(texture);
        window.draw(sprite);
    }
};
```

**Use separate threads for audio and network operations**
```cpp
class AudioThread {
    std::thread thread;
    std::atomic<bool> running{true};
    sf::SoundBuffer buffer;
    sf::Sound sound;
    
public:
    void start() {
        buffer.loadFromFile("background.wav");
        sound.setBuffer(buffer);
        thread = std::thread([this]() {
            while (running) {
                sound.play();
                sf::sleep(sf::seconds(1));
            }
        });
    }
    
    void stop() {
        running = false;
        if (thread.joinable())
            thread.join();
    }
};
```

**Create SFML objects only in the main thread**
```cpp
// BAD: Creating SFML objects in worker thread
std::thread worker([]() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Thread Window"); // May crash
});

// GOOD: Create in main thread, use worker for data processing
class DataProcessor {
    std::vector<sf::Vertex> vertices;
public:
    void processInBackground() {
        std::thread worker([this]() {
            // Process data without SFML objects
            for (int i = 0; i < 1000; ++i) {
                vertices.push_back(sf::Vertex(sf::Vector2f(i, i)));
            }
        });
        worker.detach();
    }
    
    void render(sf::RenderWindow& window) {
        sf::VertexArray va(sf::Points, vertices.size());
        for (size_t i = 0; i < vertices.size(); ++i)
            va[i] = vertices[i];
        window.draw(va); // Draw in main thread
    }
};
```

**Use atomic operations for shared state between threads**
```cpp
class GameState {
    std::atomic<bool> gameOver{false};
    std::atomic<int> score{0};
    
public:
    void update() {
        // Safe to call from any thread
        score.fetch_add(10);
    }
    
    bool isGameOver() const {
        return gameOver.load();
    }
    
    void endGame() {
        gameOver.store(true);
    }
};
```