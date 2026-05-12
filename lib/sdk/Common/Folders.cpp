
#include "Folders.hpp"




const Folders & Folders::get() noexcept
{
	static const Folders f = [] () -> Folders {
#ifdef SDK_SHARE_DIR
		const auto shareDir = std::filesystem::path(SDK_SHARE_DIR);
#else
		const auto shareDir = std::filesystem::current_path();
#endif
		const auto shadersDir = shareDir / "shaders";
		const auto imagesDir = shareDir / "images";

#ifdef SDK_MODELS_DIR
		const auto modelsDir = std::filesystem::path(SDK_MODELS_DIR);
#else
		const auto modelsDir = shareDir / "models";
#endif

		std::error_code ec;

		const auto cacheDir = std::filesystem::current_path() / "cache";
		std::filesystem::create_directory(cacheDir, ec);

		const auto outputDir = std::filesystem::current_path() / "output";
		std::filesystem::create_directory(outputDir, ec);

		const auto userDir = std::filesystem::current_path() / "user";
		std::filesystem::create_directory(userDir, ec);

		return Folders {
			.shareDir = shareDir,
			.shadersDir = shadersDir,
			.imagesDir = imagesDir,
			.modelsDir = modelsDir,
			.cacheDir = cacheDir,
			.outputDir = outputDir,
			.userDir = userDir,
		};
	}();

	return f;
}
