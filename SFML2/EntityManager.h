#pragma once
#include "Entity.h"

typedef std::vector<std::shared_ptr<Entity>> EntityVecType;
typedef std::map<std::string, EntityVecType> EntityMapType;


class EntityManager
{
    EntityVecType m_entitiesVec;
    EntityVecType m_toAddEntitiesVec;
    EntityMapType m_entitiesMap;
    size_t m_totalEntities = 0;

public:
    EntityManager();
    void update();
    std::shared_ptr<Entity> addEntity(const std::string& tag);
    EntityVecType& getEntities();
    EntityVecType& getEntities(const std::string& tag);
    void deleteEntity(std::shared_ptr<Entity>& x);
};

