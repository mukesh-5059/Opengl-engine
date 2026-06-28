# Engine Architecture & Feature Todo List

This document acts as a development checklist and notes log.

- [x] 1. Resource Manager
  * **Notes**: A new shared pointer instance is created everytime we need gui, so decremented 1 from ref count in gui.
  * **Notes**: Automatic prune timer needs to be updated.
- [ ] 2. Input Manager
  * **Notes**: 
- [ ] 3. Scenegraph
  * **Notes**: 
- [ ] 4. Better Logging
  * **Notes**: 
- [ ] 5. Model Loading
  * **Notes**: Baked local transforms into vertices
- [x] 6. Uniforms Optimization
  * **Notes**: 
- [x] 7. Entity Class
  * **Notes**: Uses mesh instead of models.
- [x] 8. Uniform Buffer Object (UBO)
  * **Notes**: 
- [x] 9. Renderer Class
  * **Notes**: needs optimizations
- [ ] 10. Dynamic Vertex Layout
  * **Notes**: 
- [x] 11. Default Shaders
  * **Notes**: 
- [x] 12. Default Textures
  * **Notes**: If one texture is default(specular) and other is error(diffuse) the outcome is default
- [ ] 13. State Baching
  * **Notes**:
- [ ] 14. Replace string keys in unordered maps
  * **Notes** :
- [ ] 15. Entity Componenet System

FPS now: 135~, 1000 cubes