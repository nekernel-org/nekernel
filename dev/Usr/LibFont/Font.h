/**
 *
 * Copyright (c) 2024 Amlal El Mahrouss
 *
 */

#pragma once

#include <LibCF/Core.h>

#define kCFFontExt ".ttf"

/// @file Font.h
/// @brief Font parsing using a NeOS True Font.

namespace LibCF
{
	class CFFont;

	class CFFont
	{
	public:
		explicit CFFont() = default;
		virtual ~CFFont() = default;

	protected:
		CFRect		m_size{};
		CFRect*		m_glyphs{nullptr};
		CFInteger32 m_glyphs_cnt{0};
		BOOL		m_bold{NO};
		BOOL		m_italic{NO};

		CFFont(const CFFont& fnt)			 = delete;
		CFFont& operator=(const CFFont& fnt) = delete;

		virtual BOOL render_(CFPoint pos, CFChar16 character, CFColor color) = 0;

		virtual void dispose_()
		{
			m_bold	 = NO;
			m_italic = NO;

			m_size.height = 0;
			m_size.width  = 0;

			m_glyphs_cnt = 0;

			delete[] m_glyphs;
			m_glyphs = nullptr;

			m_size.x = 0;
			m_size.y = 0;
		}
	};
} // namespace LibCF