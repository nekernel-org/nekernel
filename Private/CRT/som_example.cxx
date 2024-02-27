/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include "__som.hxx"

int main(int argc, char const *argv[])
{
    Handle handle_type = (Handle)__som_send(kSomCreate, 1, kHUnknown);
    handle_type->Release(handle_type);

    return 0;
}
