#pragma once

#include <directxmath.h>
#include <SimpleMath.h>

#include "../Engine/TransformComponent.h"
#include "../Engine/CameraComponent.h"
#include "../Engine/Game.h"
#include "../GraphicEngine/Shader.h"
#include "../GraphicEngine/LayoutDescriptor.h"
#include "RenderComponent.h"
#include "ShapeComponent.h"

class RenderSystem : public engine::Game::SystemBase {
public:
	struct TransformData {
		DirectX::SimpleMath::Matrix mat;
	};

	void Init(engine::Game& game) override {
		using namespace graph;
		using namespace engine;

		engine::Game::SystemBase::Init(game);		
		shader_.Init(&GetRenderer(), kLayout, L"Shaders/SolarShader.hlsl");

		for (auto& [entity, transform_component, shape_component] : Filter<TransformComponent, ShapeComponent>()) {
			auto vertices = GetVertices(shape_component.points);

			VertexBuffer vb(vertices.data(), sizeof(DirectX::SimpleMath::Vector4) * vertices.size());
			vb.Init(&GetRenderer());

			IndexBuffer ib(shape_component.indexes.data(), shape_component.indexes.size());
			ib.Init(&GetRenderer());

			ConstantBuffer cb;
			cb.Init(&GetRenderer(), sizeof(TransformData));

			entity.Add<RenderComponent>([&] {
				return new RenderComponent(vertices, vb, ib, cb);
			});
		}
	}

	void Update(float dt) override {
		// Only for first camera.
		auto it = Filter<engine::CameraComponent>().GetIterator();
		if (!it.HasCurrent()) return;
		auto& [camera, camera_component] = it.Get();

		using namespace engine;
		for (auto& [entity, render_component, transform_component] :  Filter<RenderComponent, TransformComponent>()) {
			DirectX::SimpleMath::Matrix model_matrix = transform_component.GetModelMatrix();
			DirectX::SimpleMath::Matrix camera_matrix = camera_component.GetCameraMatrix();
			DirectX::SimpleMath::Matrix transform_matrix = model_matrix * camera_matrix;
			TransformData data{ transform_matrix.Transpose() };
			render_component.transform_buffer.Update(&data);
		}
	}

	void Render() override {
		using namespace engine;

		shader_.SetShader();
		GetRenderer().GetContext().IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				
		for (auto& [entity, render_component] : Filter<RenderComponent>()) {
			render_component.vertex_buffer.SetBuffer(32);
			render_component.index_buffer.SetBuffer();
			render_component.transform_buffer.VSSetBuffer();
			
			GetRenderer().GetContext().DrawIndexed(
				render_component.index_buffer.GetSize(), 0, 0);
		}
	}

private:
	const graph::LayoutDescriptor kLayout{
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
		const std::vector<DirectX::SimpleMath::Vector3>& points)
	{
		std::vector<DirectX::SimpleMath::Vector4> result;
		int color = 0;

		for (auto p : points) {
			result.push_back({ p.x, p.y, p.z, 1.0f });

			if (color == 0) {
				result.push_back({ 1.0f, 0.0f, 0.0f, 1.0f });
				color += 1;
			}
			else if (color == 1) {
				result.push_back({ 1.0f, 1.0f, 0.0f, 1.0f });
				color += 1;
			}
			else {
				result.push_back({ 1.0f, 0.0f, 1.0f, 1.0f });
				color = 0;
			}
		}

		return result;
	}
};