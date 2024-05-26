#include "stdafx.h"
#include "EditorScene.h"

using json = nlohmann::json;

EditorScene::EditorScene(Window* window, AppSceneData& appSceneData)
	: Scene(window, appSceneData, true),
	m_textureNames{ "TextureSprite", "Player", "EnemyRatWatcher", "EnemyRatSniffer" }
	//m_textures(textures)
{
	m_rangeOptimize = false;
}

void EditorScene::onUpdate(float deltaTime)
{
	bool escPressed = glfwGetKey(m_window->getGlfwWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS;

	if (m_paused)
	{
		if (escPressed && !m_escPrePressed)
		{
			onPauseEnd();
			m_paused = false;
		}
	}
	else
	{
		//if (m_player)
		//{
		//	m_player->onUpdate(deltaTime);
		//	m_window->updateView(m_player->getCenter(), m_player->getRotation() + PI_F / 2.0f);
		//}

		double mouseX, mouseY;
		glfwGetCursorPos(m_window->getGlfwWindow(), &mouseX, &mouseY);


		constexpr double moveSpeed = 20.0;
		constexpr double moveDist = 10.0;

		if (mouseX < moveDist)
		{
			m_viewCenter.x -= moveSpeed * deltaTime;
		}
		else if (mouseX > m_window->getWidth() - moveDist)
		{
			m_viewCenter.x += moveSpeed * deltaTime;
		}

		if (mouseY < moveDist)
		{
			m_viewCenter.y += moveSpeed * deltaTime;
		}
		else if (mouseY > m_window->getHeight() - moveDist)
		{
			m_viewCenter.y -= moveSpeed * deltaTime;
		}

		m_window->updateView(m_viewCenter, 0.0f);

		const int mouseState = glfwGetMouseButton(m_window->getGlfwWindow(), GLFW_MOUSE_BUTTON_LEFT);

		if (mouseState == GLFW_PRESS)
		{
			mouseX = (mouseX - m_window->getWidth() / 2.0f) * m_window->getScale() + m_viewCenter.x;
			mouseY = -(mouseY - m_window->getHeight() / 2.0f) * m_window->getScale() + m_viewCenter.y;
			
			std::cerr << mouseX << ", " << mouseY << '\n';
		}




		if (escPressed && !m_escPrePressed)
		{
			onPauseStart();
			m_paused = true;
		}
	}
	
	m_escPrePressed = escPressed;

	updateLights(m_appSceneData.textureEditorShader);
}

void EditorScene::onRender()
{
	renderSprites(m_appSceneData.textureVertexArraysBack, m_textureSpritesBack, m_appSceneData.textureEditorShader.get());
	renderSprites(m_appSceneData.shadowVertexArraysMiddle, m_shadowSpritesMiddle, m_appSceneData.shadowShader.get());
	renderSprites(m_appSceneData.textureVertexArraysMiddle, m_textureSpritesMiddle, m_appSceneData.textureEditorShader.get());
	renderSprites(m_appSceneData.shadowVertexArraysFront, m_shadowSpritesFront, m_appSceneData.shadowShader.get());
	renderSprites(m_appSceneData.textureVertexArraysFront, m_textureSpritesFront, m_appSceneData.textureEditorShader.get());
	renderSprites(m_appSceneData.uiVertexArrays, m_uiSprites, m_appSceneData.uiShader.get());

	ImGui::Begin("Lights");

	{
		int i = 0;
		for (auto& light : m_lights)
		{
			std::string label = std::string("Light ") + std::to_string(i);
			ImGui::Text(label.c_str());
			//ImGui::DragFloat3((label + " color: ").c_str(), (float*)&light.color, 0.01f, 0.0f, 1.0f);
			ImGui::ColorEdit3((label + " color: ").c_str(), (float*)&light.color);
			ImGui::DragFloat2((label + " center: ").c_str(), (float*)&light.center, 0.1f);
			ImGui::DragFloat((label + " vanish: ").c_str(), &light.vanish, 0.001f, 0.0f, 2.0f);
			ImGui::DragFloat((label + " distance: ").c_str(), &light.distance, 0.01f, 0.0f, 20.0f);
			ImGui::Checkbox((label + " active: ").c_str(), &light.active);

			if (ImGui::Button((std::string("Delete light ") + label).c_str()))
			{
				m_lights.erase(m_lights.begin() + i);
			}
			i++;
		}


		if (ImGui::Button("New light"))
		{
			m_lights.push_back(Light{ Vec3f{ 1.0f, 1.0f, 1.0f }, Vec2f{ 0.0f, 0.0f }, 0.1f, 2.0f });
		}
	}

	ImGui::End();

	ImGui::Begin("Entities");

	ImGui::Text("------------------ Back sprites ------------------");
	createImGuiEntities(m_textureSpritesBack, m_extraDataBack, "Back Sprite", m_texturesBackBuffer);
	ImGui::Text("------------------ Middle sprites ------------------");
	createImGuiEntities(m_textureSpritesMiddle, m_extraDataMiddle, "Middle Sprite", m_texturesMiddleBuffer);
	ImGui::Text("------------------ Front sprites ------------------");
	createImGuiEntities(m_textureSpritesFront, m_extraDataFront, "Front Sprite", m_texturesFrontBuffer);

	ImGui::End();

	ImGui::Begin("Options");

	static char fileName[40] = "res/scenesData/level1.json";
	ImGui::InputText("File path", fileName, 40);

	if (ImGui::Button("Import"))
	{
		std::ifstream file(fileName);
		if (file)
		{
			json j;
			file >> j;

			if (j["sceneData"].contains("windowScale"))
				m_windowScale = j["sceneData"]["windowScale"].get<double, float>();

			const auto& lightsData = j["sceneData"]["lights"].get<std::vector<json>>();
			const auto& entitiesData = j["sceneData"]["entities"].get<std::vector<json>>();

			for (const auto& data : lightsData)
			{
				Light light;
				light.color.r = data["color"]["r"].get<double, float>();
				light.color.g = data["color"]["g"].get<double, float>();
				light.color.b = data["color"]["b"].get<double, float>();
				light.center.x = data["center"]["x"].get<double, float>();
				light.center.y = data["center"]["y"].get<double, float>();
				light.vanish = data["vanish"].get<double, float>();
				light.distance = data["distance"].get<double, float>();
				light.active = data["active"].get<bool>();
				m_lights.push_back(light);
			}

			for (const auto& data : entitiesData)
			{
				SpriteData spriteData;
				spriteData.center.x = data["center"]["x"].get<double, float>();
				spriteData.center.y = data["center"]["y"].get<double, float>();
				spriteData.halfSize.x = data["halfSize"]["x"].get<double, float>();
				spriteData.halfSize.y = data["halfSize"]["y"].get<double, float>();
				spriteData.rotation = data["rotation"].get<double, float>();
				spriteData.texturePtr = m_appSceneData.textures.at(data["textureName"].get<std::string>()).get();
				spriteData.texPartScale.x = data["texPartScale"]["x"].get<double, float>();
				spriteData.texPartScale.y = data["texPartScale"]["y"].get<double, float>();
				spriteData.texPartIndex.x = data["texPartIndex"]["x"].get<double, float>();
				spriteData.texPartIndex.y = data["texPartIndex"]["y"].get<double, float>();
				spriteData.flipHorizontal = data["flipHorizontal"].get<double, float>();
				spriteData.flipVertical = data["flipVertical"].get<double, float>();
				
				std::unique_ptr<TextureSprite> sprite;
				
				const EntityType entityType = data["entityType"].get<EntityType>();
				switch (entityType)
				{
				case EntityType::Player:
				{
					std::unique_ptr<Player> player = std::make_unique<Player>(spriteData, m_window);
					m_player = player.get();
					sprite = std::move(player);
					break;
				}
				default:
					sprite = std::make_unique<TextureSprite>(spriteData);
					break;
				}

				ExtraData ed;
				std::string strName = data["name"].get<std::string>();
				memcpy(ed.name, strName.c_str(), 20 * sizeof(char));
				ed.entityType = entityType;
				ed.textureName = data["textureName"].get<std::string>();
				ed.shadows = data["shadows"].get<bool>();
				ed.blocking = data["blocking"].get<bool>();

				switch (data["layer"].get<int>())
				{
				case 0:
					m_textureSpritesBack.push_back(std::move(sprite));
					m_extraDataBack.push_back(ed);
					break;
				case 1:
					m_textureSpritesMiddle.push_back(std::move(sprite));
					m_extraDataMiddle.push_back(ed);
					break;
				case 2:
					m_textureSpritesFront.push_back(std::move(sprite));
					m_extraDataFront.push_back(ed);
					break;
				}
			}
			file.close();
		}
		else
		{
			std::cerr << "could not open file " << fileName << '\n';
		}
	}

	if (ImGui::Button("Export"))
	{
		std::ofstream file(fileName);
		if (file)
		{
			json j;

			std::vector<json> lightsArray;
			for (const auto& light : m_lights)
			{
				json& back = lightsArray.emplace_back();
				back["color"]["r"] = light.color.r;
				back["color"]["g"] = light.color.g;
				back["color"]["b"] = light.color.b;
				back["center"]["x"] = light.center.x;
				back["center"]["y"] = light.center.y;
				back["vanish"] = light.vanish;
				back["distance"] = light.distance;
				back["active"] = light.active;
			}
			j["sceneData"]["lights"] = lightsArray;

			std::vector<json> entitiesArray;
			int i = 0;
			for (const auto& sprite : m_textureSpritesBack)
			{
				ExtraData ed = m_extraDataBack[i];

				json& back = entitiesArray.emplace_back();
				back["layer"] = 0;
				back["name"] = ed.name;
				back["entityType"] = static_cast<int>(ed.entityType);
				back["shadows"] = ed.shadows;
				back["blocking"] = ed.blocking;
				back["center"]["x"] = sprite->getCenter().x;
				back["center"]["y"] = sprite->getCenter().y;
				back["halfSize"]["x"] = sprite->getHalfSize().x;
				back["halfSize"]["y"] = sprite->getHalfSize().y;
				back["rotation"] = sprite->getRotation();
				back["textureName"] = ed.textureName;
				back["texPartScale"]["x"] = sprite->getTexPartScale().x;
				back["texPartScale"]["y"] = sprite->getTexPartScale().y;
				back["texPartIndex"]["x"] = sprite->getTexPartIndex().x;
				back["texPartIndex"]["y"] = sprite->getTexPartIndex().y;
				back["flipHorizontal"] = sprite->getFlipHorizontal();
				back["flipVertical"] = sprite->getFlipVertical();

				i++;
			}
			i = 0;
			for (const auto& sprite : m_textureSpritesMiddle)
			{
				ExtraData ed = m_extraDataMiddle[i];

				json& back = entitiesArray.emplace_back();
				back["layer"] = 1;
				back["name"] = ed.name;
				back["entityType"] = static_cast<int>(ed.entityType);
				back["shadows"] = ed.shadows;
				back["blocking"] = ed.blocking;
				back["center"]["x"] = sprite->getCenter().x;
				back["center"]["y"] = sprite->getCenter().y;
				back["halfSize"]["x"] = sprite->getHalfSize().x;
				back["halfSize"]["y"] = sprite->getHalfSize().y;
				back["rotation"] = sprite->getRotation();
				back["textureName"] = ed.textureName;
				back["texPartScale"]["x"] = sprite->getTexPartScale().x;
				back["texPartScale"]["y"] = sprite->getTexPartScale().y;
				back["texPartIndex"]["x"] = sprite->getTexPartIndex().x;
				back["texPartIndex"]["y"] = sprite->getTexPartIndex().y;
				back["flipHorizontal"] = sprite->getFlipHorizontal();
				back["flipVertical"] = sprite->getFlipVertical();

				i++;
			}
			i = 0;
			for (const auto& sprite : m_textureSpritesFront)
			{
				ExtraData ed = m_extraDataFront[i];

				json& back = entitiesArray.emplace_back();
				back["layer"] = 2;
				back["name"] = ed.name;
				back["entityType"] = static_cast<int>(ed.entityType);
				back["shadows"] = ed.shadows;
				back["blocking"] = ed.blocking;
				back["center"]["x"] = sprite->getCenter().x;
				back["center"]["y"] = sprite->getCenter().y;
				back["halfSize"]["x"] = sprite->getHalfSize().x;
				back["halfSize"]["y"] = sprite->getHalfSize().y;
				back["rotation"] = sprite->getRotation();
				back["textureName"] = ed.textureName;
				back["texPartScale"]["x"] = sprite->getTexPartScale().x;
				back["texPartScale"]["y"] = sprite->getTexPartScale().y;
				back["texPartIndex"]["x"] = sprite->getTexPartIndex().x;
				back["texPartIndex"]["y"] = sprite->getTexPartIndex().y;
				back["flipHorizontal"] = sprite->getFlipHorizontal();
				back["flipVertical"] = sprite->getFlipVertical();

				i++;
			}
			j["sceneData"]["entities"] = entitiesArray;
			j["sceneData"]["windowScale"] = m_windowScale;

			file << std::setw(4) << j;
			file.close();
		}
		else
		{
			std::cerr << "could not create file " << fileName << '\n';
		}

	}
	ImGui::SliderFloat("Window scale", &m_windowScale, 0.0f, 0.1f);
	m_window->setScale(m_windowScale);

	double mouseX, mouseY;
	glfwGetCursorPos(m_window->getGlfwWindow(), &mouseX, &mouseY);

	mouseX = (mouseX - m_window->getWidth() / 2.0f) * m_window->getScale() + m_viewCenter.x;
	mouseY = -(mouseY - m_window->getHeight() / 2.0f) * m_window->getScale() + m_viewCenter.y;
	
	ImGui::Text("x: %f, y: %f", mouseX, mouseY);

	if (ImGui::Button("Toggle all black"))
	{
		m_allLight = !m_allLight;
		m_appSceneData.textureEditorShader->bind();
		m_appSceneData.textureEditorShader->setUniform1i("u_AllLight", static_cast<int>(m_allLight));
	}

	ImGui::End();
}

void EditorScene::createImGuiEntities(std::vector<std::unique_ptr<TextureSprite>>& sprites, std::vector<ExtraData>& extraData, const std::string labelName, char* buffer)
{
	for (int i = 0; i < sprites.size(); i++)
	{
		auto& entity = sprites[i];

		std::string label = labelName + ' ' + std::to_string(i);
		if (ImGui::Button(label.c_str()))
		{
			if (m_selectedTextureSprite == entity.get())
			{
				m_selectedTextureSprite = nullptr;
			}
			else
			{
				m_selectedTextureSprite = entity.get();
			}
		}

		if (m_selectedTextureSprite != entity.get())
		{
			continue;
		}

		ImGui::InputText((label + " name: ").c_str(), extraData[i].name, 20);

		auto& spriteData = entity->getSpriteData();
		ImGui::DragFloat2((label + " center: ").c_str(), (float*)&spriteData.center, 0.1f);
		ImGui::DragFloat2((label + " halfSize: ").c_str(), (float*)&spriteData.halfSize, 0.1f);
		ImGui::DragFloat((label + " rotation: ").c_str(), &spriteData.rotation, 0.005f, -PI_F, PI_F);
		ImGui::DragFloat2((label + " texPartScale: ").c_str(), (float*)&spriteData.texPartScale, 0.1f);
		ImGui::DragFloat2((label + " texPartIndex: ").c_str(), (float*)&spriteData.texPartIndex, 0.1f);
		ImGui::DragFloat((label + " flipHorizontal: ").c_str(), &spriteData.flipHorizontal, 0.1f, -1.0f, 1.0f);
		ImGui::DragFloat((label + " flipVertical: ").c_str(), &spriteData.flipVertical, 0.1f, -1.0f, 1.0f);
		
		if (ImGui::Combo((label + " type").c_str(), reinterpret_cast<int*>(&extraData[i].entityType), m_textureNames, IM_ARRAYSIZE(m_textureNames)))
		{
			switch (extraData[i].entityType)
			{
			case EntityType::Player:
				spriteData.halfSize = Vec2f{ 2.0f, 1.0f };
				spriteData.texPartScale = Vec2f{ 1.0f / 10.0f, 1.0f / 10.0f };
				spriteData.texPartIndex = Vec2f{ 0.0f, 0.0f };
				break;
			case EntityType::EnemyRatWatcher:
			case EntityType::EnemyRatSniffer:
				spriteData.texPartScale = Vec2f{ 1.0f / 10.0f, 1.0f / 10.0f };
				spriteData.texPartIndex = Vec2f{ 0.0f, 1.0f };
				extraData[i].blocking = true;
				break;
			}
		}

		ImGui::Checkbox((label + " shadows: ").c_str(), &extraData[i].shadows);
		ImGui::Checkbox((label + " blocking: ").c_str(), &extraData[i].blocking);

		if (ImGui::Button((std::string("delete ") + label).c_str()))
		{
			sprites.erase(sprites.begin() + i);
			extraData.erase(extraData.begin() + i);
			break; // for safety
		}
	}

	ImGui::Text((std::string("New ") + labelName).c_str());


	ImGui::InputText((labelName + " name: ").c_str(), buffer, 30);

	if (ImGui::Button((std::string("Insert ") + labelName).c_str()))
	{
		std::string textureName = buffer;
		std::cout << textureName << '\n';

		if (m_appSceneData.textures.find(textureName) != m_appSceneData.textures.end())
		{
			SpriteData spriteData;
			spriteData.texturePtr = m_appSceneData.textures.at(textureName).get();

			auto entity = std::make_unique<TextureSprite>(spriteData);
			sprites.push_back(std::move(entity));
			ExtraData ed;
			ed.textureName = textureName;
			extraData.push_back(ed);
		}
		else
		{
			std::cerr << "Cannot find texture!\n";
		}
	}
}
