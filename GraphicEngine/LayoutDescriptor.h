#pragma once
#include <d3d11.h>
#include <initializer_list>
#include <vector>

namespace graph {

class LayoutDescriptor {
public:
	static const LayoutDescriptor kPosition3;
	static const LayoutDescriptor kPosition3Texture2;
	static const LayoutDescriptor kPosition3Normal3Texture2;
	static const LayoutDescriptor kPosition4Color4;

	const D3D11_INPUT_ELEMENT_DESC* GetData() const;
	size_t GetSize() const;

	LayoutDescriptor() = default;
	LayoutDescriptor(std::initializer_list<D3D11_INPUT_ELEMENT_DESC> list);

	void Add(D3D11_INPUT_ELEMENT_DESC element);

private:
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout_;
};

} // namespace graph


