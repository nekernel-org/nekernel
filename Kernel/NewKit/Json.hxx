
/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#pragma once

// last-rev: 30/01/24

#include <CompilerKit/CompilerKit.hxx>
#include <NewKit/Defines.hpp>
#include <NewKit/Stream.hpp>
#include <NewKit/String.hpp>
#include <NewKit/Utils.hpp>

#define cMaxJsonPath 4096

namespace Kernel
{
	/// @brief Json value class
	class JsonType final
	{
	public:
		explicit JsonType()
			: Kernel::JsonType(1, 1)
		{
		}

		explicit JsonType(SizeT lhsLen, SizeT rhsLen)
			: fKey(lhsLen), fValue(rhsLen)
		{
		}

		~JsonType() = default;

		NEWOS_COPY_DEFAULT(JsonType);

	private:
		StringView fKey;
		StringView fValue;

	public:
		/// @brief returns the key of the json
		/// @return the key as string view.
		StringView& AsKey()
		{
			return fKey;
		}

		/// @brief returns the value of the json.
		/// @return the key as string view.
		StringView& AsValue()
		{
			return fValue;
		}

		static JsonType kUndefined;
	};

	/// @brief Json stream reader helper.
	struct JsonStreamReader final
	{
		STATIC JsonType In(const Char* full_array)
		{
			if (full_array[0] != '{')
				return JsonType::kUndefined;

			SizeT	len			= rt_string_len(full_array);
			Boolean probe_value = false;

			SizeT key_len	= 0;
			SizeT value_len = 0;

			JsonType type(cMaxJsonPath, cMaxJsonPath);

			for (SizeT i = 1; i < len; ++i)
			{
				if (full_array[i] == '\r' ||
					full_array[i] == '\n')
					continue;

				if (probe_value)
				{
					if (full_array[i] == '}' ||
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

	using JsonStream = Stream<JsonStreamReader, JsonType>;
} // namespace Kernel
