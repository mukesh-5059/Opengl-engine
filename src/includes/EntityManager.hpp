#pragma once
#include "Id.hpp"
#include <deque>
#include <vector>
#include <string>

class Scene;

class EntityManager{
    private:
        Id::idType MIN_FREE_INDICES = 256;
        Id::idType m_currentCount;
        std::vector<Id::idType> m_generations;
        std::deque<Id::idType> m_freeIndices;
        Scene* m_scene;

    public:
        EntityManager(Scene* scene);
        ~EntityManager();

        Id::EntityId create(const std::string& name);
        bool isValid(Id::EntityId id);
        Id::idType getCount() {return m_currentCount;}
        void destroy(Id::EntityId id);
};