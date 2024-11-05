/* -------------------------------------------

	Copyright EL Mahrouss Logic.

------------------------------------------- */

#include <NewKit/Utils.h>

namespace Kernel
{
	Int32 rt_string_cmp(const Char* src, const Char* cmp, Size size)
	{
		if (!cmp ||
			!src)
			return 1;

		Int32 counter = 0;

		for (Size index = 0; index < size; ++index)
		{
			if (src[index] != cmp[index])
				++counter;
		}

		return counter;
	}

	Void rt_zero_memory(voidPtr pointer, Size len)
	{
		rt_set_memory(pointer, 0, len);
	}

	SizeT rt_string_len(const Char* str, SizeT _len)
	{
		SizeT len{0};

		do
		{
			if (len > _len)
			{
				return _len;
			}

			++len;
		} while (str[len] != '\0');

		return len;
	}

	Size rt_string_len(const Char* ptr)
	{
		SizeT cnt{0};

		do
		{
			++cnt;
		} while (ptr[cnt] != 0);

		return cnt;
	}

	voidPtr rt_set_memory(voidPtr src, UInt32 value, Size len)
	{
		if (!src || len < 1)
			return nullptr;

		UInt32* start = reinterpret_cast<UInt32*>(src);

		while (len)
		{
			*start = value;
			++start;
			--len;
		}

		return (voidPtr)start;
	}

	Int rt_move_memory(const voidPtr src, voidPtr dst, Size len)
	{
		if (len < 1)
			return 2;

		if (!src || !dst)
			return 1;

		Char* srcChr  = reinterpret_cast<Char*>(src);
		Char* dstChar = reinterpret_cast<Char*>(dst);
		SizeT index	  = 0;

		while (index < len)
		{
			dstChar[index] = srcChr[index];
			srcChr[index]  = 0;

			++index;
		}

		return 0;
	}

	Int rt_copy_memory(const voidPtr src, voidPtr dst, Size len)
	{
		if (len < 1)
			return 0;

		char* srcChr  = reinterpret_cast<char*>(src);
		char* dstChar = reinterpret_cast<char*>(dst);
		Size  index	  = 0;

		while (index < len)
		{
			dstChar[index] = srcChr[index];
			++index;
		}

		return index;
	}

	const Char* alloc_string(const Char* text)
	{
		if (!text)
			return nullptr;

		const Char* string = new Char[rt_string_len(text)];

		if (!string)
			return nullptr;

		voidPtr vText = reinterpret_cast<voidPtr>(const_cast<char*>(text));
		voidPtr vStr  = reinterpret_cast<voidPtr>(const_cast<char*>(string));

		rt_copy_memory(vText, vStr, rt_string_len(text));

		return string;
	}

	Int32 rt_to_uppercase(Int32 character)
	{
		if (character >= 'a' && character <= 'z')
			return character - 0x20;

		return character;
	}

	Int32 rt_to_lower(Int32 character)
	{
		if (character >= 'A' && character <= 'Z')
			return character + 0x20;

		return character;
	}

	Bool rt_to_string(Char* str, Int32 limit, Int32 base)
	{
		if (limit == 0)
			return false;

		Int copy_limit = limit;
		Int cnt		   = 0;
		Int ret		   = base;

		while (limit != 1)
		{
			ret		 = ret % 10;
			str[cnt] = ret;

			++cnt;
			--limit;
			--ret;
		}

		str[copy_limit] = '\0';
		return true;
	}

	Boolean is_space(Char chr)
	{
		return chr == ' ';
	}

	Boolean is_newln(Char chr)
	{
		return chr == '\n';
	}

	voidPtr rt_string_in_string(const Char* in, const Char* needle)
	{
		for (SizeT i = 0; i < rt_string_len(in); ++i)
		{
			if (rt_string_cmp(in + i, needle, rt_string_len(needle)) == 0)
				return reinterpret_cast<voidPtr>(const_cast<char*>(in + i));
		}

		return nullptr;
	}

	// @brief Checks for a string start at the character.

	Char* rt_string_has_char(Char* str, const Char chr)
	{
		while (*str != chr)
		{
			++str;

			if (*str == 0)
				return nullptr;
		}

		return str;
	}
} // namespace Kernel

EXTERN_C Kernel::VoidPtr memset(Kernel::VoidPtr dst, Kernel::UInt32 c, Kernel::Size len)
{
	return Kernel::rt_set_memory(dst, c, len);
}
