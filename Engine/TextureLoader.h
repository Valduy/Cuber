#pragma once

#include <DirectXTex.h>

namespace engine {

class TextureLoader {
public:
	static HRESULT LoadWic(const wchar_t* path, DirectX::ScratchImage* image) {
		return LoadFromWICFile(
			path,
			DirectX::WIC_FLAGS_NONE,
			nullptr,
			*image);
	}

	static HRESULT LoadWicSrgb(const wchar_t* path, DirectX::ScratchImage* image) {
		return LoadFromWICFile(
			path,
			DirectX::WIC_FLAGS_DEFAULT_SRGB,
			nullptr,
			*image);
	}
};

} // namespace engine