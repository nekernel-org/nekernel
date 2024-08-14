/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <CompilerKit/CompilerKit.hxx>
#include <HintKit/CompilerHint.hxx>
#include <NewKit/Defines.hxx>
#include <NewKit/ErrorOr.hxx>

namespace Kernel
{
	/// @brief This interface is used to make loader contracts (MSCOFF, PEF).
	/// @author @Amlal-El-Mahrouss
	class LoaderInterface
	{
	public:
		explicit LoaderInterface() = default;
		virtual ~LoaderInterface() = default;

		NEWOS_COPY_DEFAULT(LoaderInterface);

	public:
		virtual _Output const char* AsString()											= 0;
		virtual _Output const char* MIME()														= 0;
		virtual _Output const char* Path()														= 0;
		virtual _Output ErrorOr<VoidPtr> FindStart()											= 0;
		virtual _Output VoidPtr			 FindSymbol(_Input const char* name, _Input Int32 kind) = 0;
	};
} // namespace Kernel
