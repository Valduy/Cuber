#pragma once

#include "../Engine/Game.h"
#include "InputComponent.h"

class InputSystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		GetGame().GetEntityManager().For<InputComponent>([&](ecs::Entity& e) {
			InputComponent& input_component = e.Get<InputComponent>();
			input_component.up = false;
			input_component.down = false;

			switch (input_component.player_type) {
			case PlayerType::kLeft: {
				if (GetGame().GetWindow().GetKeyboardState().IsKeyDown(0x57)) {
					input_component.up = true;
				}
				if (GetGame().GetWindow().GetKeyboardState().IsKeyDown(0x53)) {
					input_component.down = true;
				}
				break;
			}
			case PlayerType::kRight: {
				if (GetGame().GetWindow().GetKeyboardState().IsKeyDown(VK_UP)) {
					input_component.up = true;
				}
				if (GetGame().GetWindow().GetKeyboardState().IsKeyDown(VK_DOWN)) {
					input_component.down = true;
				}
				break;
			}
			}
		});
	}
};