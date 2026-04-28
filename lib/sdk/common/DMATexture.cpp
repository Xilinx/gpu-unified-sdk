
#include "DMATexture.hpp"

#include <unordered_set>




DmaTexture DmaTexture::create(const EGLDisplay display, const Dma::Buffer & dmaBuffer,
		const Target target, const Params & params)
{
	const EGLint commonImageAttributes[] = {
		EGL_WIDTH,                     dmaBuffer.size().width,
		EGL_HEIGHT,                    dmaBuffer.size().height,
		EGL_LINUX_DRM_FOURCC_EXT,      EGLint(dmaBuffer.pixelFormat()),
		EGL_DMA_BUF_PLANE0_FD_EXT,     dmaBuffer.fd(),
		EGL_DMA_BUF_PLANE0_OFFSET_EXT, 0,
		EGL_DMA_BUF_PLANE0_PITCH_EXT,  dmaBuffer.pitch(),
		EGL_NONE,
	};

	std::vector<EGLint> cache;
	const EGLint * const allImageAttributes = [&commonImageAttributes, &params, &cache] ()
			-> const EGLint * {
		if (!params.extraImageAttributes) {
			return commonImageAttributes;
		} else {
			std::unordered_set<EGLint> keys;
			const auto add = [&cache, &keys] (const EGLint * const a) {
				if (!a) return;
				for (int i = 0; ; ++i) {
					const EGLint key = a[i * 2];
					if (key == EGL_NONE) break;
					if (!keys.contains(key)) {
						keys.insert(key);
						const EGLint value = a[i * 2 + 1];
						cache.push_back(key);
						cache.push_back(value);
					}
				}
			};
			add(params.extraImageAttributes);
			add(commonImageAttributes);
			cache.push_back(EGL_NONE);
			return cache.data();
		}
	}();

	const EGLImageKHR eglImageHandle = GL_DYNAMIC_FUNCTIONS_NAMESPACE eglCreateImageKHR(display,
			EGL_NO_CONTEXT,
			EGL_LINUX_DMA_BUF_EXT,
			nullptr,
			allImageAttributes);
	CHECK_EGL_ERROR()
	CHECK(eglImageHandle != EGL_NO_IMAGE_KHR)

	EglImage eglImage(display, eglImageHandle);

	return adapt(std::move(eglImage), target);
}


DmaTexture DmaTexture::adapt(EglImage && eglImage, const Target target)
{
	TextureId textureId = TextureId::create();
	glBindTexture(GLenum(target), textureId);
	CHECK_EGL_ERROR()

	GL_DYNAMIC_FUNCTIONS_NAMESPACE glEGLImageTargetTexture2DOES(GLenum(target), eglImage);
	CHECK_EGL_ERROR()

	return DmaTexture(target, std::move(eglImage), std::move(textureId));
}


DmaTexture::~DmaTexture() noexcept
{
}




DmaFbo DmaFbo::create(const EGLDisplay display, const Dma::Buffer & dmaBuffer)
{
	DmaTexture dmaTexture = DmaTexture::create(display, dmaBuffer, DmaTexture::Target::Texture2D,
			DmaTexture::Params{});

	FboId fboId = FboId::create();
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
			dmaTexture.textureId(), 0);

	const GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	CHECK(fboStatus == GL_FRAMEBUFFER_COMPLETE, Log::hex(fboStatus, 2))

	return DmaFbo(std::move(dmaTexture), std::move(fboId));
}


DmaFbo::~DmaFbo() noexcept
{
}
