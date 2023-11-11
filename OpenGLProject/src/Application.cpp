#include "stdafx.h"
#include "Application.h"
#include "Debug.h"
#include "VertexArray.h"

/*
struct ColorVertex
{
    Vec2f position;
    float rotation;
    //Vec4f color;
};
*/

Application::Application()
	: m_window(/*1920, 1080,*/ "OpenGL Project", 0.03f, false)
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

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &m_maxTextureUnits);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#if ENABLE_ANTIALIASING
    glEnable(GL_MULTISAMPLE);
#endif

    m_textureShader = std::make_unique<Shader>("res/shaders/texture_shader.vert", "res/shaders/texture_shader.frag");
    m_shadowShader = std::make_unique<Shader>("res/shaders/shadow_shader.vert", "res/shaders/shadow_shader.frag");
    m_uiShader = std::make_unique<Shader>("res/shaders/ui_shader.vert", "res/shaders/ui_shader.frag");

    GLint* samplers = new GLint[m_maxTextureUnits];
    for (GLint i = 0; i < m_maxTextureUnits; i++)
        samplers[i] = i;
    m_textureShader->bind();
    m_textureShader->setUniform1iv("u_Textures", m_maxTextureUnits, samplers);
    m_shadowShader->bind();
    m_shadowShader->setUniform1iv("u_Textures", m_maxTextureUnits, samplers);
    m_uiShader->bind();
    m_uiShader->setUniform1iv("u_Textures", m_maxTextureUnits, samplers);
    delete[] samplers;

    m_rat1Texture = std::make_unique<Texture>("res/textures/rat1.png");
    m_ratPlayerTexture = std::make_unique<Texture>("res/textures/ratPlayer.png");
    m_testBlockTexture = std::make_unique<Texture>("res/textures/testBlock.png");
    m_testBlock2Texture = std::make_unique<Texture>("res/textures/testBlock2.png");
    m_testButtonTexture = std::make_unique<Texture>("res/textures/testButton.png");

    createMainMenu();
}

Application::~Application()
{
    delete m_currentScene;
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

    m_currentScene->onRender();

#if DEBUG_LINES
    for (auto& line : Line::s_debugLines)
    {
        const glm::mat4 mvp = m_window.getProj() * m_window.getView();
        line.setMVP(mvp);

        line.draw();
    }
#endif

    m_window.display();
}

void Application::createNewScene(const UpdateFunc& updateFunc)
{
    Scene::s_skip = true;

    delete m_currentScene;
    m_currentScene = new Scene(&m_window, updateFunc, m_maxTextureUnits, m_textureShader.get(), m_shadowShader.get(), m_uiShader.get());
}

void Application::createMainMenu()
{
    createNewScene();
    
    //m_currentScene->createUISprite(Vec2f{ 200.0f, -100.0f }, Vec2f{ 150.0f, 50.0f }, m_testButtonTexture.get(), UISprite::Left | UISprite::Up);
    ButtonPresses buttonPresses;
    buttonPresses.onPress = []() {
        std::cout << "pressed\n";
    };
    buttonPresses.onRelease = [this]() {
        std::cout << "released\n";
        createTestLevel();
    };
    buttonPresses.onHoverOver = []() {
        std::cout << "hovered over\n";
    };
    m_currentScene->createButton(Vec2f{ 0.0f, 0.0f }, Vec2f{ 150.0f, 50.0f }, m_testButtonTexture.get(), UISprite::Center, buttonPresses);
}

void Application::createTestLevel()
{
    createNewScene([](float deltaTime) {
        std::cerr << (1.0f / deltaTime) << '\n';
    });

    m_currentScene->createLight(Light{ Vec3f{ 1.0f, 1.0f, 0.2f }, Vec2f{ -16.0f, 0.0f }, 0.1f, .50f });
    m_currentScene->createLight(Light{ Vec3f{ 1.0f, 1.0f, 1.0f }, Vec2f{ -2.0f, 6.0f }, 0.1f, 3.0f });
    m_currentScene->createLight(Light{ Vec3f{ 1.0f, 1.0f, 1.0f }, Vec2f{ 9.0f, 1.0f }, 0.02f, 3.0f });
    m_currentScene->createLight(Light{ Vec3f{ 1.0f, 1.0f, 1.0f }, Vec2f{ 19.0f, 10.0f }, 0.1f, 2.0f });
    m_currentScene->createLight(Light{ Vec3f{ 1.0f, 1.0f, 1.0f }, Vec2f{ -12.0f, 33.0f }, 0.1f, 2.0f });
    m_currentScene->createLight(Light{ Vec3f{ 1.0f, 1.0f, 1.0f }, Vec2f{ 30.0f, 15.0f }, 0.1f, 2.0f });
    m_currentScene->createLight(Light{ Vec3f{ 1.0f, 1.0f, 1.0f }, Vec2f{ 40.0f, 9.0f }, 0.1f, 2.0f });
    m_currentScene->createLight(Light{ Vec3f{ 1.0f, 1.0f, 1.0f }, Vec2f{ -28.0f, -30.0f }, 0.1f, 2.0f });
    m_currentScene->createLight(Light{ Vec3f{ 1.0f, 1.0f, 1.0f }, Vec2f{ 26.0f, -20.0f }, 0.1f, 2.0f });

    m_currentScene->createTextureSpriteBack(Vec2f{ 0.0f, 0.0f }, Vec2f{ 100.0f, 100.0f }, 0.0f, m_testBlockTexture.get());
    m_currentScene->createTextureBlockBack(Vec2f{ 7.0f, 17.0f }, Vec2f{ 6.0f, 3.0f }, 0.0f, m_testBlock2Texture.get());
    m_currentScene->createTextureBlockBack(Vec2f{ -12.0f, 12.0f }, Vec2f{ 1.0f, 2.0f }, 0.2f, m_testBlock2Texture.get());
    m_currentScene->createTextureBlockBack(Vec2f{ 11.0f, 8.0f }, Vec2f{ 1.0f, 3.0f }, -0.1f, m_testBlock2Texture.get());
    m_currentScene->createTextureBlockBack(Vec2f{ 11.0f, 3.0f }, Vec2f{ 1.0f, 3.0f }, 0.1f, m_testBlock2Texture.get());
    m_currentScene->createTextureBlockBack(Vec2f{ -3.0f, -5.0f }, Vec2f{ 8.0f, 1.0f }, 0.0f, m_testBlock2Texture.get());
    m_currentScene->createPlayer(Vec2f{ 0.0f, 0.0f }, m_ratPlayerTexture.get());
    auto rat1 = m_currentScene->createEnemyRatSniffer(Vec2f{ -15.0f, 9.0f }, Vec2f{ 3.0f, 1.5f }, 0.0f, m_ratPlayerTexture.get());
    m_currentScene->addTargetPoint(rat1, Vec2f{ 20.0f, 0.0f });
    m_currentScene->addTargetPoint(rat1, Vec2f{ 18.0f, 20.0f });
    m_currentScene->addTargetPoint(rat1, Vec2f{ 6.0f, 6.0f });
    m_currentScene->createEnemyRatSniffer(Vec2f{ -20.0f, 9.0f }, Vec2f{ 3.0f, 1.5f }, 0.0f, m_ratPlayerTexture.get());
    m_currentScene->createEnemyRatSniffer(Vec2f{ 30.0f, -4.0f }, Vec2f{ 3.0f, 1.5f }, 0.0f, m_ratPlayerTexture.get());
}
