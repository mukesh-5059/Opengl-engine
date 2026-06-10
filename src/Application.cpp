#include <Application.h>
#include <iostream>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
 #include <imgui/imgui_impl_opengl3.h>

Application::Application(const std::string& title, int width, int height)
    : m_title(title), m_width(width), m_height(height), m_window(nullptr), m_isRunning(false), m_vSync(true), m_targetFps(60) {
}

Application::~Application() {
    shutdownImGui();
    if (m_window) {
        glfwDestroyWindow(m_window);
    }
    glfwTerminate();
}

bool Application::init() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    std::cout << "OpenGL Info:" << std::endl;
    std::cout << "  Vendor:   " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "  Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "  Version:  " << glGetString(GL_VERSION) << std::endl;

    glViewport(0, 0, m_width, m_height);

    initImGui();
    
    return true;
}

void Application::run() {
    if (!init()) {
        return;
    }

    m_isRunning = true;
    float lastFrame = 0.0f;

    while (m_isRunning && !glfwWindowShouldClose(m_window)) {
        float currentFrame = (float)glfwGetTime();
        float deltaTime = currentFrame - lastFrame;

        if (!m_vSync && m_targetFps > 0) {    //manual fps limiting
            float targetFrameTime = 1.0 / (float)m_targetFps;
            if (deltaTime < targetFrameTime) continue; 
        }

        lastFrame = currentFrame;

        processInput();
        update(deltaTime);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        render();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void Application::stop() {
    m_isRunning = false;
}

void Application::processInput() {
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        m_isRunning = false;
    }
}

void Application::update(float deltaTime) {
    // Logic updates will go here (e.g., scene updates)
}

void Application::render() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    onGui();
}

void Application::initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

void Application::shutdownImGui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Application::onGui() {
    ImGui::Begin("Performance");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Frame Time: %.3f ms/frame ", 1000.0f / ImGui::GetIO().Framerate);

    if (ImGui::Checkbox("VSync", &m_vSync)) {
        glfwSwapInterval(m_vSync ? 1 : 0);
    }

    if (!m_vSync) {
        ImGui::SliderInt("Target FPS", &m_targetFps, 30, 300);
    }

    ImGui::End();
}

void Application::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
