/*
 *	========================================================
 *
 *	newBoot
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include "API.hxx"

struct Files32FileHdr
{
    char Filename[32];
    char Ext[3];
    char Attr;
    char Case;
    char CreateMs;
    unsigned short Create;
    unsigned short CreateDate;
    unsigned short LastAccess;
    unsigned short Timestamp;
    unsigned short Datestamp;
    unsigned short StartLba;
    unsigned int   SizeFile;
};

#define kFilesR 0x01 /* read-only */
#define kFilesH 0x02 /* hidden */
#define kFilesS 0x04 /* system */
#define kFilesL 0x08 /* volume label */
#define kFilesD 0x10 /* directory */
#define kFilesZ 0x20 /* archive */

// @brief Array of unused bits.
#define kFilesU { 0x40, 0x80 }

struct Files32FileGroup {
    Files32FileHdr* fHdr{ nullptr };

    Files32FileGroup* fUpper{ nullptr };
    Files32FileGroup* fLower{ nullptr };
    Files32FileGroup* fPrev{ nullptr };
    Files32FileGroup* fNext{ nullptr };
} kRootGroup = nullptr;

extern "C" Assert(bool expr);
extern "C" void* AllocPtr(long sz);

namespace detail
{
template <typename Cls>
Cls* new_class()
{
    Cls* cls = (Cls*)AllocPtr(sizeof(Cls));
    *cls = Cls();

    return cls;
}
}

namespace mpt
{
bool filesystem_init(void)
{
    kRootGroup = detail::new_class<Files32FileGroup>();
    Assert(kRootGroup != nullptr);

    return true;
}
}