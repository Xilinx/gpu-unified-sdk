/*
* Copyright (C) <2026> Advanced Micro Devices, Inc. All rights reserved.
*/


#pragma once

#include <memory>
#include <utility>

#include <imgui.h>




namespace imgui {




namespace detail {
	class OpenGLGraphics;
}




class Context {
public:
	struct Lock {
		ImGuiContext * context() const noexcept {
			return c.context_;
		}

		~Lock() noexcept
		{
			c._unlock();
		}

		Context & c;
	};

	class Frame {
	public:
		Frame(Frame && frame) noexcept :
			context_(std::exchange(frame.context_, nullptr))
		{}

		~Frame() noexcept {
			if (context_) {
				context_->endFrame();
			}
		}

		void render() noexcept;

	private:
		Frame(Context & context) noexcept :
			context_(&context)
		{
			context_->beginFrame();
		}

		Context * context_ = nullptr;

		friend class Context;
	};

	static std::unique_ptr<Context> create();

	~Context() noexcept;

	Lock lock() noexcept;

	Frame newFrame() {
		return Frame(*this);
	}

private:
	Context(ImGuiContext * context) noexcept :
		context_(context)
	{}

	void _unlock() noexcept;

	void beginFrame();
	void endFrame();

	ImGuiContext * const context_;
	int lockRefCount_ = 0;
};




class Graphics {
public:
	class Frame {
	public:
		Frame(Frame && frame) noexcept :
			graphics_(std::exchange(frame.graphics_, nullptr))
		{}

		~Frame() noexcept {
			if (graphics_) {
				graphics_->endFrame();
			}
		}

	private:
		Frame(Graphics & graphics) noexcept :
			graphics_(&graphics)
		{
			graphics_->beginFrame();
		}

		Graphics * graphics_ = nullptr;

		friend class Graphics;
	};

	static std::unique_ptr<Graphics> createOpenGL(Context & context);

	virtual ~Graphics() noexcept;

	Frame newFrame() {
		return Frame(*this);
	}

	virtual void render() = 0;

protected:
	virtual void beginFrame() = 0;
	virtual void endFrame() = 0;

private:
	Graphics(Context & context) noexcept;

	Context & context_;

	friend class detail::OpenGLGraphics;
};




} // imgui
