#pragma once

#include <directxmath.h>
#include <SimpleMath.h>

#include "../Engine/Game.h"
#include "../GraphicEngine/Shader.h"
#include "RenderComponent.h"
#include "ShapeComponent.h"
#include "TransformComponent.h"

struct ConstData {
	DirectX::SimpleMath::Matrix mat;
};

class RenderSystem : public engine::Game::SystemBase {
public:
	RenderSystem(engine::Game& game)
		: shader_(game.GetRenderer(), L"../Shaders/SolarShader.hlsl")
	{}

	void Init(engine::Game& game) override {
		engine::Game::SystemBase::Init(game);
		shader_.Init();

		GetGame().GetEntityManager().For<
			ShapeComponent>([&](ecs::Entity& e) {
				e.Add<RenderComponent>([&]() {
					ShapeComponent& shape_component = e.Get<ShapeComponent>();
					auto vertices = GetVertices(shape_component.points);

					graph::VertexBuffer vb(
						GetGame().GetRenderer(),
						vertices);
					vb.Init();

					graph::IndexBuffer ib(
						GetGame().GetRenderer(),
						shape_component.indexes);
					ib.Init();

					graph::ConstantBuffer cb(
						GetGame().GetRenderer(),
						sizeof(ConstData)
					);
					cb.Init();

					return new RenderComponent(vertices, vb, ib, cb);
				});				
			});
	}

	void Render() override {
		GetGame().GetRenderer().BeginRender();
		shader_.SetShader();

		GetGame().GetEntityManager().For<
			RenderComponent,
			TransformComponent>([&](ecs::Entity& e) {
				RenderComponent& render_component = e.Get<RenderComponent>();
				TransformComponent& transform_component = e.Get<TransformComponent>();

				using namespace DirectX::SimpleMath;

				DirectX::XMMATRIX model = DirectX::XMMatrixIdentity();
				//model *= DirectX::XMMatrixScaling(1.0, 1.0, 1.0);
				model *= DirectX::XMMatrixRotationY(transform_component.rotation.y);
				model *= DirectX::XMMatrixRotationZ(transform_component.rotation.z);
				//model *= DirectX::XMMatrixTranslation(0.0, 0.0, 2.0);

				DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 0.0f, -3.0f, 0.0f);
				DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
				DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
				DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(eye, at, up);

				//Matrix view = Matrix::Identity;

				DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(
					DirectX::XM_PIDIV2, 1.0, 0.1, 100);
				//Matrix projection = Matrix::Identity;

				DirectX::XMMATRIX mat =  model * view * projection;

				ConstData data {DirectX::XMMatrixTranspose(mat)};

				render_component.vertex_buffer.SetBuffer();
				render_component.index_buffer.SetBuffer();
				render_component.constant_buffer.SetBuffer();
				render_component.constant_buffer.Update(&data);

				GetGame().GetRenderer().GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				GetGame().GetRenderer().GetContext()->DrawIndexed(
					render_component.index_buffer.GetSize(), 0, 0);
			});

		GetGame().GetRenderer().EndRender();
	}

	void Update(float dt) override {
		GetGame().GetEntityManager().For<
			TransformComponent>([&](ecs::Entity& e) {
				TransformComponent& tc = e.Get<TransformComponent>();
				tc.rotation.y += dt * 2.0f;
				tc.rotation.z += dt * 2.0f;
			});
	}

private:
	graph::Shader shader_;

	std::vector<DirectX::SimpleMath::Vector4> GetVertices(
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
				result.push_back({ 0.0f, 1.0f, 0.0f, 1.0f });
				color += 1;
			}
			else {
				result.push_back({ 0.0f, 0.0f, 1.0f, 1.0f });
				color = 0;
			}
		}

		return result;
	}
};