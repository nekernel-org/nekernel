/* -------------------------------------------

	Copyright Zeta Electronics Corporation.

	File: newstd.hxx.
	Purpose: NewOS standard interface.

------------------------------------------- */

#pragma once

#ifdef __KERNEL__
#error !!! including header in kernel mode !!!
#endif // __KERNEL__

class NUser; /// @brief User application class.
class NCallback; /// @brief User callback class.

#define IMPORT_CXX extern "C++"
#define IMPORT_C extern "C"

/**
@brief a class which helps the user do application I/O.
*/
class NUser 
{
public:

};