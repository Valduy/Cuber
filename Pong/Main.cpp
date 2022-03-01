#include <directxmath.h>
#include <vector>

#include "../ECS/EntityManager.h"
#include "../GraphicEngine/Window.h"
#include "../GraphicEngine/Renderer.h"
#include "../GraphicEngine/Shader.h"
#include "../GraphicEngine/VertexBuffer.h"
#include "../GraphicEngine/IndexBuffer.h"
#include "../Engine/Game.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

class ShapeComponent : public ecs::IComponent {
public:
	std::vector<DirectX::XMFLOAT4> points;
	std::vector<int> indexes;
	DirectX::XMFLOAT4 color;
};

class RenderComponent : public ecs::IComponent {
public:
	graph::Shader& shader;
	std::vector<DirectX::XMFLOAT4> vertices;
	graph::VertexBuffer vertex_buffer;
	graph::IndexBuffer index_buffer;

	RenderComponent(
		graph::Shader& shader,
		std::vector<DirectX::XMFLOAT4> vertices,
		graph::VertexBuffer vertex_buffer, 
		graph::IndexBuffer index_buffer)
		: shader(shader)
		, vertices(vertices)
		, vertex_buffer(vertex_buffer)
		, index_buffer(index_buffer)
	{}
};

class RenderSystem : public engine::Game::SystemBase {
public:
	RenderSystem(engine::Game& game)
		: shader_(game.GetRenderer(), L"../Shaders/MyVeryFirstShader.hlsl")
	{}

	void Init(engine::Game& game) override {
		engine::Game::SystemBase::Init(game);
		shader_.Init();

		GetGame().GetEntityManager().For<ShapeComponent>([&](ecs::Entity& e) {
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

				return new RenderComponent(shader_, vertices, vb, ib);
			});
		});
	}

	void Update(float dt) override {
		if (GetGame().GetWindow().GetKeyboardState().IsKeyDown(VK_SPACE)) {
			std::cout << "space!" << std::endl;
		}
	}

	void Render() override {
		GetGame().GetRenderer().BeginRender();

		GetGame().GetEntityManager().For<RenderComponent>([&](ecs::Entity& e) {
			auto render_component = e.Get<RenderComponent>();
			render_component.vertex_buffer.SetBuffer();
			render_component.index_buffer.SetBuffer();
			render_component.shader.SetShader();

			GetGame().GetRenderer().GetContext()->DrawIndexed(3, 0, 0);
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

int main() {
	engine::Game game;
	RenderSystem render_system(game);
	game.PushSystem(render_system);

	ecs::Entity& e1 = game.GetEntityManager().AddEntity();
	ShapeComponent& shape1 = e1.Add<ShapeComponent>();
	shape1.points = {
		DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),	
		DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),
	};
	shape1.indexes = { 0, 1, 2 };
	shape1.color = { 1, 1, 1, 1 };

	ecs::Entity& e2 = game.GetEntityManager().AddEntity();
	ShapeComponent& shape2 = e2.Add<ShapeComponent>();
	shape2.points = {
		DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),
		DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),
	};
	shape2.indexes = { 0, 1, 2 };
	shape2.color = { 1, 0, 1, 1 };

	game.Run();

	return 0;
}