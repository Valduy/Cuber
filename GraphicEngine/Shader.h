#pragma once

#include<iostream>
#include <d3d11.h>
#include <d3dcompiler.h>

#include "Renderer.h"

namespace graph {

	class Shader {
	public:
		Shader();

		HRESULT Init(Renderer* renderer, LPCWSTR path);
		void SetShader();

	private:
		Renderer* renderer_;

		Microsoft::WRL::ComPtr<ID3DBlob> vertex_byte_code_;
		Microsoft::WRL::ComPtr<ID3DBlob> pixel_byte_code_;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader_;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader_;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> layout_;

		HRESULT CompileVertexByteCode(LPCWSTR path);
		HRESULT CompilePixelByteCode(LPCWSTR path);
		HRESULT CompileByteCode(LPCWSTR path, LPCSTR entry_point, LPCSTR target, ID3DBlob** byte_code);
		HRESULT CreateVertexShader();
		HRESULT CreatePixelShader();
		HRESULT CreateInputLayout();
	};

} // namespace engine