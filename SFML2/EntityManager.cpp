#include "EntityManager.h"
#include <iostream>

typedef std::vector<std::shared_ptr<Entity>> EntityVecType;
typedef std::map<std::string, EntityVecType> EntityMapType;



EntityManager::EntityManager(){}

void EntityManager::update()
{
    for (auto a : m_toAddEntitiesVec)
    {
        m_entitiesVec.push_back(a);
        m_entitiesMap[a->getTag()].push_back(a);
    }
    //brisanje iz vectora
    m_entitiesVec.erase(
        std::remove_if(m_entitiesVec.begin(), m_entitiesVec.end(),
            [](const std::shared_ptr<Entity>& entity) {
                return !entity->isActive();
            }),
        m_entitiesVec.end()
    );

    //brisanje iz mape
    for (auto it = m_entitiesMap.begin(); it != m_entitiesMap.end(); ) {
        auto& vectorOfEntities = it->second;
        vectorOfEntities.erase(
            std::remove_if(vectorOfEntities.begin(), vectorOfEntities.end(),
                [](const std::shared_ptr<Entity>& entity) {
                    return !entity->isActive();
                }),
            vectorOfEntities.end()
        );

        // If the vector is empty after removal, remove the key-value pair from the map
        if (vectorOfEntities.empty()) {
            it = m_entitiesMap.erase(it);
        }
        else {
            ++it;
        }
    }





    m_toAddEntitiesVec.clear();
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
