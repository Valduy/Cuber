#pragma once

#include <vector>
#include <chrono>

#include "../ECS/EntityManager.h"
#include "../GraphicEngine/Window.h"
#include "../GraphicEngine/Renderer.h"

namespace engine {

class Game {
public:
	const long kFps = 60;
	const long kMsPerUpdate = 1000 / 60;
	const float kDt = static_cast<float>(kMsPerUpdate) / 1000;

	class SystemBase {
	public:
		Game& GetGame();

		SystemBase();
		virtual ~SystemBase();

		virtual void Init(Game& game);		  
		virtual void Update(float dt);
		virtual void Render();
		virtual void Delete();

	private:
		Game* game_;
	};

	ecs::EntityManager& GetEntityManager();
	graph::Window& GetWindow();
	graph::Renderer& GetRenderer();
	
	Game();
	~Game();

	void PushSystem(SystemBase& system);
	void Run();

private:
	ecs::EntityManager entity_manager_;
	graph::Window window_;
	graph::Renderer renderer_;
	std::vector<SystemBase*> systems_;

	void Init();
	void Update();
	void Render();
	void Delete();
};

} // namespace engine