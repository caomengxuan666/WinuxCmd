# Best Practices


**Use a game loop with fixed timestep**
```cpp
class Game {
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    const sf::Time timePerFrame = sf::seconds(1.0f / 60.0f);
    
public:
    void run() {
        while (window.isOpen()) {
            processEvents();
            timeSinceLastUpdate += clock.restart();
            while (timeSinceLastUpdate > timePerFrame) {
                timeSinceLastUpdate -= timePerFrame;
                update(timePerFrame);
            }
            render();
        }
    }
};
```

**Implement resource caching for textures and fonts**
```cpp
class ResourceManager {
    std::unordered_map<std::string, std::shared_ptr<sf::Texture>> textures;
    std::unordered_map<std::string, std::shared_ptr<sf::Font>> fonts;
    
public:
    std::shared_ptr<sf::Texture> loadTexture(const std::string& path) {
        auto it = textures.find(path);
        if (it == textures.end()) {
            auto tex = std::make_shared<sf::Texture>();
            if (tex->loadFromFile(path))
                textures[path] = tex;
            else
                return nullptr;
        }
        return textures[path];
    }
};
```

**Use vertex arrays for efficient rendering**
```cpp
// Efficient batch rendering
sf::VertexArray vertices(sf::Quads, 4);
vertices[0].position = sf::Vector2f(0, 0);
vertices[1].position = sf::Vector2f(100, 0);
vertices[2].position = sf::Vector2f(100, 100);
vertices[3].position = sf::Vector2f(0, 100);

window.draw(vertices); // Single draw call instead of multiple
```

**Implement proper error handling for all file operations**
```cpp
bool loadResources() {
    try {
        if (!texture.loadFromFile("texture.png"))
            throw std::runtime_error("Failed to load texture");
        if (!font.loadFromFile("font.ttf"))
            throw std::runtime_error("Failed to load font");
        if (!buffer.loadFromFile("sound.wav"))
            throw std::runtime_error("Failed to load sound");
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Resource loading failed: " << e.what() << std::endl;
        return false;
    }
}
```