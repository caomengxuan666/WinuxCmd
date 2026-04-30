# Pitfalls


**BAD: Not checking if texture loaded successfully**
```cpp
sf::Texture texture;
texture.loadFromFile("image.png"); // No error checking
sf::Sprite sprite(texture);
// If loading fails, sprite will be invalid
```

**GOOD: Always check resource loading**
```cpp
sf::Texture texture;
if (!texture.loadFromFile("image.png"))
{
    // Handle error appropriately
    std::cerr << "Failed to load texture" << std::endl;
    return -1;
}
sf::Sprite sprite(texture);
```

**BAD: Using texture after it's destroyed**
```cpp
sf::Sprite createSprite()
{
    sf::Texture texture;
    texture.loadFromFile("image.png");
    return sf::Sprite(texture); // Texture destroyed when function returns
}
```

**GOOD: Keep texture alive while sprite exists**
```cpp
class GameSprite {
    sf::Texture texture;
    sf::Sprite sprite;
public:
    GameSprite(const std::string& filename) {
        texture.loadFromFile(filename);
        sprite.setTexture(texture);
    }
};
```

**BAD: Not handling window events properly**
```cpp
while (window.isOpen())
{
    // Missing event polling - window becomes unresponsive
    window.clear();
    window.draw(something);
    window.display();
}
```

**GOOD: Always poll events in main loop**
```cpp
while (window.isOpen())
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();
    }
    window.clear();
    window.draw(something);
    window.display();
}
```

**BAD: Using special characters in file paths**
```cpp
sf::Image image;
image.saveToFile("C:\\Users\\Sébastien\\Desktop\\image.png"); // Fails with special chars
```

**GOOD: Use wide strings for paths with special characters**
```cpp
sf::Image image;
// Convert to wide string for paths with special characters
std::wstring widePath = L"C:\\Users\\Sébastien\\Desktop\\image.png";
image.saveToFile(widePath); // Assuming overload exists
```

**BAD: Creating too many textures at startup**
```cpp
std::vector<sf::Texture> textures;
for (int i = 0; i < 1000; ++i)
{
    sf::Texture tex;
    tex.loadFromFile("texture" + std::to_string(i) + ".png");
    textures.push_back(tex);
}
```

**GOOD: Load textures on demand and reuse**
```cpp
class TextureManager {
    std::unordered_map<std::string, sf::Texture> textures;
public:
    sf::Texture& get(const std::string& filename) {
        auto it = textures.find(filename);
        if (it == textures.end())
        {
            sf::Texture tex;
            tex.loadFromFile(filename);
            textures[filename] = std::move(tex);
        }
        return textures[filename];
    }
};
```