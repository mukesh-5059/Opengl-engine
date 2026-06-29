#include "Id.hpp"
#include <EntityManager.hpp>
#include <Scene.h>
#include <cassert>

EntityManager::EntityManager(Scene* scene) : m_scene(scene), m_generations(), m_freeIndices() {
    assert(scene != nullptr);
}
EntityManager::~EntityManager() {};

Id::EntityId EntityManager::create(){
    Id::EntityId index;
    if(m_freeIndices.size() > MIN_FREE_INDICES){
        index = m_freeIndices.front();
        m_freeIndices.pop_front();
    }else{
        m_generations.push_back(0);
        index = m_generations.size() - 1;
    }
    Id::EntityId id = Id::generateId(m_generations[index], index);
    m_scene->getTransformManager().create(id);
    return id;
}

bool EntityManager::isValid(Id::EntityId id){
    Id::EntityId index = Id::indexOf(id);

    if(index >= m_generations.size()) return false;
    if(id == Id::invalidId) return false;
    
    return m_generations[index] == Id::generationOf(id);
}

void EntityManager::destroy(Id::EntityId id){
    assert(isValid(id));
    m_scene->getTransformManager().destroy(id);
    Id::EntityId index = Id::indexOf(id);
    ++m_generations[index];
    m_freeIndices.push_back(index);
}