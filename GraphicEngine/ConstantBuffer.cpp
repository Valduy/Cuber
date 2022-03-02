#include "pch.h"
#include "ConstantBuffer.h"

graph::ConstantBuffer::ConstantBuffer(Renderer& renderer, size_t size)
	: renderer_(renderer)
    , size_(size)
	, constant_buffer_(nullptr)
{}

HRESULT graph::ConstantBuffer::Init() {
	D3D11_BUFFER_DESC const_buf_desc = {};
	const_buf_desc.Usage = D3D11_USAGE_DEFAULT;
	const_buf_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	const_buf_desc.CPUAccessFlags = 0;
	const_buf_desc.MiscFlags = 0;
	const_buf_desc.StructureByteStride = 0;
	const_buf_desc.ByteWidth = size_;

	return renderer_.GetDevice()->CreateBuffer(&const_buf_desc, nullptr, &constant_buffer_);
}

void graph::ConstantBuffer::SetBuffer() {
	renderer_.GetContext()->VSSetConstantBuffers(0, 1, constant_buffer_.GetAddressOf());
}

void graph::ConstantBuffer::Update(void* data) {
	renderer_.GetContext()->UpdateSubresource(constant_buffer_.Get(), 0, nullptr, data, 0, 0);
}
