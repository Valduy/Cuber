#pragma once

#include <SimpleMath.h>

#include "RenderComponent.h"
#include "ShapeComponent.h"
#include "TransformComponent.h"
#include "../ECS/EntityManager.h"
#include "../GraphicEngine/Shader.h"
#include "../GraphicEngine/VertexBuffer.h"
#include "../GraphicEngine/IndexBuffer.h"
#include "../Engine/Game.h"
#include "../GraphicEngine/Renderer.h"

#pragma pack(push, 4)
struct ConstData {
	float x;
	float y;
	float dummy0;
	float dummy1;
};
#pragma pack(pop)

class RenderSystem : public engine::Game::SystemBase {
public:
	void Init(engine::Game& game) override {
		engine::Game::SystemBase::Init(game);
		shader_.Init(&GetRenderer(), L"../Shaders/PongShader.hlsl");

		using namespace DirectX::SimpleMath;
		auto it = GetIterator<ShapeComponent, TransformComponent>();
		for (; it.HasCurrent(); it.Next()) {
			ecs::Entity& entity = it.Get();
			ShapeComponent& shape_component = entity.Get<ShapeComponent>();
			auto vertices = GetVertices(shape_component.points, shape_component.color);

			graph::VertexBuffer vb(vertices.data(), sizeof(Vector4) * vertices.size());
			vb.Init(&GetRenderer());

			graph::IndexBuffer ib(shape_component.indexes.data(), shape_component.indexes.size());
			ib.Init(&GetRenderer());

			graph::ConstantBuffer cb(sizeof(ConstData));
			cb.Init(&GetRenderer());

			entity.Add<RenderComponent>([&] {
				return new RenderComponent(vertices, vb, ib, cb);
			});
		}
	}

	void Render() override {
		GetRenderer().BeginRender();
		shader_.SetShader();

		auto it = GetIterator<RenderComponent, TransformComponent>();
		for (; it.HasCurrent(); it.Next()) {
			ecs::Entity& entity = it.Get();
			RenderComponent& render_component = entity.Get<RenderComponent>();
			TransformComponent& transform_component = entity.Get<TransformComponent>();
			ConstData data{ transform_component.x, transform_component.y };

			render_component.vertex_buffer.SetBuffer();
			render_component.index_buffer.SetBuffer();
			render_component.constant_buffer.SetBuffer();
			render_component.constant_buffer.Update(&data);

			GetGame().GetRenderer().GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			GetGame().GetRenderer().GetContext()->DrawIndexed(
				render_component.index_buffer.GetSize(), 0, 0);
		}

		GetRenderer().EndRender();
	}

private:
	graph::Shader shader_;

	static std::vector<DirectX::SimpleMath::Vector4> GetVertices(
		const std::vector<DirectX::SimpleMath::Vector4>& points,
		const DirectX::SimpleMath::Vector4 color)
	{
		std::vector<DirectX::SimpleMath::Vector4> vertices;

		for (auto p : points) {
			vertices.push_back(p);
			vertices.push_back(color);
		}

		return vertices;
	}
};