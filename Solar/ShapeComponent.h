#pragma once

#include <vector>
#include <array>
#include <SimpleMath.h>

#include "../Ash/Entity.h"

class ShapeComponent : public ash::Entity::ComponentBase {
public:
	const std::vector<DirectX::SimpleMath::Vector3> points{};
	const std::vector<int> indexes{};

	ShapeComponent(
		std::vector<DirectX::SimpleMath::Vector3> points,
		std::vector<int> indexes)
		: points(points)
		, indexes(indexes)
	{}

	static ShapeComponent* CreateCube() {
		return new ShapeComponent {
			std::vector<DirectX::SimpleMath::Vector3> {
				{ -0.5f, 0.5f, -0.5f }, // Top face
				{ 0.5f,  0.5f, -0.5f },
				{ 0.5f,  0.5f,  0.5f },
				{ -0.5f,  0.5f,  0.5f },

				{ -0.5f, -0.5f,  0.5f }, // Bottom face
				{ 0.5f, -0.5f,  0.5f },
				{ 0.5f, -0.5f, -0.5f },
				{ -0.5f, -0.5f, -0.5f },
			},
			std::vector {
				0, 1, 2,	0, 2, 3,	4, 5, 6,	4, 6, 7,
				3, 2, 5,	3, 5, 4,	2, 1, 6,	2, 6, 5,
				1, 7, 6,	1, 0, 7,	0, 3, 4,	0, 4, 7
			}
		};
	}

	static ShapeComponent* CreateSphere(int subdivisions) {
		constexpr float x = 0.525731112119133606f;
		constexpr float z = 0.850650808352039932f;
		constexpr float n = 0.f;

		std::vector<DirectX::SimpleMath::Vector3> points = {
			{ -x, n, z }, {  x, n, z }, { -x, n,-z },
			{  x, n,-z }, {  n, z, x }, {  n, z,-x },
			{  n,-z, x }, {  n,-z,-x }, {  z, x, n },
			{ -z, x, n }, {  z,-x, n }, { -z,-x, n }
		};

		std::vector<int> indexes = {
			0, 4, 1,	0, 9, 4,	9, 5, 4,	4, 5, 8,	4, 8, 1,
			8, 10, 1,	8, 3, 10,	5, 3, 8,	5, 2, 3,	2, 7, 3,
			7, 10, 3,	7, 6, 10,	7, 11, 6,	11, 0, 6,	0, 1, 6,
			6, 1, 10,	9, 0, 11, 	9, 11, 2,	9, 2, 5, 	7, 2, 11
		};

		for (int i = 0; i < subdivisions; ++i)
		{
			indexes = Subdivide(points, indexes);
		}

		return new ShapeComponent {	points, indexes };
	}

private:
	using Lookup = std::map<std::pair<int, int>, int>;
	using VertexList = std::vector<DirectX::SimpleMath::Vector3>;
	using TriangleList = std::vector<int>;

	static int VertexForEdge(Lookup& lookup, VertexList& vertices, int first, int second) {
		Lookup::key_type key(first, second);

		if (key.first > key.second) {
			std::swap(key.first, key.second);
		}

		const auto inserted = lookup.insert({
			key, vertices.size()
		});

		if (inserted.second)
		{
			DirectX::SimpleMath::Vector3& edge0 = vertices[first];
			DirectX::SimpleMath::Vector3& edge1 = vertices[second];
			DirectX::SimpleMath::Vector3 point = (edge0 + edge1);
			point.Normalize();
			vertices.push_back(point);
		}

		return inserted.first->second;
	}

	static TriangleList Subdivide(VertexList& vertices, TriangleList triangles) {
		Lookup lookup;
		TriangleList result;

		for (int i = 0; i < triangles.size(); i += 3) {
			std::array<int, 3> triangle = {
				triangles[i],
				triangles[i + 1],
				triangles[i + 2]
			};

			std::array<int, 3> mid;

			for (int edge = 0; edge < 3; ++edge)
			{
				mid[edge] = VertexForEdge(
					lookup,
					vertices,
					triangle[edge],
					triangle[(edge + 1) % 3]);
			}

			result.push_back(triangle[0]);
			result.push_back(mid[0]);
			result.push_back(mid[2]);

			result.push_back(triangle[1]);
			result.push_back(mid[1]);
			result.push_back(mid[0]);

			result.push_back(triangle[2]);
			result.push_back(mid[2]);
			result.push_back(mid[1]);

			result.push_back(mid[0]);
			result.push_back(mid[1]);
			result.push_back(mid[2]);
		}

		return result;
	}
};