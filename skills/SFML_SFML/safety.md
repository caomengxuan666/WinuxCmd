# Safety


**RED LINE 1: Never use a texture after its associated sprite is destroyed without copying the texture data**
```cpp
// BAD: Sprite references destroyed texture
sf::Sprite* createBadSprite() {
    sf::Texture* tex = new sf::Texture();
    tex->loadFromFile("image.png");
    sf::Sprite* sprite = new sf::Sprite(*tex);
    delete tex; // Texture destroyed, sprite now references invalid data
    return sprite;
}

// GOOD: Texture outlives sprite
sf::Sprite* createGoodSprite() {
    sf::Texture* tex = new sf::Texture();
    tex->loadFromFile("image.png");
    sf::Sprite* sprite = new sf::Sprite(*tex);
    // Texture will be managed separately
    return sprite;
}
```

**RED LINE 2: Never call OpenGL functions directly without proper context management**
```cpp
// BAD: Direct OpenGL call without context
glClear(GL_COLOR_BUFFER_BIT); // No active OpenGL context

// GOOD: Use SFML's OpenGL context
sf::RenderWindow window(sf::VideoMode(800, 600), "OpenGL");
window.setActive(true); // Activate OpenGL context
glClear(GL_COLOR_BUFFER_BIT);
```

**RED LINE 3: Never modify a texture while it's being used by a sprite in a different thread**
```cpp
// BAD: Thread safety violation
sf::Texture texture;
sf::Sprite sprite(texture);

std::thread t([&texture]() {
    texture.loadFromFile("new_image.png"); // Modifying while sprite uses it
});

// GOOD: Load texture before creating sprite
sf::Texture texture;
texture.loadFromFile("image.png");
sf::Sprite sprite(texture);
```

**RED LINE 4: Never use sf::Sound after its sf::SoundBuffer is destroyed**
```cpp
// BAD: Sound buffer destroyed before sound
sf::Sound* createBadSound() {
    sf::SoundBuffer buffer;
    buffer.loadFromFile("sound.wav");
    return new sf::Sound(buffer); // Buffer destroyed when function returns
}

// GOOD: Keep buffer alive
class SoundPlayer {
    sf::SoundBuffer buffer;
    sf::Sound sound;
public:
    SoundPlayer(const std::string& filename) {
        buffer.loadFromFile(filename);
        sound.setBuffer(buffer);
    }
};
```

**RED LINE 5: Never call window.create() while the window is still active**
```cpp
// BAD: Creating window while another is active
sf::RenderWindow window1(sf::VideoMode(800, 600), "Window 1");
sf::RenderWindow window2(sf::VideoMode(800, 600), "Window 2"); // May cause issues

// GOOD: Close first window before creating second
sf::RenderWindow window1(sf::VideoMode(800, 600), "Window 1");
window1.close();
sf::RenderWindow window2(sf::VideoMode(800, 600), "Window 2");
```