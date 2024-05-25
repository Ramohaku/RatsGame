#include "stdafx.h"
#include "Application.h"
#include "Debug.h"
#include "VertexArray.h"

Application::Application()
	: m_window(/*1920, 1080,*/ "Rats game", 0.03f, true),
    m_screenVertices{
        { Vec2f{ -1.0f, -1.0f }, Vec2f{ 0.0f, 0.0f } },
        { Vec2f{  1.0f, -1.0f }, Vec2f{ 1.0f, 0.0f } },
        { Vec2f{  1.0f,  1.0f }, Vec2f{ 1.0f, 1.0f } },
        { Vec2f{ -1.0f,  1.0f }, Vec2f{ 0.0f, 1.0f } },
    }
{
    if (glewInit() != GLEW_OK)
    {
        ASSERT(false);
        return;
    }

#ifdef _DEBUG
    Debug::enableOpenGLDebugging();
    std::cout << glGetString(GL_VERSION) << '\n';
#endif

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_appSceneData.maxTextureUnits);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#if ENABLE_ANTIALIASING
    glEnable(GL_MULTISAMPLE);
#endif

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplGlfw_InitForOpenGL(m_window.getGlfwWindow(), true);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();

    m_appSceneData.textureShader = std::make_unique<Shader>("res/shaders/texture_shader.vert", "res/shaders/texture_shader.frag");
    m_appSceneData.textureEditorShader = std::make_unique<Shader>("res/shaders/texture_shader.vert", "res/shaders/texture_editor_shader.frag");
    m_appSceneData.shadowShader = std::make_unique<Shader>("res/shaders/shadow_shader.vert", "res/shaders/shadow_shader.frag");
    m_appSceneData.uiShader = std::make_unique<Shader>("res/shaders/ui_shader.vert", "res/shaders/ui_shader.frag");

    GLint* samplers = new GLint[m_appSceneData.maxTextureUnits];
    for (GLint i = 0; i < m_appSceneData.maxTextureUnits; i++)
        samplers[i] = i;
    m_appSceneData.textureShader->bind();
    m_appSceneData.textureShader->setUniform1iv("u_Textures", m_appSceneData.maxTextureUnits, samplers);
    m_appSceneData.textureEditorShader->bind();
    m_appSceneData.textureEditorShader->setUniform1iv("u_Textures", m_appSceneData.maxTextureUnits, samplers);
    m_appSceneData.textureEditorShader->setUniform1i("u_AllLight", 0);
    m_appSceneData.shadowShader->bind();
    m_appSceneData.shadowShader->setUniform1iv("u_Textures", m_appSceneData.maxTextureUnits, samplers);
    m_appSceneData.uiShader->bind();
    m_appSceneData.uiShader->setUniform1iv("u_Textures", m_appSceneData.maxTextureUnits, samplers);
    delete[] samplers;

    createTexture("Rat1", "res/textures/rat1.png");
    createTexture("Player", "res/textures/ratPlayer.png");
    createTexture("AllRats", "res/textures/allrats.png");
    createTexture("TestBlock", "res/textures/testBlock.png");
    createTexture("TestBlock2", "res/textures/testBlock2.png");
    createTexture("TestBlock3", "res/textures/testBlock3.png");
    createTexture("TestButton", "res/textures/testButton.png");
    createTexture("TestColor", "res/textures/color.png");
    createTexture("Bg1", "res/textures/bg1.png");
    createTexture("Lightholder", "res/textures/lightholder.png");
    createTexture("Level1_2_floor", "res/textures/level1_2_floor.png");
    createTexture("FastDoor", "res/textures/fastDoor.png");
    createTexture("Level1_wall1", "res/textures/level1_wall1.png");
    /*m_rat1Texture = std::make_unique<Texture>("res/textures/rat1.png");
    m_ratPlayerTexture = std::make_unique<Texture>("res/textures/ratPlayer.png");
    m_testBlockTexture = std::make_unique<Texture>("res/textures/testBlock.png");
    m_testBlock2Texture = std::make_unique<Texture>("res/textures/testBlock2.png");
    m_testButtonTexture = std::make_unique<Texture>("res/textures/testButton.png");*/


    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // generate texture
    glGenTextures(1, &m_screenTexture);
    glBindTexture(GL_TEXTURE_2D, m_screenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_window.getWidth(), m_window.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    // attach it to currently bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_screenTexture, 0);

    //unsigned int rbo;
    //glGenRenderbuffers(1, &rbo);
    //glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_window.getWidth(), m_window.getHeight());
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);

    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        ASSERT(false);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_screenShader = std::make_unique<Shader>("res/shaders/screen_shader.vert", "res/shaders/screen_shader.frag");
    m_screenShader->bind();
    m_screenShader->setUniform1f("u_ScreenScale", static_cast<float>(m_window.getWidth()) / static_cast<float>(m_window.getHeight()));
    
    m_screenVertexArray = std::make_unique<VertexArray<ScreenVertex>>(4);

    m_appSceneData.textureVertexArraysBack.emplace_back(4);
    m_appSceneData.shadowVertexArraysMiddle.emplace_back(4);
    m_appSceneData.textureVertexArraysMiddle.emplace_back(4);
    m_appSceneData.shadowVertexArraysFront.emplace_back(4);
    m_appSceneData.textureVertexArraysFront.emplace_back(4);
    m_appSceneData.uiVertexArrays.emplace_back(4);

    createMainMenu();

    m_soundDevice = alcOpenDevice(nullptr);
    if (!m_soundDevice)
    {
        std::cerr << "Cannot open OpenAL device\n";
        return;
    }

    m_soundContext = alcCreateContext(m_soundDevice, nullptr);
    alcMakeContextCurrent(m_soundContext);

    using namespace std::string_literals;
    for (int i = 0; i < RAT_SOUNDS_COUNT; i++)
    {
        const auto iStr = std::to_string(i + 1);
        m_appSceneData.soundBuffers["RatSound"s + iStr].loadFromFile(("res/audio/ratSound"s + iStr + ".wav").c_str());
    }
}

Application::~Application()
{
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(m_soundContext);
    alcCloseDevice(m_soundDevice);
}

void Application::run()
{
    using namespace std::literals::chrono_literals;
    while (m_window.isOpen())
    {
        auto frameStartTime = std::chrono::high_resolution_clock::now();

        update();
        render();

        glfwPollEvents();

        auto frameEndTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> frameDurationMs = frameEndTime - frameStartTime;
        //const float desiredFrameDurationMs = FRAME_TIME_MS;
        //if (frameDurationMs.count() < FRAME_TIME_MS)
        //{
        //    std::chrono::milliseconds sleepDurationMs((long long)(desiredFrameDurationMs - frameDurationMs.count()));
        //    std::this_thread::sleep_for(sleepDurationMs);
        //
        //    frameEndTime = std::chrono::high_resolution_clock::now();
        //    frameDurationMs = frameEndTime - frameStartTime;
        //}

        m_deltaTime = frameDurationMs.count();

        //const float time = 1.0f / m_deltaTime;
        //std::cout << time << '\n';
    }

    clean();
}

void Application::update()
{
#if DEBUG_LINES
    Line::s_debugLines.clear();
#endif

    m_currentScene->onUpdate(m_deltaTime);
}

void Application::render()
{
    m_window.clear();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_currentScene->onRender();

#if DEBUG_LINES
    for (auto& line : Line::s_debugLines)
    {
        const glm::mat4 mvp = m_window.getProj() * m_window.getView();
        line.setMVP(mvp);

        line.draw();
    }
#endif

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_screenShader->bind();
    //if (m_currentScene && m_currentScene->getPlayer())
    //    m_screenShader->setUniform1f("u_PlayerAngle", m_currentScene->getPlayer()->getRotation());
    m_screenVertexArray->clear();
    m_screenVertexArray->addGeometryPiece(m_screenVertices);
    m_screenVertexArray->createSubData();
    m_screenVertexArray->bind();
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, m_screenTexture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    {
        ImGui::Begin("Hello, world!");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    m_window.display();
}

void Application::clean()
{
    for (auto& va : m_appSceneData.textureVertexArraysBack)
        va.onClean();
    for (auto& va : m_appSceneData.shadowVertexArraysMiddle)
        va.onClean();
    for (auto& va : m_appSceneData.textureVertexArraysMiddle)
        va.onClean();
    for (auto& va : m_appSceneData.shadowVertexArraysFront)
        va.onClean();
    for (auto& va : m_appSceneData.textureVertexArraysFront)
        va.onClean();
    for (auto& va : m_appSceneData.uiVertexArrays)
        va.onClean();

    m_appSceneData.textureShader->onClean();
    m_appSceneData.textureEditorShader->onClean();
    m_appSceneData.shadowShader->onClean();
    m_appSceneData.uiShader->onClean();

    m_screenShader->onClean();
    m_screenVertexArray->onClean();

    for (auto& [name, texture] : m_appSceneData.textures)
        texture->onClean();

    delete m_currentScene;
}

void Application::createNewScene()
{
    Scene::s_skip = true;

    delete m_currentScene;
    m_currentScene = new Scene(&m_window, m_appSceneData);
}

void Application::createMainMenu()
{
    createNewScene();
    
    //m_currentScene->createUISprite(Vec2f{ 200.0f, -100.0f }, Vec2f{ 150.0f, 50.0f }, m_testButtonTexture.get(), UISprite::Left | UISprite::Up);
    ButtonPresses testLevelButtonPresses;
    testLevelButtonPresses.onPress = [] {
        std::cout << "pressed\n";
    };
    testLevelButtonPresses.onRelease = [this] {
        std::cout << "released\n";
        createLevel1();
    };
    testLevelButtonPresses.onHoverOver = [] {
        std::cout << "hovered over\n";
    };
    m_currentScene->createButton(Vec2f{ 0.0f, 0.0f }, Vec2f{ 150.0f, 50.0f }, m_appSceneData.textures.at("TestButton").get(), UISprite::Center, testLevelButtonPresses);

    ButtonPresses editorLevelButtonPresses;
    editorLevelButtonPresses.onRelease = [this] {
        createEditorScene();
    };
    m_currentScene->createButton(Vec2f{ 0.0f, 200.0f }, Vec2f{ 150.0f, 50.0f }, m_appSceneData.textures.at("TestButton").get(), UISprite::Center, editorLevelButtonPresses);
}

void Application::createEditorScene()
{
    Scene::s_skip = true;

    delete m_currentScene;
    m_currentScene = new EditorScene(&m_window, m_appSceneData);
}

void Application::createLevel1()
{
    Scene::s_skip = true;

    delete m_currentScene;
    m_currentScene = new Level1(&m_window, m_appSceneData, [this]() {
        createLevel2();
    });
}

void Application::createLevel2()
{
    Scene::s_skip = true;

    SpriteData playerSpriteData = m_currentScene->getPlayer()->getSpriteData();
    playerSpriteData.center.x += 49.0f;
    playerSpriteData.center.y += 1.5f;
    float destRotation = m_currentScene->getPlayer()->getDestRotation();

    delete m_currentScene;
    m_currentScene = new Level2(&m_window, m_appSceneData, [this]() {
        
    });
    m_currentScene->getPlayer()->setSpriteData(playerSpriteData);
    m_currentScene->getPlayer()->setDestRotation(destRotation);
}

void Application::createTexture(std::string&& name, std::string&& filepath)
{
    m_appSceneData.textures.insert({ name, std::make_unique<Texture>(filepath) });
}
