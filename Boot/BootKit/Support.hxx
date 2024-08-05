/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#pragma once

/// @file Support.hxx
/// @brief Purpose of this file is to help port libs into the bootloader.

#define cLongMax ((long)(~0UL >> 1))
#define cLongMin (~cLongMax)

#ifdef __NEWOSLDR__

#define SetMem(dst, c, sz)	  memset(dst, c, sz)
#define MoveMem(dst, src, sz) memcpy(dst, src, sz)
#define CopyMem(dst, src, sz) memcpy(dst, src, sz)
#define StrLen(src)			  strlen(src)
#define StrCmp(dst, src) strcmp(dst, src)

#endif // __NEWOSLDR__

inline int IsSpace(int c)
{
	return c == ' ';
}

inline int StringNCompare(const char* destination, const char* source, long length)
{
	long err = 0;

	for (long i = 0UL; i < length; ++i)
	{
		if (source[i] != destination[i])
			++err;
	}

	return err;
}

inline long StringToLong(const char* nptr, char** endptr, int base)
{
	const char *p	   = nptr, *endp;
	bool		is_neg = 0, overflow = 0;

	/* Need unsigned so (-cLongMin) can fit in these: */
	unsigned long n = 0UL, cutoff;
	int			  cutlim;

	if (base < 0 || base == 1 || base > 36)
	{
		return 0L;
	}

	endp = nptr;

	while (IsSpace(*p))
		p++;

	if (*p == '+')
	{
		p++;
	}
	else if (*p == '-')
	{
		is_neg = 1, p++;
	}
	if (*p == '0')
	{
		p++;
		/* For strtol(" 0xZ", &endptr, 16), endptr should point to 'x';
		 * pointing to ' ' or '0' is non-compliant.
		 * (Many implementations do this wrong.) */
		endp = p;
		if (base == 16 && (*p == 'X' || *p == 'x'))
		{
			p++;
		}
		else if (base == 2 && (*p == 'B' || *p == 'b'))
		{
			/* C23 standard supports "0B" and "0b" prefixes. */
			p++;
		}
		else if (base == 0)
		{
			if (*p == 'X' || *p == 'x')
			{
				base = 16, p++;
			}
			else if (*p == 'B' || *p == 'b')
			{
				base = 2, p++;
			}
			else
			{
				base = 8;
			}
		}
	}
	else if (base == 0)
	{
		base = 10;
	}

	cutoff = (is_neg) ? -(cLongMin / base) : cLongMax / base;
	cutlim = (is_neg) ? -(cLongMin % base) : cLongMax % base;

	while (1)
	{
		int c;
		if (*p >= 'A')
			c = ((*p - 'A') & (~('a' ^ 'A'))) + 10;
		else if (*p <= '9')
			c = *p - '0';
		else
			break;
		if (c < 0 || c >= base)
			break;
		endp = ++p;
		if (overflow)
		{
			/* endptr should go forward and point to the non-digit character
			 * (of the given base); required by ANSI standard. */
			if (endptr)
				continue;
			break;
		}
		if (n > cutoff || (n == cutoff && c > cutlim))
		{
			overflow = 1;
			continue;
		}
		n = n * base + c;
	}

	if (endptr)
		*endptr = (char*)endp;

	if (overflow)
	{
		return ((is_neg) ? cLongMin : cLongMax);
	}

	return (long)((is_neg) ? -n : n);
}
