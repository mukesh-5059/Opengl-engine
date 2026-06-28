#pragma once
#include "Id.hpp"
#include <deque>
#include <vector>


class EntityManager{
    private:
        Id::idType MIN_FREE_INDICES = 256;
        std::vector<Id::idType> m_generations;
        std::deque<Id::idType> m_freeIndices;

    public:
        EntityManager();
        ~EntityManager();
        Id::idType create();
        bool isValid(Id::idType id);
        void destroy(Id::idType id);
};