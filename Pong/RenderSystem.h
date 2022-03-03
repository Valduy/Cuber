#pragma once

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
	RenderSystem(engine::Game& game)
		: shader_(game.GetRenderer(), L"../Shaders/MyVeryFirstShader.hlsl")
	{}

	void Init(engine::Game& game) override {
		engine::Game::SystemBase::Init(game);
		shader_.Init();

		GetGame().GetEntityManager().For<ShapeComponent, TransformComponent>([&](ecs::Entity& e) {
			e.Add<RenderComponent>([&]() {
				auto shape_component = e.Get<ShapeComponent>();
				auto vertices = GetVertices(shape_component.points, shape_component.color);

				graph::VertexBuffer vb(
					GetGame().GetRenderer(),
					vertices.data(),
					vertices.size());
				vb.Init();

				graph::IndexBuffer ib(
					GetGame().GetRenderer(),
					shape_component.indexes.data(),
					shape_component.indexes.size());
				ib.Init();

				graph::ConstantBuffer cb(GetGame().GetRenderer(), sizeof(ConstData));
				cb.Init();

				return new RenderComponent(vertices, vb, ib, cb);
			});
		});
	}

	void Render() override {
		GetGame().GetRenderer().BeginRender();
		shader_.SetShader();

		GetGame().GetEntityManager().For<RenderComponent, TransformComponent>([&](ecs::Entity& e) {
			RenderComponent& render_component = e.Get<RenderComponent>();
			TransformComponent& transform_component = e.Get<TransformComponent>();
			ConstData data{ transform_component.x, transform_component.y };
			
			render_component.vertex_buffer.SetBuffer();
			render_component.index_buffer.SetBuffer();
			render_component.constant_buffer.SetBuffer();					
			render_component.constant_buffer.Update(&data);

			GetGame().GetRenderer().GetContext()->DrawIndexed(
				render_component.index_buffer.GetSize(), 0, 0);
			});

		GetGame().GetRenderer().EndRender();
	}

private:
	graph::Shader shader_;

	static std::vector<DirectX::XMFLOAT4> GetVertices(
		const std::vector<DirectX::XMFLOAT4>& points,
		const DirectX::XMFLOAT4 color) {
		std::vector<DirectX::XMFLOAT4> vertices;

		for (auto p : points) {
			vertices.push_back(p);
			vertices.push_back(color);
		}

		return vertices;
	}
};