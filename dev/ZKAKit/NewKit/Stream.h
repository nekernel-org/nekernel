
/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>
#include <NewKit/Ref.h>

namespace Kernel
{
	template <typename StreamTrait, typename Kind>
	class Stream final
	{
	public:
		explicit Stream(Ref<Stream> ref)
			: fStream(ref)
		{
		}

		~Stream() = default;

		Stream& operator=(const Stream&) = default;
		Stream(const Stream&)			 = default;

		template <typename Data>
		friend Stream<StreamTrait, Kind>& operator>>(Stream<StreamTrait, Kind>& Ks, Ref<Data>& Buf)
		{
			Ks.fKind = Ks.fStream->In(Buf);
			return *Ks;
		}

		template <typename Data>
		friend Stream<StreamTrait, Kind>& operator<<(Stream<StreamTrait, Kind>& Ks, Ref<Data>& Buf)
		{
			Ks.fKind = Buf;
			Ks.fStream->Out(Buf.Leak());
			return *Ks;
		}

		Ref<StreamTrait>& AsStreamTrait()
		{
			return fStream;
		}

		Ref<Kind>& AsType()
		{
			return fKind;
		}

	private:
		Ref<StreamTrait> fStream;
		Ref<Kind>		 fKind;
	};
} // namespace Kernel
