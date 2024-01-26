/*
 *	========================================================
 *
 *	newBoot
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include "API.hxx"
#include "Detail.hxx"

#define kFilesR 0x01 /* read-only */
#define kFilesH 0x02 /* hidden */
#define kFilesS 0x04 /* system */
#define kFilesL 0x08 /* volume label */
#define kFilesD 0x10 /* directory */
#define kFilesZ 0x20 /* archive */

// @brief Array of unused bits.
#define kFilesU { 0x40, 0x80 }

namespace mpt::detail
{
    struct Files32FileHdr final
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

    struct Files32FileGroup final
    {
        Files32FileHdr* fHdr{ nullptr };

        Files32FileGroup* fUpper{ nullptr };
        Files32FileGroup* fLower{ nullptr };
        Files32FileGroup* fPrev{ nullptr };
        Files32FileGroup* fNext{ nullptr };
    };

    /* @brief external inits */
    extern "C" int init_ata_mpt(void);
    extern "C" int init_mpt(void);

    Files32FileGroup* kRootGroup = nullptr;
}

namespace mpt
{
    bool init_mpt() noexcept
    {
        detail::kRootGroup = detail::new_class<detail::Files32FileGroup>();

        assert(detail::kRootGroup != nullptr);
        assert(detail::init_ata_mpt() == detail::okay);
        assert(detail::init_mpt() == detail::okay);

        return true;
    }
}