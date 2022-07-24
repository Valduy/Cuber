#include "pch.h"
#include "Game.h"

engine::Game& engine::Game::SystemBase::GetGame() {
	return *game_;
}

engine::Game::SystemBase::SystemBase()
	: game_(nullptr)
{}

engine::Game::SystemBase::~SystemBase() = default;

void engine::Game::SystemBase::Init(Game& game) {
	game_ = &game;
}

void engine::Game::SystemBase::Update(float dt)
{}

void engine::Game::SystemBase::Render()
{}

void engine::Game::SystemBase::Delete() {
	game_ = nullptr;
}

ash::Entities& engine::Game::GetEntities() {
	return entities_;
}

graph::Window& engine::Game::GetWindow() {
	return window_;
}

graph::Renderer& engine::Game::GetRenderer() {
	return renderer_;
}

engine::Game::Game()
	: window_(GetModuleHandle(nullptr), L"Game", 800, 800)
    , renderer_(window_)
{}

engine::Game::~Game() {
	systems_.clear();
}

void engine::Game::PushSystem(SystemBase& system) {
	systems_.push_back(&system);
}

void engine::Game::Run() {
	Init();

	MSG msg = {};
	bool is_exit_requested = false;

	using namespace std::chrono;
	nanoseconds lag(0ns);
	auto time_start = high_resolution_clock::now();

	while (!is_exit_requested) {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) {
				is_exit_requested = true;
			}
		}

		auto delta_time = high_resolution_clock::now() - time_start;
		time_start = high_resolution_clock::now();
		lag += duration_cast<nanoseconds>(delta_time);

		while (lag >= kTimestep) {
			lag -= kTimestep;
			Update();
		}

		Render();
	}
}

void engine::Game::Init() {
	renderer_.Init();

	for (auto system : systems_) {
		system->Init(*this);
	}
}

void engine::Game::Update() {
	for (SystemBase* system : systems_) {
		system->Update(kDt);
	}

	entity_manager_.Invalidate();
}

void engine::Game::Render() {
	renderer_.BeginRender();

	for (SystemBase* system : systems_) {
		system->Render();
	}

	entity_manager_.Invalidate();
	renderer_.EndRender();
}

void engine::Game::Delete() {
	for (SystemBase* system : systems_) {
		system->Delete();
	}
}
