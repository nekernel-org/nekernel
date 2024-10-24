/*
 * Copyright (c) 2024 ZKA Web Services Co
 */

#include <FireWall/FireWall.h>

struct FW_CONNECTION_ENTRY
{
	Bool is_ipv6, is_allowed;
	Char f_ip_address;
	SizeT f_ip_address_len;
};

IMPORT_C Bool FwAllowConnection(const Char* ip_address, SizeT ip_address_len, Bool is_ipv6)
{
	if (!ip_address ||
		!ip_address_len)
		return No;



	return Yes;
}

IMPORT_C Bool FwForbidConnection(const Char* ip_address, SizeT ip_address_len, Bool is_ipv6)
{
	if (!ip_address ||
		!ip_address_len)
		return No;



	return Yes;
}
