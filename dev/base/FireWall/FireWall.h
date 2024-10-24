/*
 * Copyright (c) 2024 ZKA Web Services Co
 */

#pragma once

#include <sci/sci_base.h>

IMPORT_C Bool FwAllowConnection(const Char* ip_address, SizeT ip_address_len, Bool is_ipv6);

IMPORT_C Bool FwForbidConnection(const Char* ip_address, SizeT ip_address_len, Bool is_ipv6);
