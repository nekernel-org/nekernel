/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 * 	
 * 	========================================================
 */

#pragma once

#include <NewKit/Defines.hpp>

namespace hCore
{
	//! @brief Window Delegate system.
	class CWindowDelegate
	{
	public:
		CWindowDelegate() = default;
		~CWindowDelegate() = default;

	public:
		CWindowDelegate& operator=(const CWindowDelegate&) = default;
		CWindowDelegate(const CWindowDelegate&) = default;

		virtual void DispatchMessage(UInt32, VoidPtr, SizeT) = 0;

	};

	typedef CWindowDelegate* CWindowDelegatePtr;

	//! @brief A Window message manager.
	//! Takes care of sending message and delegates data to the window.
	class CWindowMessage final
	{
	public:
		explicit CWindowMessage() = default;
		~CWindowMessage() = default;
		
		CWindowMessage& operator=(const CWindowMessage&) = default;
		CWindowMessage(const CWindowMessage&) = default;
		
		//! @brief updates window with message, rpc data, and it's size.
		inline void operator()(UInt32 msg, VoidPtr rpc, SizeT rpcSize)
		{
			fWindowDelegate->DispatchMessage(msg, rpc, rpcSize);
		}
		
		CWindowDelegatePtr fWindowDelegate;

	};
	
	typedef VoidPtr CWindowRef;
}
