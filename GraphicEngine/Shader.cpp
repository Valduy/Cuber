#include "pch.h"
#include "Shader.h"

graph::Shader::Shader(Renderer& renderer, LPCWSTR path)
	: renderer_(renderer)
	, path_(path)
	, vertex_byte_code_(nullptr)
	, pixel_byte_code_(nullptr)
	, vertex_shader_(nullptr)
	, pixel_shader_(nullptr)
	, layout_(nullptr) 
{}

HRESULT graph::Shader::Init() {
	HRESULT result;

	if (result = CompileVertexByteCode(); FAILED(result)) {
		return result;
	}
	if (result = CompilePixelByteCode(); FAILED(result)) {
		return result;
	}
	if (result = CreateVertexShader(); FAILED(result)) {
		return result;
	}
	if (result = CreatePixelShader(); FAILED(result)) {
		return result;
	}
	if (result = CreateInputLayout(); FAILED(result)) {
		return result;
	}

	return result;
}

void graph::Shader::SetShader() {
	renderer_.GetContext()->IASetInputLayout(layout_.Get());
	renderer_.GetContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	renderer_.GetContext()->VSSetShader(vertex_shader_.Get(), nullptr, 0);
	renderer_.GetContext()->PSSetShader(pixel_shader_.Get(), nullptr, 0);
}

HRESULT graph::Shader::CompileVertexByteCode() {
	return CompileByteCode(
		path_,
		"VSMain",
		"vs_5_0",
		&vertex_byte_code_);
}

HRESULT graph::Shader::CompilePixelByteCode() {
	return CompileByteCode(
		path_,
		"PSMain",
		"ps_5_0",
		&pixel_byte_code_);
}

HRESULT graph::Shader::CompileByteCode(LPCWSTR path, LPCSTR entry_point, LPCSTR target, ID3DBlob** byte_code) {
	ID3DBlob* error_vertex_code_ = nullptr;
	HRESULT result = D3DCompileFromFile(
		path,
		nullptr,
		nullptr,
		entry_point,
		target,
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		byte_code,
		&error_vertex_code_);

	if (FAILED(result)) {
		if (error_vertex_code_) {
			const char* message = static_cast<char*>(error_vertex_code_->GetBufferPointer());
			std::cout << message << std::endl;
			error_vertex_code_->Release();
		}

		if (vertex_byte_code_) {
			vertex_byte_code_->Release();
		}

		return result;
	}

	return result;
}

HRESULT graph::Shader::CreateVertexShader() {
	return renderer_.GetDevice()->CreateVertexShader(
		vertex_byte_code_->GetBufferPointer(),
		vertex_byte_code_->GetBufferSize(),
		nullptr,
		&vertex_shader_);
}

HRESULT graph::Shader::CreatePixelShader() {
	return renderer_.GetDevice()->CreatePixelShader(
		pixel_byte_code_->GetBufferPointer(),
		pixel_byte_code_->GetBufferSize(),
		nullptr,
		&pixel_shader_);
}

HRESULT graph::Shader::CreateInputLayout() {
	D3D11_INPUT_ELEMENT_DESC input_elements[] = {
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

	return renderer_.GetDevice()->CreateInputLayout(
		input_elements,
		2,
		vertex_byte_code_->GetBufferPointer(),
		vertex_byte_code_->GetBufferSize(),
		&layout_);
}
