#pragma once

#include <vector>

#include "Vertex.h"

namespace engine {

class Mesh {
public:
	const std::vector<Vertex> vertices{};
	const std::vector<int> indices{};

	Mesh() = default;

	Mesh(
		std::vector<Vertex> vertices, 
		std::vector<int> indices)
		: vertices(vertices)
		, indices(indices)
	{}
};

} // namespace engine