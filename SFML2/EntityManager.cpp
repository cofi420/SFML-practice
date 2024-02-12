#include "EntityManager.h"

typedef std::vector<std::shared_ptr<Entity>> EntityVecType;
typedef std::map<std::string, EntityVecType> EntityMapType;

bool forDelete(std::shared_ptr<Entity> a)
{
    return !a->isActive();
}

EntityManager::EntityManager(){}

void EntityManager::update()
{
    /*for (auto a : m_toAddEntitiesVec)
    {
        m_entitiesVec.push_back(a);
        m_entitiesMap[a->getTag()].push_back(a);
    }

    std::remove_if(m_entitiesVec.begin(), m_entitiesVec.end(), forDelete);
    std::erase_if(m_entitiesMap, forDelete);


    m_toAddEntitiesVec.clear();*/
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
    auto e = std::make_shared<Entity>(m_totalEntities++, tag);
    m_toAddEntitiesVec.push_back(e);
    return e;
}

EntityVecType& EntityManager::getEntities()
{
    return m_entitiesVec;
}

EntityVecType& EntityManager::getEntities(const std::string& tag)
{
    return m_entitiesMap[tag];
}

void EntityManager::deleteEntity(std::shared_ptr<Entity>& x)
{
    x->destroy();
}
