#include <iostream>
#include <tuple>
#include "Entity.h"
#include "Engine.h"

class TBase{
public:
	virtual void Test() {
		std::cout << "Base\n";
	}
};

class TChild : public TBase {
	void Test() override {
		std::cout << "Child\n";
	}
};

int main() {
	TBase base = TChild();
	TBase* pBase = &base;
	pBase->Test();
	return 0;
}