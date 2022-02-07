#pragma once

#include "Engine.h"

namespace ecs {

class SystemBase {
public:
	using Priority = uint32_t;

	//Engine& GetEngine() {
	//	return engine_;
	//}

	SystemBase(Engine& engine, Priority priority)
		: engine_(engine)
		, priority_(priority)
	{}

	void Update(float dt) {

	}

private:
	Engine& engine_;
	Priority priority_;
	
};

} // namespace ecs