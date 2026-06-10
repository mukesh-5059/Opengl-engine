# PixelShader Engine Architecture

This document outlines the architectural components and coding standards for the PixelShader engine.

## 1. Core Infrastructure

### **Application Class**
- **OS/Window Wrapper**: Manages GLFW, OpenGL 4.6 context initialization, and the main execution loop.
- **Diagnostics**: Integrated performance monitoring including an FPS counter, frame time history graph, and controls for VSync and manual FPS limiting.
- **Input Management**: Handles window events and provides hooks for keyboard and mouse input.

### **Dear ImGui Integration**
- **Debugging Layer**: Provides an immediate-mode GUI for real-time engine state manipulation and monitoring.
- **`onGui()` Pattern**: A decentralized UI pattern where each component (Camera, Light, etc.) defines its own debug interface, called by the Application during the render pass.

---

## 2. Graphics Engine Components

### **ResourceManager**
- **The Cache**: Centralized system for loading and caching assets (Shaders, Textures, Meshes). Ensures that resources are loaded only once and shared across multiple entities.

### **Shader Class**
- **GLSL Wrapper**: Handles the lifecycle of GLSL programs: reading from disk, compilation, linking, and error checking.
- **Uniform Management**: Provides utility functions to update shader variables (`setMat4`, `setVec3`, `setFloat`, etc.).

### **Renderer**
- **The Drawing Machine**: Stateless component responsible for executing OpenGL draw calls. It takes a `Scene` and a `Camera` and translates them into GPU commands.

### **Scene**
- **The Container**: Represents the 3D world. Holds collections of `Entities`, `Lights`, and the active `Camera`.

### **Camera**
- **The Eye**: Manages the view and projection matrices. Responsible for calculating the aspect ratio and handling perspective/orthographic projections.

### **Entity**
- **The World Instance**: Represents an object in the scene. Combines a `Transform` (Position, Rotation, Scale) with a `Mesh` and a `Material`.

### **Mesh**
- **The Geometry**: Encapsulates raw vertex data and OpenGL buffer objects (VAO, VBO, EBO).

### **Material**
- **The Look**: An instance of a `Shader` paired with specific data, such as textures and material properties (shininess, base color).

---

## 3. Coding Standards & Conventions

### **Naming Conventions**
- **Classes**: `PascalCase` (e.g., `ResourceManager`)
- **Functions/Local Variables**: `camelCase` (e.g., `initImGui`, `deltaTime`)
- **Member Variables**: `m_camelCase` (e.g., `m_window`, `m_isRunning`)

### **Project Structure & Rules**
- **Header Guards**: Use `#pragma once`.
- **Include Style**: Use angle brackets `<Header.h>` for all includes, as headers are separated into a dedicated include directory.
- **Class Layout**: **Private** members and methods at the **top**, **Public** members and methods at the **bottom**.
- **Casting**: Prefer C-style casts `(float)value` for explicit type conversions.
- **Error Handling**: Use `std::cerr` for unbuffered, immediate error reporting and graceful exit strategies.
