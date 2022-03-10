#pragma once

#include "../Engine/Game.h"
#include "InputComponent.h"

class InputSystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		For<InputComponent>([&](ecs::Entity& e) {
			InputComponent& input_component = e.Get<InputComponent>();
			input_component.up = false;
			input_component.down = false;

			switch (input_component.player_type) {
			case PlayerType::kLeft: {
				if (GetKeyboardState().IsKeyDown(graph::Keys::kW)) {
					input_component.up = true;
				}
				if (GetKeyboardState().IsKeyDown(graph::Keys::kS)) {
					input_component.down = true;
				}
				break;
			}
			case PlayerType::kRight: {
				if (GetKeyboardState().IsKeyDown(graph::Keys::kUp)) {
					input_component.up = true;
				}
				if (GetKeyboardState().IsKeyDown(graph::Keys::kDown)) {
					input_component.down = true;
				}
				break;
			}
			}
		});
	}
};