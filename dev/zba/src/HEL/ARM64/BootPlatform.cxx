/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <BootKit/Platform.hxx>
#include <BootKit/Protocol.hxx>
#include <BootKit/BootKit.hxx>

#ifdef __STANDALONE__

using namespace Boot;

EXTERN_C void rt_hlt()
{
    while (Yes);
}

EXTERN_C void rt_cli()
{

}

EXTERN_C void rt_sti()
{

}

EXTERN_C void rt_cld()
{

}

EXTERN_C void rt_std()
{

}

#endif // __STANDALONE__
