#pragma once
#include "Id.hpp"
#include <deque>
#include <vector>

class Scene;

class EntityManager{
    private:
        Id::idType MIN_FREE_INDICES = 256;
        std::vector<Id::idType> m_generations;
        std::deque<Id::idType> m_freeIndices;
        Scene* m_scene;

    public:
        EntityManager(Scene* scene);
        ~EntityManager();
        Id::EntityId create();
        bool isValid(Id::EntityId id);
        void destroy(Id::EntityId id);
};