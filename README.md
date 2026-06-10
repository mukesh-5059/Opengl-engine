# PixelShader

A modern OpenGL project setup with essential dependencies for 3D graphics development.

## Project Dependencies

The project uses several third-party libraries to handle windowing, rendering, mathematics, and asset loading. Below is a list of the dependencies and their roles:

### 1. OpenGL
- **Purpose**: The core graphics API used for hardware-accelerated rendering.
- **Usage**: Linked as a system dependency via CMake's `find_package(OpenGL REQUIRED)`.

### 2. GLFW (Graphics Library Framework)
- **Purpose**: Provides a simple API for creating windows, contexts, and surfaces, as well as handling input (keyboard, mouse, etc.).
- **Location**: `dependencies/includes/GLFW`, `dependencies/lib/libglfw3.a`.
- **Usage**: Used to initialize the window and manage the OpenGL context.

### 3. GLAD (OpenGL Extension Loader)
- **Purpose**: A library that manages OpenGL function pointers for modern OpenGL versions.
- **Location**: `dependencies/includes/glad`, `dependencies/lib/glad.c`.
- **Usage**: Must be initialized after the OpenGL context is created to enable modern OpenGL features.

### 4. GLM (OpenGL Mathematics)
- **Purpose**: A header-only C++ mathematics library designed for graphics software, based on the GLSL (OpenGL Shading Language) specification.
- **Location**: `dependencies/includes/glm`.
- **Usage**: Handles all mathematical operations such as vectors, matrices, and transformations.

### 5. Assimp (Open Asset Import Library)
- **Purpose**: A library to import various 3D model formats (like `.obj`, `.fbx`, `.gltf`) into a uniform data structure.
- **Location**: `dependencies/includes/assimp`, `dependencies/lib/libassimp.a`.
- **Usage**: Used for loading 3D assets into the application.

### 6. Dear ImGui
- **Purpose**: A minimalist, immediate-mode graphical user interface library for C++.
- **Location**: `dependencies/includes/imgui`, `dependencies/lib/imgui/*.cpp`.
- **Usage**: Provides a toolkit for creating debugging overlays and developer tools within the application.

### 7. stb_image
- **Purpose**: A single-file public domain library for loading images.
- **Location**: `dependencies/includes/stb/StbImage.h`, `dependencies/lib/StbImage.c`.
- **Usage**: Primarily used for loading textures from various image formats (PNG, JPG, etc.).

## Build Instructions

This project uses CMake. To build the project:

1. Create a build directory: `mkdir build && cd build`
2. Configure the project: `cmake ..`
3. Build the project: `cmake --build .` (or use your preferred build tool)
