/* -------------------------------------------

 Copyright ZKA Technologies.

------------------------------------------- */

#include <UIRenderView.hxx>

/// @file Renderer.cxx
/// @brief 2D UI renderer, using GX APIs.

namespace System
{
	UIRenderView::UIRenderView()  = default;
	UIRenderView::~UIRenderView() = default;

	UIRenderEffect* UIRenderView::GetEffect(const SizeT& at)
	{
		if (!this->fEffects->Contains(at))
		{
			return nullptr;
		}

		return this->fEffects[at];
	}

	SizeT UIRenderView::AddEffect(const UIRenderEffect* effect)
	{
		if (!effect)
			return (SizeT)-1;

		return this->fEffects->Add(effect);
	}
} // namespace System
