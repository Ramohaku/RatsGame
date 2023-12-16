#include "stdafx.h"
#include "EditorScene.h"

using json = nlohmann::json;

EditorScene::EditorScene(Window* window, std::unordered_map<std::string, std::unique_ptr<Texture>>& textures, /*const UpdateFunc& updateFunc,*/ GLint maxTextureUnits, Shader* textureShader, Shader* shadowShader, Shader* uiShader)
	: Scene(window, textures/*updateFunc*/, maxTextureUnits, textureShader, shadowShader, uiShader),
	m_textureNames{ "TextureSprite", "Player", "EnemyRatGuard", "EnemyRatSniffer" }
	//m_textures(textures)
{
	//createPlayer(Vec2f{ 0.0f, 0.0f }, m_textures.at("Player").get());
	//m_currentItemsFront.push_back(1); // player
}

void EditorScene::onUpdate(float deltaTime)
{
	if (m_player)
	{
		m_player->onUpdate(deltaTime);
		m_window->setCenter(m_player->getCenter());
	}

	updateLights();
}

void EditorScene::onRender()
{
	Scene::onRender();

	ImGui::Begin("Lights");

	{
		int i = 0;
		for (auto& light : m_lights)
		{
			std::string label = std::string("Light ") + std::to_string(i++);
			ImGui::Text(label.c_str());
			//ImGui::DragFloat3((label + " color: ").c_str(), (float*)&light.color, 0.01f, 0.0f, 1.0f);
			ImGui::ColorEdit3((label + " color: ").c_str(), (float*)&light.color);
			ImGui::DragFloat2((label + " center: ").c_str(), (float*)&light.center, 0.1f);
			ImGui::DragFloat((label + " vanish: ").c_str(), &light.vanish, 0.001f, 0.0f, 2.0f);
			ImGui::DragFloat((label + " distance: ").c_str(), &light.distance, 0.01f, 0.0f, 20.0f);
			ImGui::Checkbox((label + " active: ").c_str(), &light.active);
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
				spriteData.texturePtr = m_textures.at(data["textureName"].get<std::string>()).get();
				spriteData.texPartScale.x = data["texPartScale"]["x"].get<double, float>();
				spriteData.texPartScale.y = data["texPartScale"]["y"].get<double, float>();
				spriteData.texPartIndex.x = data["texPartIndex"]["x"].get<double, float>();
				spriteData.texPartIndex.y = data["texPartIndex"]["y"].get<double, float>();
				spriteData.flipHorizontal = data["flipHorizontal"].get<double, float>();
				spriteData.flipVertical = data["flipVertical"].get<double, float>();
				
				std::unique_ptr<TextureSprite> sprite;
				
				const int entityType = data["entityType"].get<int>();
				switch (entityType)
				{
				case 1:
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
				back["entityType"] = ed.entityType;
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
				back["entityType"] = ed.entityType;
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
				back["entityType"] = ed.entityType;
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

	ImGui::End();
}

void EditorScene::createImGuiEntities(std::vector<std::unique_ptr<TextureSprite>>& sprites, std::vector<ExtraData>& extraData, const std::string labelName, char* buffer)
{
	int i = 0;
	for (auto& entity : sprites)
	{
		std::string label = labelName + ' ' + std::to_string(i);
		ImGui::Text(label.c_str());

		auto& spriteData = entity->getSpriteData();
		ImGui::DragFloat2((label + " center: ").c_str(), (float*)&spriteData.center, 0.1f);
		ImGui::DragFloat2((label + " halfSize: ").c_str(), (float*)&spriteData.halfSize, 0.1f);
		ImGui::DragFloat((label + " rotation: ").c_str(), &spriteData.rotation, 0.005f, -PI_F, PI_F);
		ImGui::DragFloat2((label + " texPartScale: ").c_str(), (float*)&spriteData.texPartScale, 0.1f);
		ImGui::DragFloat2((label + " texPartIndex: ").c_str(), (float*)&spriteData.texPartIndex, 0.1f);
		ImGui::DragFloat((label + " flipHorizontal: ").c_str(), &spriteData.flipHorizontal, 0.1f, -1.0f, 1.0f);
		ImGui::DragFloat((label + " flipVertical: ").c_str(), &spriteData.flipVertical, 0.1f, -1.0f, 1.0f);
		
		if (ImGui::Combo((label + " type").c_str(), &extraData[i].entityType, m_textureNames, IM_ARRAYSIZE(m_textureNames)))
		{
			switch (extraData[i].entityType)
			{
			case 1: // Player
				spriteData.halfSize = Vec2f{ 2.0f, 1.0f };
				spriteData.texPartScale = Vec2f{ 1.0f / 2.0f, 1.0f / 10.0f };
				spriteData.texPartIndex = Vec2f{ 1.0f, 9.0f };
				break;
			case 2: // EnemyRatGuard
			case 3: // EnemyRatSniffer
				spriteData.texPartScale = Vec2f{ 1.0f / 2.0f, 1.0f / 10.0f };
				spriteData.texPartIndex = Vec2f{ 1.0f, 9.0f };
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

		i++;
	}

	ImGui::Text((std::string("New ") + labelName).c_str());


	ImGui::InputText((labelName + " name: ").c_str(), buffer, 30);

	if (ImGui::Button((std::string("Insert ") + labelName).c_str()))
	{
		std::string textureName = buffer;
		std::cout << textureName << '\n';

		if (m_textures.find(textureName) != m_textures.end())
		{
			SpriteData spriteData;
			spriteData.texturePtr = m_textures.at(textureName).get();

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
