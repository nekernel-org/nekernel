/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Stream.hpp>
#include <NewKit/Ref.hpp>
#include <FSKit/NewFS.hxx>

#include <NewKit/ErrorID.hpp>
#include <NewKit/Ref.hpp>

/// Main filesystem abstraction manager.

#define kBootFolder "/boot"
#define kBinFolder "/bin"
#define kShLibsFolder "/lib"

#define kSectorSz 512

/// refer to first enum.
#define kFileOpsCount 4

namespace hCore
{
    enum
    {
        kFileWriteAll = 100,
        kFileReadAll = 101,
        kFileReadChunk = 102,
        kFileWriteChunk = 103,
    };

    typedef VoidPtr NodePtr;

    class IFilesystemManager
    {
    public:
        IFilesystemManager() = default;
        virtual ~IFilesystemManager() = default;

    public:
        HCORE_COPY_DEFAULT(IFilesystemManager);

    public:
        static bool Mount(IFilesystemManager* pMount);
        static IFilesystemManager* Unmount();
        static IFilesystemManager* GetMounted();

    public:
        virtual NodePtr Create(const char* path) = 0;
        virtual NodePtr CreateAlias(const char* path) = 0;
        virtual NodePtr CreateDirectory(const char* path) = 0;

    public:
        virtual bool Remove(const char* path) = 0;

    public:
        virtual NodePtr Open(const char* path, const char* r) = 0;

    public:
        virtual void Write(NodePtr node, VoidPtr data, Int32 flags) = 0;
        virtual VoidPtr Read(NodePtr node, Int32 flags, SizeT sz) = 0;

    public:
        virtual bool Seek(NodePtr node, SizeT off) = 0;

    public:
        virtual SizeT Tell(NodePtr node) = 0;
        virtual bool Rewind(NodePtr node) = 0;

    };

    class NewFilesystemManager final : public IFilesystemManager
    {
    public:
        NewFilesystemManager();
        ~NewFilesystemManager() override;

    public:
        HCORE_COPY_DEFAULT(NewFilesystemManager);

    public:
        NodePtr Create(const char* path) override;
        NodePtr CreateAlias(const char* path) override;
        NodePtr CreateDirectory(const char* path) override;

    public:
        bool Remove(const char* node) override;

    public:
        NodePtr Open(const char* path, const char* r) override { return nullptr; }

    public:
        void Write(NodePtr node, VoidPtr data, Int32 flags) override { }
        VoidPtr Read(NodePtr node, Int32 flags, SizeT sz) override { return nullptr; }

    public:
        bool Seek(NodePtr node, SizeT off) override { return true; }

    public:
        SizeT Tell(NodePtr node) override { return 0; }
        bool Rewind(NodePtr node) override { this->Seek(node, 0); return true; }

    public:
        NewFSImpl* fIO{ nullptr };

    };

    template <typename Encoding = char, typename FSClass = NewFilesystemManager>
    class FileStream final
    {
    public:
        explicit FileStream(const Encoding* path);
        ~FileStream();

    public:
        FileStream &operator=(const FileStream &);                                                                         \
        FileStream(const FileStream &);

    public:
        ErrorOr<Int64> WriteAll(const VoidPtr data) noexcept
        {
            if (data == nullptr)
                return ErrorOr<Int64>(ME_INVALID_DATA);

            auto man = FSClass::GetMounted();

            if (man)
            {
                man->Write(fFile, data, kFileWriteAll);
                return ErrorOr<Int64>(0);
            }

            return ErrorOr<Int64>(ME_INVALID_DATA);
        }

        VoidPtr ReadAll() noexcept
        {
            auto man = FSClass::GetMounted();

            if (man)
            {
                VoidPtr ret = man->Read(fFile, kFileReadAll, 0);
                return ret;
            }

            return nullptr;
        }

        voidPtr Read(SizeT offset, SizeT sz)
        {
            auto man = FSClass::GetMounted();

            if (man)
            {
                man->Seek(fFile, offset);
                auto ret = man->Read(fFile, kFileReadChunk, sz);

                return ret;
            }

            return nullptr;
        }

        void Write(SizeT offset, voidPtr data, SizeT sz)
        {
            auto man = FSClass::GetMounted();

            if (man)
            {
                man->Seek(fFile, offset);
                man->Write(fFile, data, sz, kFileReadChunk);
            }
        }

    public:
        const char* MIME() noexcept { return fMime; }

    private:
        NodePtr fFile;
        Char* fMime{ "application-type/*" };

    };

    using FileStreamUTF8 = FileStream<char>;
    using FileStreamUTF16 = FileStream<wchar_t>;

    template <typename Encoding, typename Class>
    FileStream<Encoding, Class>::FileStream(const Encoding* path)
        : fFile(Class::GetMounted()->Open(path, "r+"))
    {}

    template <typename Encoding, typename Class>
    FileStream<Encoding, Class>::~FileStream() = default;
}

#define node_cast(PTR) reinterpret_cast<hCore::NodePtr>(PTR)


