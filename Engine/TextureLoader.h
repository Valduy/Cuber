#pragma once

#include "CoreErrors.h"

#include <DirectXTex.h>

namespace engine {

class TextureLoader {
public:
	static std::error_code LoadWic(const wchar_t* path, DirectX::ScratchImage* image) {
		const HRESULT hr = LoadFromWICFile(
			path,
			DirectX::WIC_FLAGS_NONE,
			nullptr,
			*image);

		return FAILED(hr) ? CoreErrors::kInternalError : CoreErrors::kSuccess;
	}

	static std::error_code LoadWicSrgb(const wchar_t* path, DirectX::ScratchImage* image) {
		const HRESULT hr = LoadFromWICFile(
			path,
			DirectX::WIC_FLAGS_DEFAULT_SRGB,
			nullptr,
			*image);

		return FAILED(hr) ? CoreErrors::kInternalError : CoreErrors::kSuccess;
	}
};

} // namespace engine