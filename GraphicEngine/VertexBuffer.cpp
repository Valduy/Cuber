#include "pch.h"
#include "VertexBuffer.h"

graph::VertexBuffer::VertexBuffer(
	Renderer& renderer, 
	std::vector<DirectX::SimpleMath::Vector4> vertexes)
	: renderer_(renderer)
	, vertexes_(vertexes)
	, vertex_buffer_(nullptr)
{}

HRESULT graph::VertexBuffer::Init() {
	D3D11_BUFFER_DESC vertex_buf_desc = {};
	vertex_buf_desc.Usage = D3D11_USAGE_DEFAULT;
	vertex_buf_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex_buf_desc.CPUAccessFlags = 0;
	vertex_buf_desc.MiscFlags = 0;
	vertex_buf_desc.StructureByteStride = 0;
	vertex_buf_desc.ByteWidth = sizeof(DirectX::SimpleMath::Vector4) * vertexes_.size();

	D3D11_SUBRESOURCE_DATA vertex_data = {};
	vertex_data.pSysMem = vertexes_.data();
	vertex_data.SysMemPitch = 0;
	vertex_data.SysMemSlicePitch = 0;

	return renderer_.GetDevice()->CreateBuffer(&vertex_buf_desc, &vertex_data, &vertex_buffer_);
}

void graph::VertexBuffer::SetBuffer() {
	const UINT strides[] = { 32 };
	const UINT offsets[] = { 0 };

	renderer_.GetContext()->IASetVertexBuffers(0, 1, vertex_buffer_.GetAddressOf(), strides, offsets);
}
