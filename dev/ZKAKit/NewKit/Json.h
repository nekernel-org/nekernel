
/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

// last-rev: 30/01/24

#include <CompilerKit/CompilerKit.h>
#include <NewKit/Defines.h>
#include <NewKit/Stream.h>
#include <NewKit/KString.h>
#include <NewKit/Utils.h>

#define kMaxJsonPath 4096
#define kJSONLen	 32
#define kJSONNull	 "null"

namespace Kernel
{
	/// @brief Json class
	class JSON final
	{
	public:
		explicit JSON()
		{
			auto	len = kJSONLen;
			KString key = KString(len);
			key += kJSONNull;

			this->AsKey()	= key;
			this->AsValue() = key;
		}

		explicit JSON(SizeT lhsLen, SizeT rhsLen)
			: fKey(lhsLen), fValue(rhsLen)
		{
		}

		~JSON() = default;

		ZKA_COPY_DEFAULT(JSON);

		const Bool& IsUndefined()
		{
			return fUndefined;
		}

	private:
		Bool	fUndefined; // is this instance undefined?
		KString fKey;
		KString fValue;

	public:
		/// @brief returns the key of the json
		/// @return the key as string view.
		KString& AsKey()
		{
			return fKey;
		}

		/// @brief returns the value of the json.
		/// @return the key as string view.
		KString& AsValue()
		{
			return fValue;
		}

		static JSON kNull;
	};

	/// @brief Json stream reader helper.
	struct JsonStreamReader final
	{
		STATIC JSON In(const Char* full_array)
		{
			auto	start_val	= '{';
			auto	end_val		= '}';
			Boolean probe_value = false;

			if (full_array[0] != start_val)
			{
				if (full_array[0] != '[')
					return JSON::kNull;

				start_val = '[';
				end_val	  = ']';

				probe_value = true;
			}

			SizeT len = rt_string_len(full_array);

			SizeT key_len	= 0;
			SizeT value_len = 0;

			JSON type(kMaxJsonPath, kMaxJsonPath);

			for (SizeT i = 1; i < len; ++i)
			{
				if (full_array[i] == '\r' ||
					full_array[i] == '\n')
					continue;

				if (probe_value)
				{
					if (full_array[i] == end_val ||
						full_array[i] == ',')
					{
						probe_value = false;

						++value_len;
					}
					else
					{
						type.AsValue().Data()[value_len] = full_array[i];

						++value_len;
					}
				}
				else
				{
					if (start_val == '[')
						continue;

					if (full_array[i] == ':')
					{
						probe_value					 = true;
						type.AsKey().Data()[key_len] = 0;
						++key_len;
					}
					else
					{
						type.AsKey().Data()[key_len] = full_array[i];

						++key_len;
					}
				}
			}

			type.AsValue().Data()[value_len] = 0;

			return type;
		}
	};

	using JsonStream = Stream<JsonStreamReader, JSON>;
} // namespace Kernel
