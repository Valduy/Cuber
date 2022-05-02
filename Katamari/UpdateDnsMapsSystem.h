#pragma once

#include "DnsMapsComponent.h"
#include "../Engine/Game.h"

class UpdateDnsMapsSystem : public engine::Game::SystemBase {
public:
	void Init(engine::Game& game) override {
		engine::Game::SystemBase::Init(game);

		for (auto it = GetIterator<DnsMapsComponent>(); it.HasCurrent(); it.Next()) {
			auto& entity = it.Get();
			auto& dns_maps_component = entity.Get<DnsMapsComponent>();

			if (dns_maps_component.diffuse.GetImageCount() > 0) {
				dns_maps_component.diffuse_texture.Init(&GetRenderer(), dns_maps_component.diffuse);
			}
			if (dns_maps_component.normal.GetImageCount() > 0) {
				dns_maps_component.normal_texture.Init(&GetRenderer(), dns_maps_component.normal);
			}
			if (dns_maps_component.specular.GetImageCount() > 0) {
				dns_maps_component.specular_texture.Init(&GetRenderer(), dns_maps_component.specular);
			}
		}
	}
};