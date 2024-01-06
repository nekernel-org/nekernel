/*
 *	========================================================
 *
 *	newBoot
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

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
