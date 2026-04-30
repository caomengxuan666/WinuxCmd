# Lifecycle


**Construction and initialization**
```cpp
// Proper construction with error handling
sf::RenderWindow window;
window.create(sf::VideoMode(800, 600), "My Window", sf::Style::Default);

// Or use constructor directly
sf::RenderWindow window(sf::VideoMode(800, 600), "My Window");

// Resource construction
sf::Texture texture;
if (!texture.loadFromFile("image.png"))
    throw std::runtime_error("Failed to load texture");
```

**Move semantics for efficient resource transfer**
```cpp
// Move construction
sf::Texture texture1;
texture1.loadFromFile("image1.png");
sf::Texture texture2 = std::move(texture1); // Efficient transfer

// Move assignment
sf::Texture texture3;
texture3 = std::move(texture2);

// Using in containers
std::vector<sf::Texture> textures;
textures.push_back(std::move(texture3)); // No copy
```

**Resource management with RAII**
```cpp
class GameWindow {
    sf::RenderWindow window;
    sf::Texture texture;
    sf::Sprite sprite;
    
public:
    GameWindow() : window(sf::VideoMode(800, 600), "Game") {
        texture.loadFromFile("background.png");
        sprite.setTexture(texture);
    }
    
    // Destructor automatically cleans up
    ~GameWindow() {
        window.close(); // Explicit close, though destructor handles it
    }
    
    // Move constructor
    GameWindow(GameWindow&& other) noexcept 
        : window(std::move(other.window))
        , texture(std::move(other.texture))
        , sprite(std::move(other.sprite)) {}
};
```

**Proper destruction sequence**
```cpp
class AudioManager {
    sf::SoundBuffer buffer;
    sf::Sound sound;
    
public:
    AudioManager(const std::string& filename) {
        buffer.loadFromFile(filename);
        sound.setBuffer(buffer);
    }
    
    ~AudioManager() {
        sound.stop(); // Stop playback before destruction
        // buffer and sound are automatically destroyed
    }
    
    // Prevent copying
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;
};
```