#include "Id.hpp"
#include <EntityManager.hpp>
#include <cassert>

EntityManager::EntityManager() : m_generations(), m_freeIndices() {}
EntityManager::~EntityManager() {};

Id::idType EntityManager::create(){
    Id::idType index;
    if(m_freeIndices.size() > MIN_FREE_INDICES){
        index = m_freeIndices.front();
        m_freeIndices.pop_front();
    }else{
        m_generations.push_back(0);
        index = m_generations.size() - 1;
    }
    Id::idType id = Id::generateId(m_generations[index], index);
    return id;
}

bool EntityManager::isValid(Id::idType id){
    Id::idType index = Id::indexOf(id);

    if(index >= m_generations.size()) return false;
    if(id == Id::invalidId) return false;
    
    return m_generations[index] == Id::generationOf(id);
}

void EntityManager::destroy(Id::idType id){
    assert(isValid(id));
    Id::idType index = Id::indexOf(id);
    ++m_generations[index];
    m_freeIndices.push_back(index);
}