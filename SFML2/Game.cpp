#include "Game.h"

#include <iostream>
#include <fstream>
#include <string>

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
            // sf::Text text("Test", font, fSize);
            // text.setPosition(0, (float)text.getCharacterSize());
            // //text.setColor(sf::Color(fR, fG, fB));
            // text.setFillColor(sf::Color(fR, fG, fB));
        }
        else if (line.compare("Player") == 0)
        {
            in >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.FR >> m_playerConfig.FG >>
                m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V >> m_playerConfig.S;
        }
        else if (line.compare("Enemy") == 0)
        {
            in >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >>
                m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX;
        }
        // struct PlayerConfig {int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, float S};
        // struct EnemyConfig  {int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI, float SMIN, SMAX};
        // struct BulletConfig {int SR, CR, FR, FG, FB, OR, OB, OT, V, L, float S};
        else if (line.compare("Bullet") == 0)
        {
            in >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.FR >> m_bulletConfig.FG >>
                m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L >> m_bulletConfig.S;
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
        m_entities.update();

        if (!m_paused)
        {
        sEnemySpawner();
        sMovement();
        sCollision();
        sUserInput();
        }
        sRender();

        m_currentFrame++;
    }
}

void Game::sEnemySpawner()
{

}

void Game::sMovement()
{

}

void Game::sUserInput()
{

}

void Game::sRender()
{

}

void Game::sCollision()
{
    /*for (auto& e : m_entities.getEntities("enemy"))
    {
        for (auto& b: m_entities.getEntities("bullet"))
        {
            float dist = b->cTransform->pos.dist(e->cTransform->pos);
            if (b->cCollision->radius + e->cCollision->radius <= dist)
            {
                
                //TODO Handle Bullet Enemy collision
                spawnSmallEnemies(e);
                m_entities.deleteEntity(e);
                m_score += e->cScore->score;
                
            }
        }
        float dist = m_player->cTransform->pos.dist(e->cTransform->pos);
        if (m_player->cCollision->radius + e->cCollision->radius <= dist)
        {
            //TODO Handle player enemy collision
            m_player->cTransform = std::make_shared<CTransform>(Vec2(m_window.getSize().x/2.0, m_window.getSize().y)/2.0, Vec2(0.0, 0.0), 0);
        }
    }*/
}

void Game::setPaused(bool paused)
{
    m_paused = paused;
}

void Game::spawnPlayer()
{
    m_player = m_entities.addEntity("player");
    m_player->cTransform = std::make_shared<CTransform>(Vec2(m_window.getSize().x / 2.0, m_window.getSize().y / 2.0), Vec2(0, 0), 0);
    m_player->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);
    m_player->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);
    m_player->cInput = std::make_shared<CInput>();
}

void Game::spawnEnemy()
{
    auto e = m_entities.addEntity("enemy");

    int k=randomBroj(m_enemyConfig.VMIN, m_enemyConfig.VMAX);

    e->cTransform = std::make_shared<CTransform>(Vec2(randomBroj(m_enemyConfig.SR,m_window.getSize().x - m_enemyConfig.SR), randomBroj(m_enemyConfig.SR, m_window.getSize().y - m_enemyConfig.SR)), Vec2(randomBroj(m_enemyConfig.SMIN, m_enemyConfig.SMAX), randomBroj(m_enemyConfig.SMIN, m_enemyConfig.SMAX)) , randomBroj(0,360));
    e->cShape = std::make_shared<CShape>(m_enemyConfig.SR,k, sf::Color(0, 0,0), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_enemyConfig.OT);
    e->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);
    e->cScore = std::make_shared<CScore>(k*100);

    m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
    int pointCount = e->cShape->circle.getPointCount();
    float angle = 0;
    /*for (int i = 0; i < pointCount; i++)
    {
        angle = i * (360/pointCount);
        auto e = m_entities.addEntity("enemy");
        e->cTransform = std::make_shared<CTransform>(Vec2)
    }*/

}

void Game::spawnBullet(std::shared_ptr<Entity> e, const Vec2 & mousePos)
{
    auto bullet = m_entities.addEntity("bullet");
    Vec2 bulletVec = mousePos - e->cTransform->pos;
    bulletVec = bulletVec.norm() * m_bulletConfig.S;
    
}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
    
}