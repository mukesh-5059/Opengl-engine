# Deferred Features & Discussion Points

This file tracks advanced features we have identified but decided to defer for later stages of development.

## 1. Shader Includes (`#include` in GLSL)
**Status**: Pending Discussion
**Description**: The ability to share common GLSL code (like lighting math, constants, or utility functions) across multiple `.vert` and `.frag` files.

## 2. Dynamic Uniforms (Material Property System)
**Status**: Pending
**Description**: Refactoring the `Material` class to use a `std::vector` of `std::variant` to store arbitrary uniform types (float, vec3, mat4) instead of hardcoded member variables.

## 3. Input Manager
**Status**: Pending
**Description**: A centralized, static system for handling keyboard and mouse input to avoid passing `GLFWwindow*` pointers through the entire Scene hierarchy.

## 4. Flexible Vertex Layout (Buffer Layout)
**Status**: Pending
**Description**: A system to allow meshes to have different vertex attributes (e.g., only positions for shadow maps, or extra attributes for skeletal animation) instead of a fixed "Universal Vertex".

## 5. ResourceManager
**Status**: Pending
**Description**: A centralized asset cache to ensure each texture, shader, and model is loaded into memory only once and shared across the engine.

## 6. Model Class (Asset Container)
**Status**: Pending
**Description**: A container for complex 3D assets loaded via Assimp that may consist of multiple sub-meshes and materials.

## 7. Hierarchical Uniform Management
**Status**: Pending Discussion
**Description**: Categorizing uniforms into Scene-wide (Projection, View, Lights), Entity-wide (Model Matrix), and Mesh-wide (Textures, Material Properties) tiers to minimize GPU state changes and improve organization.



