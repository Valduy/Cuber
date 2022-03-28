#include "pch.h"
#include "LayoutDescriptor.h"

const graph::LayoutDescriptor graph::LayoutDescriptor::kPosition3{
	D3D11_INPUT_ELEMENT_DESC {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		0,
		D3D11_INPUT_PER_VERTEX_DATA,
		0,
	},
};

const graph::LayoutDescriptor graph::LayoutDescriptor::kPosition3Texture2 {
	D3D11_INPUT_ELEMENT_DESC {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		0,
		D3D11_INPUT_PER_VERTEX_DATA,
		0,
	},
	D3D11_INPUT_ELEMENT_DESC {
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA,
		0,
	},
};

const graph::LayoutDescriptor graph::LayoutDescriptor::kPosition3Normal3Texture2{
	D3D11_INPUT_ELEMENT_DESC {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		0,
		D3D11_INPUT_PER_VERTEX_DATA,
		0,
	},
	D3D11_INPUT_ELEMENT_DESC {
		"NORMAL",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA,
		0,
	},
	D3D11_INPUT_ELEMENT_DESC {
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA,
		0,
	},
};

const graph::LayoutDescriptor graph::LayoutDescriptor::kPosition4Color4 {
	D3D11_INPUT_ELEMENT_DESC {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0,
		0,
		D3D11_INPUT_PER_VERTEX_DATA,
		0,
	},
	D3D11_INPUT_ELEMENT_DESC {
		"COLOR",
		0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0,
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA,
		0,
	},
};


const D3D11_INPUT_ELEMENT_DESC* graph::LayoutDescriptor::GetData() const {
	return layout_.data();
}

size_t graph::LayoutDescriptor::GetSize() const {
	return layout_.size();
}

graph::LayoutDescriptor::LayoutDescriptor(std::initializer_list<D3D11_INPUT_ELEMENT_DESC> list)
	: layout_(list)
{}

void graph::LayoutDescriptor::Add(D3D11_INPUT_ELEMENT_DESC element) {
	layout_.push_back(element);
}
