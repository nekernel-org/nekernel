/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

namespace Kernel
{
	class UserProcess;
    class KernelProcess;
	class UserProcessTeam;

	/***********************************************************************************/
	/// @brief Subsystem enum type.
	/***********************************************************************************/

	enum class ProcessSubsystem : Int32
	{
		kProcessSubsystemSecurity = 100,
		kProcessSubsystemApplication,
		kProcessSubsystemService,
		kProcessSubsystemDriver,
		kProcessSubsystemInvalid = 256U,
		kProcessSubsystemCount	 = 4,
	};

	typedef UInt64 PTime;

	/***********************************************************************************/
	//! @brief Local Process identifier.
	/***********************************************************************************/
	typedef Int64 ProcessID;

	/***********************************************************************************/
	//! @brief Local Process status enum.
	/***********************************************************************************/
	enum class ProcessStatusKind : Int32
	{
		kInvalid,
		kStarting,
		kRunning,
		kKilled,
		kFrozen,
		kFinished,
		kCount,
	};

	/***********************************************************************************/
	//! @brief Affinity is the amount of nano-seconds this process is going to run.
	/***********************************************************************************/
	enum class AffinityKind : Int32
	{
		kRealTime	  = 500,
		kVeryHigh	  = 250,
		kHigh		  = 200,
		kStandard	  = 1000,
		kLowUsage	  = 1500,
		kVeryLowUsage = 2000,
	};

	/***********************************************************************************/
	//! Operators for AffinityKind
	/***********************************************************************************/

	inline bool operator<(AffinityKind lhs, AffinityKind rhs)
	{
		Int32 lhs_int = static_cast<Int>(lhs);
		Int32 rhs_int = static_cast<Int>(rhs);

		return lhs_int < rhs_int;
	}

	inline bool operator>(AffinityKind lhs, AffinityKind rhs)
	{
		Int32 lhs_int = static_cast<Int>(lhs);
		Int32 rhs_int = static_cast<Int>(rhs);

		return lhs_int > rhs_int;
	}

	inline bool operator<=(AffinityKind lhs, AffinityKind rhs)
	{
		Int32 lhs_int = static_cast<Int>(lhs);
		Int32 rhs_int = static_cast<Int>(rhs);

		return lhs_int <= rhs_int;
	}

	inline bool operator>=(AffinityKind lhs, AffinityKind rhs)
	{
		Int32 lhs_int = static_cast<Int>(lhs);
		Int32 rhs_int = static_cast<Int>(rhs);

		return lhs_int >= rhs_int;
	}

	using ProcessTime = UInt64;
	using PID		  = Int64;

	/***********************************************************************************/
	/// @note For User manager, tells where we run the code.
	/***********************************************************************************/
	enum class ProcessLevelRing : Int32
	{
		kRingStdUser   = 1,
		kRingSuperUser = 2,
		kRingGuestUser = 5,
		kRingCount	   = 5,
	};

	/***********************************************************************************/
	/// @brief Helper type to describe a code image.
	/***********************************************************************************/
	using ImagePtr = VoidPtr;

	struct ProcessImage final
	{
		explicit ProcessImage() = default;

		ImagePtr fCode;
		ImagePtr fBlob;

		Bool HasCode()
		{
			return this->fCode != nullptr;
		}

		Bool HasImage()
		{
			return this->fBlob != nullptr;
		}
	};
} // namespace Kernel
