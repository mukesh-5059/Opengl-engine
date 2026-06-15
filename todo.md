# Engine Architecture & Feature Todo List

This document acts as a development checklist and architecture reference based on the existing codebase.

---

## 1. Resource Manager
A name-based, static/namespace-driven resource cache returning `std::shared_ptr` and storing `std::weak_ptr` for meshes, textures, and shaders.
- **Related Files**:
  - [ResourceManager.h](file:///home/mukes/dev/C++/Projects/aiTesting/src/includes/ResourceManager.h)
  - [ResourceManager.cpp](file:///home/mukes/dev/C++/Projects/aiTesting/src/ResourceManager.cpp)
  - [Scene.cpp](file:///home/mukes/dev/C++/Projects/aiTesting/src/Scene.cpp)
- **Tasks**:
  - [ ] Refactor [Scene.cpp](file:///home/mukes/dev/C++/Projects/aiTesting/src/Scene.cpp#L8-L11) to load textures and shaders through `ResourceManager` instead of manual instantiation.
  - [ ] Extend `ResourceManager` to manage, compile, and cache mesh and material templates.
  - [ ] Implement garbage collection/reference counting using `std::weak_ptr` alongside `std::shared_ptr`.

## 2. Input Manager
An input handling system to query key/mouse states, removing the direct GLFW dependency from update loops.
- **Related Files**:
  - [Application.cpp](file:///home/mukes/dev/C++/Projects/aiTesting/src/Application.cpp#L109-L113)
  - [Camera.cpp](file:///home/mukes/dev/C++/Projects/aiTesting/src/Camera.cpp)
- **Tasks**:
  - [ ] Create an `InputManager` class (static or singleton) that binds window-level GLFW event callbacks.
  - [ ] Replace custom state updates in [Camera.cpp](file:///home/mukes/dev/C++/Projects/aiTesting/src/Camera.cpp) with queries to the new `InputManager`.
  - [ ] Decouple `GLFWwindow*` dependencies from the camera and scene hierarchies.

## 3. Scenegraph
A hierarchical node system to propagate parent-child spatial transformations in the scene.
- **Related Files**:
  - [Scene.h](file:///home/mukes/dev/C++/Projects/aiTesting/src/includes/Scene.h)
  - [Scene.cpp](file:///home/mukes/dev/C++/Projects/aiTesting/src/Scene.cpp)
- **Tasks**:
  - [ ] Design a `Node` / `SceneNode` class capable of parenting other nodes.
  - [ ] Implement hierarchical spatial transformation matrix propagation.
  - [ ] Update `Scene` to manage a root node and traverse the hierarchical tree for updates and rendering.

## 4. Better Logging
A configurable logging library to replace raw console prints with formatted severity levels.
- **Related Files**:
  - All source files, particularly [ShaderProgram.cpp](file:///home/mukes/dev/C++/Projects/aiTesting/src/ShaderProgram.cpp).
- **Tasks**:
  - [ ] Integrate a flexible logging library (like `spdlog`) or implement a robust custom `Logger` class.
  - [ ] Implement logging severity channels (e.g., Trace, Debug, Info, Warning, Error, Critical).
  - [ ] Provide log formatting options (timestamps, file/line context) and console/file output redirection.

## 5. Model Loading
Integration of a library to load and parse complex 3D file formats rather than generating basic primitives.
- **Related Files**:
  - [Primitive.cpp](file:///home/mukes/dev/C++/Projects/aiTesting/src/Primitive.cpp)
  - [Mesh.h](file:///home/mukes/dev/C++/Projects/aiTesting/src/includes/Mesh.h)
- **Tasks**:
  - [ ] Include and link `Assimp` (Asset Import Library) in [CMakeLists.txt](file:///home/mukes/dev/C++/Projects/aiTesting/CMakeLists.txt).
  - [ ] Create a `Model` class that encapsulates an array of sub-meshes and a list of textures.
  - [ ] Write a model importer that parses complex file formats and auto-populates meshes and materials.

## 6. Uniforms Optimization
A uniform location caching mechanism to avoid calling `glGetUniformLocation` every frame.
- **Related Files**:
  - [ShaderProgram.cpp](file:///home/mukes/dev/C++/Projects/aiTesting/src/ShaderProgram.cpp#L70-L92)
  - [Material.cpp](file:///home/mukes/dev/C++/Projects/aiTesting/src/Material.cpp#L13-L45)
- **Tasks**:
  - [ ] Implement a uniform location caching system inside `ShaderProgram` using an efficient lookup structure.
  - [ ] Implement caching of uniform values inside the `Material` or shader layer to prevent duplicate GPU uploads.
  - [ ] Sort render queues by Shader/Material to minimize API binding switches.

## 7. Entity Class
A standardized entity model to represent generic game objects and clean up hardcoded testing primitives.
- **Related Files**:
  - [Scene.h](file:///home/mukes/dev/C++/Projects/aiTesting/src/includes/Scene.h)
  - [Scene.cpp](file:///home/mukes/dev/C++/Projects/aiTesting/src/Scene.cpp)
- **Tasks**:
  - [ ] Implement an `Entity` class consisting of a `Transform`, a reference to a `Mesh`/`Model`, and a `Material`.
  - [ ] Move logic for compiling individual model matrices into `Entity`'s transform structure.
  - [ ] Modify `Scene` to store and render a dynamic collection of `Entity` instances.

## 8. Uniform Buffer Object (UBO)
A shared uniform buffer class to bind scene-wide and camera-wide data efficiently.
- **Related Files**:
  - [Scene.cpp](file:///home/mukes/dev/C++/Projects/aiTesting/src/Scene.cpp#L45-L47)
  - [ShaderProgram.h](file:///home/mukes/dev/C++/Projects/aiTesting/src/includes/ShaderProgram.h)
- **Tasks**:
  - [ ] Create a `UniformBuffer` wrapper class managing OpenGL buffer bindings (`GL_UNIFORM_BUFFER`).
  - [ ] Standardize a global block (e.g., binding point 0) in GLSL shaders for engine-wide parameters.
  - [ ] Bind projection and view matrices once per frame to the global UBO instead of manually uploading them for each object shader.

## 9. Renderer Class
A dedicated rendering pipeline class to separate draw commands and state management from the scene structure.
- **Related Files**:
  - [Scene.cpp](file:///home/mukes/dev/C++/Projects/aiTesting/src/Scene.cpp#L39-L52)
  - [Application.cpp](file:///home/mukes/dev/C++/Projects/aiTesting/src/Application.cpp#L121-L130)
- **Tasks**:
  - [ ] Extract the drawing logic into a stateless `Renderer` class.
  - [ ] Centralize general graphics pipeline state settings (e.g., blending, depth test toggles).
  - [ ] Implement high-level rendering operations like `Renderer::drawScene(Scene*, Camera*)`.

## 10. Dynamic Vertex Layout
A system to dynamically define vertex layouts and attributes rather than using a hardcoded vertex structure.
- **Related Files**:
  - [Mesh.h](file:///home/mukes/dev/C++/Projects/aiTesting/src/includes/Mesh.h)
  - [Mesh.cpp](file:///home/mukes/dev/C++/Projects/aiTesting/src/Mesh.cpp)
- **Tasks**:
  - [ ] Design a `VertexLayout` class that defines attributes (type, size, normalized, offset, stride).
  - [ ] Refactor `Mesh` to accept dynamic layouts instead of hardcoding layout bindings in `setupMesh`.
