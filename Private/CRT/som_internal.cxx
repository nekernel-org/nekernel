/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include "__som.hxx"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

static void __som_error(const char* msg)
{
    printf("%s\n", "*****************************************");
    printf("%s\n", msg);
    printf("%s\n", "*****************************************");

    exit(-33);
}

static int __som_release(Handle handle)
{
    printf("%s\n", "*****************************************");
    printf("%s\n", "HUnknown, default behavior.");
    printf("%s\n", "*****************************************");

    free(handle);
    return 0;
}

extern "C" Handle __som_send(int sel, int len, ...)
{
    switch (sel)
    {
    case kSomCreate:
    {
        va_list list;
        va_start(list, len);

        int id = va_arg(list, int);

        Handle handle = nullptr;

        switch (id)
        {
        case kHUnknown:
            handle = (Handle)malloc(sizeof(struct _Handle));
            handle->Release = __som_release;
            handle->_Cls = (_HandleCls)handle;
            break;
        
        default:
            __som_error("SOM: Invalid Selector!");
            break;
        }

        va_end(list);

        return handle;
    }
    default:
        __som_error("SOM: Invalid Selector!");
        return nullptr;
    }
}