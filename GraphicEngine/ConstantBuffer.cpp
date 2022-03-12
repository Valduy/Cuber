#include "pch.h"
#include "ConstantBuffer.h"

#include <cassert>

graph::ConstantBuffer::ConstantBuffer(size_t sizemem)
	: renderer_(nullptr)
    , sizemem_(sizemem)
	, constant_buffer_(nullptr)
{}

HRESULT graph::ConstantBuffer::Init(Renderer* renderer) {
	renderer_ = renderer;

	D3D11_BUFFER_DESC const_buf_desc = {};
	const_buf_desc.Usage = D3D11_USAGE_DEFAULT;
	const_buf_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	const_buf_desc.CPUAccessFlags = 0;
	const_buf_desc.MiscFlags = 0;
	const_buf_desc.StructureByteStride = 0;
	const_buf_desc.ByteWidth = sizemem_;

	return renderer_->GetDevice()->CreateBuffer(&const_buf_desc, nullptr, &constant_buffer_);
}

void graph::ConstantBuffer::SetBuffer() {
	assert(renderer_ != nullptr && "Constant buffer isn't initialized");
	renderer_->GetContext()->VSSetConstantBuffers(0, 1, constant_buffer_.GetAddressOf());
}

void graph::ConstantBuffer::Update(void* data) {
	assert(renderer_ != nullptr && "Constant buffer isn't initialized");
	renderer_->GetContext()->UpdateSubresource(constant_buffer_.Get(), 0, nullptr, data, 0, 0);
}
