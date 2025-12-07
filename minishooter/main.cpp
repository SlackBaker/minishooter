#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <cmath>

using namespace sf;


Vector2f normalize(Vector2f v) {
    float length = sqrt(v.x * v.x + v.y * v.y);
    if (length == 0) return Vector2f(0, 0);
    return v / length;
}


struct Bullet {
    CircleShape shape;
    Vector2f direction;
    float speed = 10.f;

    Bullet(Vector2f pos, Vector2f dir) {
        shape.setRadius(5.f);
        shape.setFillColor(Color::Red);
        shape.setPosition(pos);
        direction = dir;
    }

    void update() {
        shape.move(direction * speed);
    }
};


struct Enemy {
    RectangleShape shape;
    Vector2f direction;
    float speed = 2.5f;

    Enemy(Vector2f pos, Vector2f dir) {
        shape.setSize(Vector2f(40.f, 40.f));
        shape.setFillColor(Color::Green);
        shape.setPosition(pos);
        direction = dir;
    }

    void update() {
        shape.move(direction * speed);
    }
};


void Update(int &keyTime, CircleShape &player, RenderWindow &window);
void Draw(RenderWindow &window, CircleShape &player, std::vector<Bullet> &bullets, std::vector<Enemy> &enemies);

int main() {
    srand(static_cast<unsigned>(time(NULL)));

    int keyTime = 8;

    RenderWindow window(VideoMode(700, 400), "Mini Shooter");
    window.setFramerateLimit(60);

    // player
    CircleShape player;
    player.setRadius(20.f);
    player.setFillColor(Color::White);
    player.setPosition(300.f, 200.f);

    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;

    int shootTimer = 0;
    int enemySpawnTimer = 0;

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                window.close();
        }

        Vector2f playerCenter(
            player.getPosition().x + player.getRadius(),
            player.getPosition().y + player.getRadius()
        );

        // ------- СТРІЛЬБА У НАПРЯМКУ МИШІ -------
        if (shootTimer < 10)
            shootTimer++;

        if (Mouse::isButtonPressed(Mouse::Left) && shootTimer >= 10) {

            Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
            Vector2f direction = normalize(mousePos - playerCenter);

            bullets.push_back(Bullet(playerCenter, direction));

            shootTimer = 0;
        }

    
        for (int i = 0; i < bullets.size(); i++) {
            bullets[i].update();

            Vector2f pos = bullets[i].shape.getPosition();
            if (pos.x < 0 || pos.x > window.getSize().x ||
                pos.y < 0 || pos.y > window.getSize().y) {
                bullets.erase(bullets.begin() + i);
                i--;
            }
        }

        
        if (enemySpawnTimer < 40)
            enemySpawnTimer++;
        else {
            enemySpawnTimer = 0;

            int side = rand() % 4; // 0=left,1=right,2=top,3=bottom
            Vector2f pos;

            switch (side) {
                case 0: pos = Vector2f(0, rand() % window.getSize().y); break;
                case 1: pos = Vector2f(window.getSize().x, rand() % window.getSize().y); break;
                case 2: pos = Vector2f(rand() % window.getSize().x, 0); break;
                case 3: pos = Vector2f(rand() % window.getSize().x, window.getSize().y); break;
            }

            Vector2f dir = normalize(playerCenter - pos);
            enemies.push_back(Enemy(pos, dir));
        }


        for (int i = 0; i < enemies.size(); i++) {
            enemies[i].update();
        }

    
        for (int i = 0; i < enemies.size(); i++) {
            for (int j = 0; j < bullets.size(); j++) {

                if (enemies[i].shape.getGlobalBounds().intersects(bullets[j].shape.getGlobalBounds())) {
                    enemies.erase(enemies.begin() + i);
                    bullets.erase(bullets.begin() + j);
                    i--;
                    break;
                }
            }
        }

        Update(keyTime, player, window);
        Draw(window, player, bullets, enemies);
    }

    return 0;
}


void Update(int &keyTime, CircleShape &player, RenderWindow &window) {
    if (keyTime < 8)
        keyTime++;

    const float speed = 5.f;

    bool left  = Keyboard::isKeyPressed(Keyboard::A);
    bool right = Keyboard::isKeyPressed(Keyboard::D);
    bool up    = Keyboard::isKeyPressed(Keyboard::W);
    bool down  = Keyboard::isKeyPressed(Keyboard::S);

    if (left || right) up = down = false;

    if (left && player.getPosition().x > 0)
        player.move(-speed, 0);
    else if (right && player.getPosition().x < window.getSize().x - player.getRadius() * 2)
        player.move(speed, 0);

    if (up && player.getPosition().y > 0)
        player.move(0, -speed);
    else if (down && player.getPosition().y < window.getSize().y - player.getRadius() * 2)
        player.move(0, speed);
}

void Draw(RenderWindow &window, CircleShape &player, std::vector<Bullet> &bullets, std::vector<Enemy> &enemies) {
    window.clear(Color::Black);

    window.draw(player);

    for (auto &b : bullets)
        window.draw(b.shape);

    for (auto &e : enemies)
        window.draw(e.shape);

    window.display();
}
