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

ecs::EntityManager& engine::Game::GetEntityManager() {
	return entity_manager_;
}

graph::Window& engine::Game::GetWindow() {
	return window_;
}

graph::Renderer& engine::Game::GetRenderer() {
	return renderer_;
}

engine::Game::Game()
	: window_(GetModuleHandle(nullptr), L"Game", 1000, 1000)
    , renderer_(window_)
{}

engine::Game::~Game() {
	for (const SystemBase* system : systems_) {
		delete system;
	}

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
	auto previous = steady_clock::now();
	long long lag = 0;

	while (!is_exit_requested) {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT) {
			is_exit_requested = true;
		}

		auto current = steady_clock::now();
		const long long elapsed = duration_cast<microseconds>(current - previous).count();
		previous = current;
		lag += elapsed;

		while (lag >= kMsPerUpdate) {
			Update();
			lag -= kMsPerUpdate;
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
}

void engine::Game::Render() {
	for (SystemBase* system : systems_) {
		system->Render();
	}
}

void engine::Game::Delete() {
	for (SystemBase* system : systems_) {
		system->Delete();
	}
}
