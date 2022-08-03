#pragma once

#include <vector>
#include <chrono>

#include "../Ash/EntityManager.h"
#include "../Ash/Entities.h"
#include "../GraphicEngine/Window.h"
#include "../GraphicEngine/Renderer.h"

namespace engine {

class Game {
public:
	const std::chrono::nanoseconds kTimestep = 
		std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::milliseconds(16));

	const float kDt = static_cast<float>(16) / 1000;

	class SystemBase {
	public:
		Game& GetGame();

		SystemBase();
		virtual ~SystemBase();

		virtual void Init(Game& game);		  
		virtual void Update(float dt);
		virtual void Render();
		virtual void Delete();

		template<typename... Args>
		ash::Filter<Args...> Filter() {
			return game_->GetEntities().Filter<Args...>();
		}

		ash::Entities& GetEntities() const {
			return game_->GetEntities();
		}

		graph::IWindow& GetWindow() const {
			return game_->GetWindow();
		}

		graph::Renderer& GetRenderer() const {
			return game_->GetRenderer();
		}

		graph::KeyboardState& GetKeyboardState() const {
			return game_->GetWindow().GetKeyboardState();
		}

		graph::MouseState& GetMouseState() const {
			return game_->GetWindow().GetMouseState();
		}

	private:
		Game* game_;
	};

	ash::Entities& GetEntities();
	graph::IWindow& GetWindow();
	graph::Renderer& GetRenderer();
	
	Game();
	~Game();

	void PushSystem(SystemBase& system);
	void Run();

private:
	ash::EntityManager entity_manager_;
	ash::Entities entities_ = { entity_manager_ };
	graph::Window window_;
	graph::Renderer renderer_;
	std::vector<SystemBase*> systems_;

	void Init();
	void Update();
	void Render();
	void Delete();
};

} // namespace engine