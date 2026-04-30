# Performance


**Minimize texture state changes**
```cpp
// BAD: Frequent texture changes
for (auto& entity : entities) {
    window.draw(entity.getSprite()); // Each draw may change texture
}

// GOOD: Batch by texture
std::unordered_map<const sf::Texture*, std::vector<sf::Sprite>> batches;
for (auto& entity : entities) {
    batches[&entity.getTexture()].push_back(entity.getSprite());
}
for (auto& [texture, sprites] : batches) {
    for (auto& sprite : sprites) {
        window.draw(sprite);
    }
}
```

**Use vertex arrays for static geometry**
```cpp
// BAD: Individual draw calls
for (int i = 0; i < 1000; ++i) {
    sf::RectangleShape rect(sf::Vector2f(10, 10));
    rect.setPosition(i * 10, 0);
    window.draw(rect); // 1000 draw calls
}

// GOOD: Single vertex array
sf::VertexArray vertices(sf::Quads, 4000); // 1000 rectangles * 4 vertices
for (int i = 0; i < 1000; ++i) {
    vertices[i*4].position = sf::Vector2f(i*10, 0);
    vertices[i*4+1].position = sf::Vector2f(i*10+10, 0);
    vertices[i*4+2].position = sf::Vector2f(i*10+10, 10);
    vertices[i*4+3].position = sf::Vector2f(i*10, 10);
}
window.draw(vertices); // Single draw call
```

**Optimize texture atlas usage**
```cpp
// Load a single large texture atlas
sf::Texture atlas;
atlas.loadFromFile("atlas.png");

// Define sub-rectangles for different sprites
sf::IntRect playerRect(0, 0, 32, 32);
sf::IntRect enemyRect(32, 0, 32, 32);

sf::Sprite player(atlas, playerRect);
sf::Sprite enemy(atlas, enemyRect);

// All sprites use same texture - no texture switches
window.draw(player);
window.draw(enemy);
```

**Use view culling for large worlds**
```cpp
void render(sf::RenderWindow& window, const std::vector<Entity>& entities) {
    sf::View view = window.getView();
    sf::FloatRect viewBounds(view.getCenter() - view.getSize() / 2.f, view.getSize());
    
    for (const auto& entity : entities) {
        if (viewBounds.intersects(entity.getBounds())) {
            window.draw(entity.getSprite()); // Only draw visible entities
        }
    }
}
```