cmake_minimum_required(VERSION 3.10.0)
project(PixelShader VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)

file(GLOB src src/*.cpp)
file(GLOB imgui dependencies/lib/imgui/*.cpp)
file(GLOB stbImage dependencies/lib/StbImage.c)

add_executable(PixelShader)
 add_compile_definitions(GLFW_INCLUDE_NONE)

target_sources(PixelShader PRIVATE ${imgui} dependencies/lib/glad.c ${stbImage} ${src})

target_include_directories(PixelShader PRIVATE dependencies/includes)
target_include_directories(PixelShader PRIVATE src/includes)

target_link_libraries(PixelShader PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lib/libglfw3.a"
    "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lib/libassimp.a"
    OpenGL::GL
)
 