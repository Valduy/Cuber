#pragma once

#include <SimpleMath.h>

#include "RenderComponent.h"
#include "ShapeComponent.h"
#include "TransformComponent.h"
#include "../ECS/EntityManager.h"
#include "../GraphicEngine/Shader.h"
#include "../GraphicEngine/LayoutDescriptor.h"
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
		using namespace graph;
		using namespace DirectX::SimpleMath;

		engine::Game::SystemBase::Init(game);		
		shader_.Init(&GetRenderer(), kLayout, L"Shaders/PongShader.hlsl");
		
		for (auto& [entity, shape_component, transform_component] : Filter<ShapeComponent, TransformComponent>()) {
			auto vertices = GetVertices(shape_component.points, shape_component.color);

			VertexBuffer vb(vertices.data(), sizeof(Vector4) * vertices.size());
			vb.Init(&GetRenderer());

			IndexBuffer ib(shape_component.indexes.data(), shape_component.indexes.size());
			ib.Init(&GetRenderer());

			ConstantBuffer cb;
			cb.Init(&GetRenderer(), sizeof(ConstData));

			entity.Add<RenderComponent>([&] {
				return new RenderComponent(vertices, vb, ib, cb);
			});
		}
	}

	void Update(float dt) override {
		for (auto& [entity, render_component, transform_component] : Filter<RenderComponent, TransformComponent>()) {
			ConstData data{ transform_component.x, transform_component.y };
			render_component.transform_buffer.Update(&data);
		}
	}

	void Render() override {
		shader_.SetShader();
		GetGame().GetRenderer().GetContext().IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (auto& [entity, render_component] : Filter<RenderComponent>()) {
			render_component.vertex_buffer.SetBuffer(32);
			render_component.index_buffer.SetBuffer();
			render_component.transform_buffer.VSSetBuffer();
			
			GetGame().GetRenderer().GetContext().DrawIndexed(
				render_component.index_buffer.GetSize(), 0, 0);
		}
	}

private:
	const graph::LayoutDescriptor kLayout {
		D3D11_INPUT_ELEMENT_DESC{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0,
		},
		D3D11_INPUT_ELEMENT_DESC{
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0,
		},
	};

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