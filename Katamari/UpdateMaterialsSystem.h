#pragma once

#include "../Engine/Game.h"
#include "MaterialComponent.h"
#include "MaterialData.h"

class UpdateMaterialsSystem : public engine::Game::SystemBase {
public:
	void Init(engine::Game& game) override {
		engine::Game::SystemBase::Init(game);

		for (auto it = GetIterator<MaterialComponent>(); it.HasCurrent(); it.Next()) {
			auto& entity = it.Get();
			auto& material_component = entity.Get<MaterialComponent>();
			material_component.material_buffer.Init(&GetRenderer(), sizeof(MaterialData));
		}
	}

	void Update(float dt) override {
		for (auto it = GetIterator<MaterialComponent>(); it.HasCurrent(); it.Next()) {
			auto& entity = it.Get();
			auto& material_component = entity.Get<MaterialComponent>();

			MaterialData material_data {
				material_component.material.ambient,
				material_component.material.shininess,
				material_component.material.specular,
			};
			material_component.material_buffer.Update(&material_data);
		}
	}
};
