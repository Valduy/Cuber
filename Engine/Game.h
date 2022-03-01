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
		Game& GetGame() {
			return *game_;
		}

		SystemBase()
			: game_(nullptr)
		{}

		virtual ~SystemBase() {}

		virtual void Init(Game& game) {
			game_ = &game;
		}

		virtual void Update(float dt) {}
		virtual void Render() {}

		virtual void Delete() {
			game_ = nullptr;
		}

	private:
		Game* game_;
	};

	ecs::EntityManager& GetEntityManager() {
		return entity_manager_;
	}

	graph::Window& GetWindow() {
		return window_;
	}

	graph::Renderer& GetRenderer() {
		return renderer_;
	}

	void PushSystems(SystemBase& system) {
		systems_.push_back(&system);
	}

	Game()
		: window_(GetModuleHandle(nullptr), L"Game", 800, 800)
		, renderer_(window_)
	{}

	~Game() {
		for (const SystemBase* system : systems_) {
			delete system;
		}

		systems_.clear();
	}
	
	void Run() {
		Init();

		MSG msg = {};
		bool is_exit_requested = false;

		auto previous = std::chrono::steady_clock::now();
		long lag = 0;

		while (!is_exit_requested) {
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (msg.message == WM_QUIT) {
				is_exit_requested = true;
			}

			auto current = std::chrono::steady_clock::now();
			const long elapsed = std::chrono::duration_cast<std::chrono::microseconds>(current - previous).count();
			previous = current;
			lag += elapsed;

			while (lag >= kMsPerUpdate) {
				Update();
				lag -= kMsPerUpdate;
			}

			renderer_.BeginRender();
			Render();
			renderer_.EndRender();
		}


	}

private:
	ecs::EntityManager entity_manager_;
	graph::Window window_;
	graph::Renderer renderer_;
	std::vector<SystemBase*> systems_;

	void Init() {
		renderer_.Init();

		for (auto system : systems_) {
			system->Init(*this);
		}
	}

	void Update() {
		for (SystemBase* system : systems_) {
			system->Update(kDt);
		}
	}

	void Render() {
		for (SystemBase* system : systems_) {
			system->Render();
		}
	}

	void Delete() {
		for (SystemBase* system : systems_) {
			system->Delete();
		}
	}
};

} // namespace engine