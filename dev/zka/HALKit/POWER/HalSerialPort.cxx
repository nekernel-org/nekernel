/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <HALKit/POWER/Processor.hxx>
#include <KernelKit/DebugOutput.hxx>

using namespace Kernel;

/// @brief Writes to COM1.
/// @param bytes
void ke_io_write(const Char* bytes)
{
	if (!bytes)
		return;

	SizeT index = 0;
	SizeT len	= rt_string_len(bytes, 255);

	while (index < len)
	{
		// TODO
		++index;
	}
}
