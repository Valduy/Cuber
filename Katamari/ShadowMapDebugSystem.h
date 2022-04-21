#pragma once

#include <SimpleMath.h>
#include "DirectionLightComponent.h"
#include "../Engine/Game.h"
#include "../GraphicEngine/Shader.h"
#include "../GraphicEngine/ConstantBuffer.h"
#include "../GraphicEngine/VertexBuffer.h"
#include "../GraphicEngine/IndexBuffer.h"

class ShadowMapDebugSystem : public engine::Game::SystemBase {
public:
	struct Vertex {
		DirectX::SimpleMath::Vector2 pos;
		DirectX::SimpleMath::Vector2 tex;
	};

	struct TransformData {
		DirectX::SimpleMath::Matrix proj;
		DirectX::SimpleMath::Vector2 pos;
		float dummy0;
		float dummy1;
	};

	ShadowMapDebugSystem()
		: shader_()
		, vertices_buffer_(kVertices, sizeof(Vertex) * kVerticesSize)
		, indexes_buffer_(kIndexes, kIndexesSize)
		, transform_buffer_(sizeof(TransformData))
	{}

	void Init(engine::Game& game) override {
		engine::Game::SystemBase::Init(game);

		using namespace graph;
		shader_.Init(&GetRenderer(), LayoutDescriptor::kPosition2Texture2, L"Shaders/ShadowMapDebugShader.hlsl");
		vertices_buffer_.Init(&GetRenderer());
		indexes_buffer_.Init(&GetRenderer());
		transform_buffer_.Init(&GetRenderer());

		using namespace DirectX::SimpleMath;
		Matrix transform = Matrix::CreateOrthographic(800, 800, 0.1, 1);
		transform_buffer_.Update(&transform);
	}

	void Update(float dt) override {
		using namespace DirectX::SimpleMath;
		Vector2 position = { 150.0f, 150.0f };
		Matrix projection = Matrix::CreateOrthographic(GetWindow().GetWidth(), GetWindow().GetHeight(), 0.1, 1);
		TransformData transform {
			projection,
			position,
		};
		transform_buffer_.Update(&transform);
	}

	void Render() override {
		auto light_it = GetIterator<DirectionLightComponent>();
		if (!light_it.HasCurrent()) return;

		auto& light = light_it.Get();
		auto& light_component = light.Get<DirectionLightComponent>();

		shader_.SetShader();
		GetRenderer().GetContext().IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		light_component.shadow_map.SetTexture(0);
		vertices_buffer_.SetBuffer(sizeof(Vertex));
		indexes_buffer_.SetBuffer();
		transform_buffer_.VSSetBuffer();

		GetRenderer().GetContext().DrawIndexed(kIndexesSize, 0, 0);
	}

private:
	graph::Shader shader_;	
	graph::VertexBuffer vertices_buffer_;
	graph::IndexBuffer indexes_buffer_;
	graph::ConstantBuffer transform_buffer_;

	static constexpr int kVerticesSize = 4;
	static constexpr int kIndexesSize = 6;
	static constexpr float kMapSize = 200.0f;

	static const Vertex kVertices[kVerticesSize];
	static const int kIndexes[kIndexesSize];
};

const ShadowMapDebugSystem::Vertex ShadowMapDebugSystem::kVertices[kVerticesSize] = {
	{{kMapSize, kMapSize}, {1.0f, 0.0f}},
	{{kMapSize, 0.0f}, {1.0f, 1.0f}},
	{{0.0f, 0.0f}, {0.0f, 1.0f}},
	{{0.0f, kMapSize}, {0.0f, 0.0f}},
};

const int ShadowMapDebugSystem::kIndexes[kIndexesSize] = {
	0, 1, 2,
	0, 2, 3,	
};