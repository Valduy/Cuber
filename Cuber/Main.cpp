#include <iostream>
#include <map>

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
	std::map<int, TBase*> arr;
	arr[1] = new TChild();
	auto it = arr.begin();
	
	return 0;
}