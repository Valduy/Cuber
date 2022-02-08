#pragma once

namespace ecs {

class IComponent {
public:
	virtual ~IComponent() = default;
};

} // namespace ecs