#include "pch.h"
#include "VertexBuffer.h"

#include <cassert>

graph::VertexBuffer::VertexBuffer(void* vertexes, size_t sizemem)
	: renderer_(nullptr)
	, vertexes_(vertexes)
	, sizemem_(sizemem)
	, vertex_buffer_(nullptr)
{}

HRESULT graph::VertexBuffer::Init(Renderer* renderer) {
	renderer_ = renderer;

	D3D11_BUFFER_DESC vertex_buf_desc = {};
	vertex_buf_desc.Usage = D3D11_USAGE_DEFAULT;
	vertex_buf_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex_buf_desc.CPUAccessFlags = 0;
	vertex_buf_desc.MiscFlags = 0;
	vertex_buf_desc.StructureByteStride = 0;
	vertex_buf_desc.ByteWidth = sizemem_;

	D3D11_SUBRESOURCE_DATA vertex_data = {};
	vertex_data.pSysMem = vertexes_;
	vertex_data.SysMemPitch = 0;
	vertex_data.SysMemSlicePitch = 0;

	return renderer_->GetDevice().CreateBuffer(
		&vertex_buf_desc, &vertex_data, &vertex_buffer_);
}

void graph::VertexBuffer::SetBuffer(unsigned int stribes) {
	assert(renderer_ != nullptr && "Vertex buffer isn't initialized");
	const UINT strides[] = { stribes };
	const UINT offsets[] = { 0 };

	renderer_->GetContext().IASetVertexBuffers(0, 1, vertex_buffer_.GetAddressOf(), strides, offsets);
}
