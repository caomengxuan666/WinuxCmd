# Quickstart


```cpp
// 1. Creating a window and handling events
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Window");
    
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        window.clear(sf::Color::Black);
        // Draw here
        window.display();
    }
    return 0;
}
```

```cpp
// 2. Drawing shapes and sprites
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Drawing Example");
    
    sf::CircleShape circle(50.0f);
    circle.setFillColor(sf::Color::Green);
    circle.setPosition(100, 100);
    
    sf::RectangleShape rect(sf::Vector2f(200, 100));
    rect.setFillColor(sf::Color::Blue);
    rect.setPosition(300, 200);
    
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        window.clear();
        window.draw(circle);
        window.draw(rect);
        window.display();
    }
    return 0;
}
```

```cpp
// 3. Loading and displaying textures
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Texture Example");
    
    sf::Texture texture;
    if (!texture.loadFromFile("image.png"))
        return -1;
    
    sf::Sprite sprite(texture);
    sprite.setPosition(100, 100);
    
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        window.clear();
        window.draw(sprite);
        window.display();
    }
    return 0;
}
```

```cpp
// 4. Playing audio
#include <SFML/Audio.hpp>

int main()
{
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("sound.wav"))
        return -1;
    
    sf::Sound sound(buffer);
    sound.play();
    
    sf::sleep(sf::seconds(5.0f));
    return 0;
}
```

```cpp
// 5. Handling keyboard input
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Input Example");
    sf::CircleShape player(20.0f);
    player.setFillColor(sf::Color::Red);
    player.setPosition(400, 300);
    
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            player.move(-5, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            player.move(5, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            player.move(0, -5);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            player.move(0, 5);
        
        window.clear();
        window.draw(player);
        window.display();
    }
    return 0;
}
```

```cpp
// 6. Using text rendering
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Text Example");
    
    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
        return -1;
    
    sf::Text text("Hello SFML!", font, 30);
    text.setFillColor(sf::Color::White);
    text.setPosition(100, 100);
    
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        window.clear();
        window.draw(text);
        window.display();
    }
    return 0;
}
```