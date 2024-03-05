/** 
*   The HCore Kit
*   Copyright Mahrouss Logic
*   File: HCObject.h
*   Purpose: Base HCore object
*/

/// HCUtf8Raw, HCUtf8RawPtr, kVirtualMemorySize, rt_kernel_port, rt_kernel_team
#import <HCoreKit/base_api.h>

/// @brief Base HCore object
/// @version 1.0

@interface HCObject {
    HCUtf8RawPtr fClsName;
}

-(id) init;
-(HCUtf8RawPtr) toString;
@end // interface HCObject