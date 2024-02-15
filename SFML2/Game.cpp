#include "Game.h"

#define _USE_MATH_DEFINES

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <iomanip>


int randomBroj(int min, int max) 
{
    return rand() % (max - min + 1) + min;
}

float randomBroj(float a, float b) {
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}


Game::Game(const std::string & config)
{
    init(config);
}


void Game::init(const std::string & config)
{
    //Read config file
    std::ifstream in(config);
    std::string line;
    int wHeight=0, wWidth=0, wFrameLimit=60, wIsFullScreen;
    std::string fName; 
    int fSize, fR, fG, fB;


    while (in>>line)
    {
        if (line.compare("Window") == 0)
        {
            in >> wHeight >> wWidth >> wFrameLimit >>wIsFullScreen;
        } 
        else if (line.compare("Font") == 0)
        {
            in >> fName >> fSize >> fR >> fG >> fB;
            if (!m_font.loadFromFile(fName))
            {
               std::cerr << "Font loading error";
               return;
            }
            sf::Text text("Test", m_font, fSize);
            text.setPosition(0, 0);
            text.setFillColor(sf::Color(fR, fG, fB));
            m_text = text;
            
        }
        else if (line.compare("Player") == 0)
        {
            in >> m_playerConfig.SR >> m_playerConfig.CR >>m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FG >>
                m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V;
        }
        else if (line.compare("Enemy") == 0)
        {
            in >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >>
                m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
        }
        else if (line.compare("Bullet") == 0)
        {
            in >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR >> m_bulletConfig.FG >>
                m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
        }
    }
    m_window.create(sf::VideoMode(wHeight, wWidth), "A2");
    m_window.setFramerateLimit(wFrameLimit);
    
    
    spawnPlayer();
}

void Game::run()
{
    while(m_running)
    {
        //std::cout << "1";
        m_entities.update();

        if (!m_paused)
        {
            sEnemySpawner();
            sMovement();
            sCollision();
            sUserInput();
            sLifespan();
            sUlt();
            sFreeze();
        }
        sRender();

        m_currentFrame++;
    }
}

void Game::sUlt()
{
    if (m_currentFrame - m_lastUltTime >= 720 && m_player->cInput->ult == true)
    {
        m_ultActive = true;
        m_lastUltTime = m_currentFrame;
    }
    if (m_currentFrame - m_lastUltTime >= 150 && m_ultActive == true)
    {
        m_ultActive = false;
    }
    if (m_ultActive == true && m_currentFrame - m_lastUltTick >= 10)
    {
        spawnSpecialWeapon();
        m_lastUltTick = m_currentFrame;
    }
}

void Game::sFreeze()
{
    if (m_currentFrame - m_lastFreezeTime >= 2100 && m_player->cInput->freeze == true)
    {
        std::cout << "1";
        m_freezeActive = true;
        m_lastFreezeTime = m_currentFrame;
    }
    if (m_currentFrame - m_lastFreezeTime >= 300 && m_freezeActive == true)
    {
        m_freezeActive = false;
        for (auto& a : m_entities.getEntities("enemy"))
        {
            a->cTransform->frozen = false;
        }
    }
    if (m_freezeActive == true)
    {
        for (auto& a : m_entities.getEntities("enemy"))
        {
            a->cTransform->frozen = true;
        }
    }
}

void Game::sEnemySpawner()
{
    if (m_currentFrame - m_lastEnemySpawnTime == m_enemyConfig.SI)
    {
        spawnEnemy();
    }
}

void Game::sMovement()
{
    m_player->cTransform->velocity = Vec2(0.0, 0.0);
    if (m_player->cInput->right && m_window.getSize().x >= m_player->cTransform->pos.x + m_playerConfig.S + m_player->cCollision->radius)
    {
        //std::cout << "Move right " << m_playerConfig.S << std::endl;
		m_player->cTransform->velocity += Vec2(m_playerConfig.S, 0.0);
    }
    if (m_player->cInput->up && 0 <= m_player->cTransform->pos.y - m_playerConfig.S - m_player->cCollision->radius)
    {
		//std::cout << "Move up";
        m_player->cTransform->velocity += Vec2(0.0, -m_playerConfig.S);
    }
    if (m_player->cInput->left && 0 <= m_player->cTransform->pos.x - m_playerConfig.S - m_player->cCollision->radius)
    {
		//std::cout << "Move left";
        m_player->cTransform->velocity += Vec2(-m_playerConfig.S, 0.0);
    }
	if (m_player->cInput->down && m_window.getSize().y >= m_player->cTransform->pos.y + m_playerConfig.S + m_player->cCollision->radius)
	{
		//std::cout << "Move down";
		m_player->cTransform->velocity += Vec2(0.0, m_playerConfig.S);
	}
    for (auto& e: m_entities.getEntities())
    {


        if (e->cTransform != nullptr)
        {
            if (e->cTransform->frozen == false) {
                e->cTransform->pos += e->cTransform->velocity;
                e->cShape->circle.setRotation(e->cShape->circle.getRotation() + 1);

                if (e->cCollision != nullptr && (e->getTag().compare("enemy") == 0)) {

                    if (e->cTransform->pos.x < e->cCollision->radius || e->cTransform->pos.x + e->cCollision->radius  > m_window.getSize().x)
                    {
                        e->cTransform->velocity.x *= -1.0;
                    }
                    if (e->cTransform->pos.y < e->cCollision->radius || e->cTransform->pos.y + e->cCollision->radius  > m_window.getSize().y)
                    {
                        e->cTransform->velocity.y *= -1.0;
                    }
                }
            }
        }
    }
}

void Game::sUserInput()
{
    sf::Event event;
    while(m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            m_running = false;
        }

        if (event.type == sf::Event::KeyPressed) 
        {
            switch (event.key.code)
            {
            case sf::Keyboard::W:
                //std::cout << "W key pressed\n";
                m_player->cInput->up = true;
                break;

            case sf::Keyboard::S:
                //std::cout << "S key pressed\n";
                m_player->cInput->down = true;
                break;

            case sf::Keyboard::A:
                //std::cout << "A key pressed\n";
                m_player->cInput->left = true;
                break;

            case sf::Keyboard::D:
               // std::cout << "D key pressed\n";
                m_player->cInput->right = true;
                break;

            case sf::Keyboard::E:
                //std::cout << "E key pressed\n";
                m_player->cInput->freeze = true;
                break;

            case sf::Keyboard::P:
                std::cout << "P key pressed\n";
                m_paused = true;
                break;
            default:
                break;
            }
        }

        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::W:
                m_player->cInput->up = false;
                break;

            case sf::Keyboard::S:
                m_player->cInput->down = false;
                break;

            case sf::Keyboard::A:
                m_player->cInput->left = false;
                break;

            case sf::Keyboard::D:
                m_player->cInput->right = false;
                break;

            case sf::Keyboard::E:
                m_player->cInput->freeze = false;
                break;
            default:
                break;
            }
        }

        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
            }
			if (event.mouseButton.button == sf::Mouse::Right)
			{
                m_player->cInput->ult = true;
			}
        }
        if (event.type == sf::Event::MouseButtonReleased)
        {
            if (event.mouseButton.button == sf::Mouse::Right)
            {
                m_player->cInput->ult = false;
            }
        }
    }
}

void Game::sLifespan()
{
    for (auto& e : m_entities.getEntities())
    {
        if(e->cLifespan != nullptr)
        {
            
            if(e->cLifespan->remaining == 0)
            {
                m_entities.deleteEntity(e);
                continue;
            }
            e->cLifespan->remaining--;
            auto opacity = e->cShape->circle.getFillColor().a;
            opacity -= 255/e->cLifespan->total;
            e->cShape->circle.setFillColor(sf::Color(e->cShape->circle.getFillColor().r, e->cShape->circle.getFillColor().g, e->cShape->circle.getFillColor().b, opacity));
			e->cShape->circle.setOutlineColor(sf::Color(e->cShape->circle.getOutlineColor().r, e->cShape->circle.getOutlineColor().g, e->cShape->circle.getOutlineColor().b, opacity));
        }
    }
}

void Game::sRender()
{
    m_window.clear();
    for (auto a : m_entities.getEntities())
    {
        
        a->cShape->circle.setPosition(a->cTransform->pos.x, a->cTransform->pos.y);
        m_window.draw(a->cShape->circle);
    }

    float ultReadyIn = m_lastUltTime - m_currentFrame + 720;

    if(ultReadyIn < 0)
    {
        ultReadyIn = 0;
    }
    ultReadyIn /= 60;

    std::string a = "Score: " + std::to_string(m_score) + "\nUlt ready in: " + std::to_string(static_cast<int>(ultReadyIn)) +
        "." +
        std::to_string(static_cast<int>(ultReadyIn * 10) % 10) + " s";

    ultReadyIn = m_lastFreezeTime - m_currentFrame + 2100;
    ultReadyIn /= 60;

    if (ultReadyIn < 0)
    {
        ultReadyIn = 0;
    }

    a+= "\nFreeze ready in: " + std::to_string(static_cast<int>(ultReadyIn)) +
        "." +
        std::to_string(static_cast<int>(ultReadyIn * 10) % 10) + " s";

    m_text.setString(a);
    
    m_window.draw(m_text);
    m_window.display();
}

void Game::sCollision()
{
    for (auto& e : m_entities.getEntities("enemy"))
    {
        for (auto& b: m_entities.getEntities("bullet"))
        {
            float dist = b->cTransform->pos.dist(e->cTransform->pos);
            if (b->cCollision->radius + e->cCollision->radius >= dist)
            {      
                spawnSmallEnemies(e);
                m_entities.deleteEntity(e);
                m_entities.deleteEntity(b);
                m_score += e->cScore->score;     
            }
        }
        float dist = m_player->cTransform->pos.dist(e->cTransform->pos);
        if (m_player->cCollision->radius + e->cCollision->radius >= dist)
        {
            m_player->cTransform->pos = Vec2(m_window.getSize().x / 2, m_window.getSize().y / 2);
            m_entities.deleteEntity(e);
            spawnSmallEnemies(e);
        }
    }
}

void Game::setPaused(bool paused)
{
    m_paused = paused;
}

void Game::spawnPlayer()
{
    
    auto e = m_entities.addEntity("player");
    e->cTransform = std::make_shared<CTransform>(Vec2(m_window.getSize().x / 2.0, m_window.getSize().y / 2.0), Vec2(0, 0), 0);
    e->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);
    e->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);
    e->cInput = std::make_shared<CInput>();

    m_player = e;
}

void Game::spawnEnemy()
{
    auto e = m_entities.addEntity("enemy");

    int k=randomBroj(m_enemyConfig.VMIN, m_enemyConfig.VMAX);
    Vec2 pozicija(randomBroj(m_enemyConfig.SR + 1, m_window.getSize().x - m_enemyConfig.SR - 1), randomBroj(m_enemyConfig.SR + 1, m_window.getSize().y - m_enemyConfig.SR - 1));
    bool preklapanje = false;
    do
    {
        pozicija.x = randomBroj(m_enemyConfig.SR + 1, m_window.getSize().x - m_enemyConfig.SR - 1);
        pozicija.y = randomBroj(m_enemyConfig.SR + 1, m_window.getSize().y - m_enemyConfig.SR - 1);

        preklapanje=false;

        if (m_player->cTransform->pos.dist(pozicija) - m_playerConfig.CR - m_enemyConfig.CR <= 0)
        {
            preklapanje = true;
        }

    } while (preklapanje);

    e->cTransform = std::make_shared<CTransform>(pozicija, Vec2(randomBroj(m_enemyConfig.SMIN, m_enemyConfig.SMAX), randomBroj(m_enemyConfig.SMIN, m_enemyConfig.SMAX)) , randomBroj(0,360));
    e->cShape = std::make_shared<CShape>(m_enemyConfig.SR,k, sf::Color(randomBroj(20,255), randomBroj(20, 255), randomBroj(20, 255)), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT);
    e->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);
    e->cScore = std::make_shared<CScore>(k*100);

    m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> enemy)
{
    int pointCount = enemy->cShape->circle.getPointCount();
    float angle = 0;
    for (int i = 0; i < pointCount; i++)
    {
        angle = i * (360/pointCount);
        auto e = m_entities.addEntity("mali");
        angle = angle * M_PI / 180;
     
        Vec2 pocetak(std::cos(angle) * enemy->cCollision->radius, std::sin(angle) * enemy->cCollision->radius);

        e->cTransform = std::make_shared<CTransform>(enemy->cTransform->pos + pocetak, pocetak.norm() * enemy->cTransform->velocity.length() / 2, i);
        e->cShape = std::make_shared<CShape>(enemy->cShape->circle.getRadius() /2, pointCount , enemy->cShape->circle.getFillColor(), enemy->cShape->circle.getOutlineColor(), enemy->cShape->circle.getOutlineThickness());
        e->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);
    }

}

void Game::spawnBullet(std::shared_ptr<Entity> e, const Vec2 & mousePos)
{
    auto bullet = m_entities.addEntity("bullet");
    Vec2 bulletVec = mousePos - e->cTransform->pos;
    bulletVec = bulletVec.norm() * m_bulletConfig.S;
    bullet->cTransform = std::make_shared<CTransform>(e->cTransform->pos, bulletVec, atan(bulletVec.y/bulletVec.x));
    bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
    bullet->cShape = std::make_shared<CShape>(m_bulletConfig.SR, 33, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), m_bulletConfig.OT);
    bullet->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
}

void Game::spawnSpecialWeapon()
{   
    float angle = 0;
    for (int i = 0; i < 10; i++)
    {
        angle = i * 36;
		angle = angle * M_PI / 180;

        auto bullet = m_entities.addEntity("bullet");
		bullet->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
		Vec2 pocetak(std::cos(angle) * bullet->cCollision->radius, std::sin(angle) * bullet->cCollision->radius);
		bullet->cTransform = std::make_shared<CTransform>(m_player->cTransform->pos + pocetak, pocetak.norm() * m_bulletConfig.S, angle);
		bullet->cShape = std::make_shared<CShape>(m_playerConfig.SR/3, m_playerConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);
		bullet->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L/2);
    }
}