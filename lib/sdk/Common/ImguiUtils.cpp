/*
* SPDX-License-Identifier: BSD-3-Clause
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#include "ImguiUtils.hpp"

#include <backends/imgui_impl_opengl3.h>

#include "Log.hpp"




namespace imgui {




void Context::Frame::render() noexcept
{
	ASSERT(context_)
	ImGui::Render();
}




std::unique_ptr<Context> Context::create()
{
	ImGuiContext * const context = ImGui::CreateContext();
	CHECK(context)
	return std::unique_ptr<Context>(new Context(context));
}


Context::~Context() noexcept
{
	ASSERT(lockRefCount_ == 0)
	ImGui::DestroyContext(context_);
}


Context::Lock Context::lock() noexcept
{
	lockRefCount_++;
	if (lockRefCount_ == 1) {
		ImGui::SetCurrentContext(context_);
	}
	return Lock{ .c = *this };
}


void Context::_unlock() noexcept
{
	ASSERT(lockRefCount_ > 0)
	lockRefCount_--;
	if (lockRefCount_ == 0) {
		ImGui::SetCurrentContext(nullptr);
	}
}


void Context::beginFrame()
{
	ImGui::NewFrame();
}


void Context::endFrame()
{
	ImGui::EndFrame();
}




namespace detail {

class OpenGLGraphics : public Graphics {
public:
	OpenGLGraphics(Context & context) :
		Graphics(context)
	{
		const char * const glslVersion =
#ifdef SDK_USE_GLES3
				"#version 300 es";
#else
				"#version 100";
#endif

		auto lock = context_.lock();
		CHECK(ImGui_ImplOpenGL3_Init(glslVersion))
	}

	~OpenGLGraphics() noexcept
	{
		auto lock = context_.lock();
		ImGui_ImplOpenGL3_Shutdown();
	}

	void beginFrame() override
	{
		ImGui_ImplOpenGL3_NewFrame();
	}

	void endFrame() override
	{
	}

	void render() override
	{
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
};

} // detail




std::unique_ptr<Graphics> Graphics::createOpenGL(Context & context)
{
	return std::unique_ptr<Graphics>(new detail::OpenGLGraphics(context));
}


Graphics::Graphics(Context & context) noexcept :
	context_(context)
{
	// ImGui_ImplGlfw_InitForOpenGL(window, true);
}


Graphics::~Graphics() noexcept
{
	// ImGui_ImplGlfw_Shutdown();
}




} // imgui
