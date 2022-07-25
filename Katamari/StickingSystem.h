#pragma once

#include "../Engine/Game.h"
#include "../Engine/TransformComponent.h"
#include "CollisionComponent.h"
#include "ItemComponent.h"
#include "KatamariComponent.h"

class StickingSystem : public engine::Game::SystemBase {
public:
	void Update(float dt) override {
		using namespace ecs;
		using namespace engine;
		using namespace DirectX::SimpleMath;

		auto filter = Filter<TransformComponent, KatamariComponent, CollisionComponent>();
		const auto it = filter.GetIterator();
		if (!it.HasCurrent()) return;

		auto& [katamari, katamari_transform, katamari_component, katamari_collision] = it.Get();
		
		for (auto& node : Filter<TransformComponent, ItemComponent, CollisionComponent>()) {
			auto& [item, item_transform, item_component, item_collision] = node;

			if (IsCanCatch(katamari_transform, katamari_collision, item_transform, item_collision) &&
				IsCollide(katamari_transform, katamari_collision, item_transform, item_collision))
			{
				item.Remove<ItemComponent>();
				const Vector3 position = item_transform.GetPosition();
				const Vector3 scale = item_transform.GetScale();
				const Quaternion rotation = item_transform.GetRotation();

				katamari_transform.AddChild(item);
				item_transform.SetPosition(position);
				item_transform.SetScale(scale);
				item_transform.SetRotation(rotation);
				ScaleUp(katamari_transform, katamari_collision, item_transform, item_collision);
			}
		}
	}

private:
	static bool IsCanCatch(
		const engine::TransformComponent& katamari_transform,
		const CollisionComponent& katamari_collision,
		const engine::TransformComponent& item_transform,
		const CollisionComponent& item_collision)
	{
		const float katamari_radius = katamari_collision.radius * GetMaxComponent(katamari_transform.GetScale());
		const float item_radius = item_collision.radius * GetMaxComponent(item_transform.GetScale());
		return katamari_radius > item_radius;
	}

	static bool IsCollide(
		const engine::TransformComponent& katamari_transform,
		const CollisionComponent& katamari_collision,
		const engine::TransformComponent& item_transform,
		const CollisionComponent& item_collision)
	{
		using namespace DirectX::SimpleMath;
		const Vector3 katamari_offset = Vector3::Transform(
			katamari_collision.offset, 
			katamari_transform.GetModelMatrix());
		const float katamari_radius = katamari_collision.radius * GetMaxComponent(katamari_transform.GetScale());

		const Vector3 item_offset = Vector3::Transform(
			item_collision.offset,
			item_transform.GetModelMatrix());
		const float item_radius = item_collision.radius * GetMaxComponent(item_transform.GetScale());

		const Vector3 difference = item_offset - katamari_offset;
		return difference.Length() < katamari_radius + item_radius;
	}

	static void ScaleUp(
		const engine::TransformComponent& katamari_transform,
		CollisionComponent& katamari_collision,
		const engine::TransformComponent& item_transform,
		const CollisionComponent& item_collision)
	{
		using namespace DirectX::SimpleMath;
		constexpr float ratio = 0.05f;

		auto& sphere_transform = katamari_collision.sphere.Get<engine::TransformComponent>();
		katamari_collision.radius += ratio;
		sphere_transform.SetLocalScale({
			sphere_transform.GetLocalScale().x + ratio,
			sphere_transform.GetLocalScale().y + ratio,
			sphere_transform.GetLocalScale().z + ratio,
		});
	}

	static float GetMaxComponent(const DirectX::SimpleMath::Vector3& vector) {
		if (vector.x > vector.y) {
			if (vector.x > vector.z) {
				return vector.x;
			}

			return vector.z;
		}

		if (vector.y > vector.z) {
			return vector.y;
		}

		return vector.z;
	}
};
