
#include "GraphicsUtils.hpp"

#include <array>
#include <optional>

#ifdef SDK_USE_GLES3
#include <GLES3/gl3.h>
#endif

#include "Folders.hpp"
#include "Log.hpp"
#include "LogTypes.hpp"
#include "PngUtils.hpp"




RegularTexture RegularTexture::fromPng(const std::filesystem::path & path)
{
	TextureId textureId = TextureId::create();

	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	Size size;
	std::unique_ptr<uint8_t[]> data;

	const auto alloc = [&] (const Size & pngSize) -> PngLoadMemory {
		size = pngSize;
		const int pitch = size.width * 4;
		data.reset(new uint8_t[pitch * size.height]);

		return PngLoadMemory {
			.data  = data.get(),
			.pitch = pitch,
		};
	};

	::loadPng(Folders::get().imagesDir / path, alloc);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.width, size.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
			data.get());

	return RegularTexture {
		.textureId = std::move(textureId),
		.size = size,
	};
}




CubemapTexture CubemapTexture::fromPng(const std::filesystem::path & prefixPath)
{
	struct FaceInfo {
		GLenum face;
		std::string_view name;
	};

	static constexpr std::array<FaceInfo, 6> faceInfos = {{
		{ GL_TEXTURE_CUBE_MAP_POSITIVE_X, "positive_x" }, // left
		{ GL_TEXTURE_CUBE_MAP_NEGATIVE_X, "negative_x" }, // right
		{ GL_TEXTURE_CUBE_MAP_POSITIVE_Y, "positive_y" }, // top
		{ GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, "negative_y" }, // bottom
		{ GL_TEXTURE_CUBE_MAP_POSITIVE_Z, "positive_z" }, // back
		{ GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, "negative_z" }, // front
	}};

	const std::filesystem::path dir = prefixPath.parent_path();
	const std::string prefix = prefixPath.filename();

	TextureId textureId = TextureId::create();

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#ifdef SDK_USE_GLES3
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
#endif

	std::optional<Size> commonSize;

	for (const FaceInfo & faceInfo : faceInfos) {
		Size size;
		std::unique_ptr<uint8_t[]> data;

		const auto alloc = [&size, &data, &commonSize, &faceInfo] (
				const Size & pngSize) -> PngLoadMemory {
			size = pngSize;

			if (commonSize.has_value()) {
				CHECK(size == *commonSize, "common:", *commonSize, "this:", size)
			} else {
				commonSize = size;
			}

			const int pitch = size.width * 4;
			data.reset(new uint8_t[pitch * size.height]);

			return PngLoadMemory {
				.data  = data.get(),
				.pitch = pitch,
			};
		};

		::loadPng(dir / (prefix + std::string(faceInfo.name) + ".png"), alloc);

		glTexImage2D(faceInfo.face, 0, GL_RGBA, size.width, size.height, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, data.get());
	}

	return CubemapTexture {
		.textureId = std::move(textureId),
		.size = *commonSize,
	};
}




#ifdef SDK_USE_DMA
DrmTexture DrmTexture::fromPng(const int drmDevFd, const EGLDisplay eglDisplay,
		const std::filesystem::path & path)
{
	Drm::Buffer drmBuffer = Drm::Buffer::null();
	DmaTexture dmaTexture = DmaTexture::null();

	const auto alloc = [&] (const Size & pngSize) -> PngLoadMemory {
		drmBuffer = Drm::Buffer::create(drmDevFd,
			Dma::Buffer::Params{
				.size = pngSize,
				.pixelFormat = PixelFormat::RGBA8888,
			});

		dmaTexture = DmaTexture::create(eglDisplay,
				drmBuffer.dmaBuffer(),
				DmaTexture::Target::TextureExternalOes,
				DmaTexture::Params{});

		return PngLoadMemory {
			.data  = drmBuffer.dmaBuffer().data(),
			.pitch = drmBuffer.dmaBuffer().pitch(),
		};
	};

	::loadPng(Folders::get().imagesDir / path, alloc);

	glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	return DrmTexture {
		.drmBuffer = std::move(drmBuffer),
		.dmaTexture = std::move(dmaTexture),
	};
}
#endif
