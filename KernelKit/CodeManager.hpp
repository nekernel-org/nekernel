/*
*	========================================================
*
*	hCore
* 	Copyright Mahrouss Logic, all rights reserved.
*
* 	========================================================
*/

#ifndef _INC_CODE_MANAGER_
#define _INC_CODE_MANAGER_

#include <KernelKit/PEF.hpp>
#include <NewKit/ErrorOr.hpp>
#include <NewKit/String.hpp>

namespace hCore
{
    ///
    /// \name PEFLoader
    /// PEF container format implementation.
    ///
    class PEFLoader : public Loader
    {
        PEFLoader() = delete;

    public:
        explicit PEFLoader(const char* path);
        ~PEFLoader() override;

    public:
        HCORE_COPY_DEFAULT(PEFLoader);

    public:
        typedef void(*MainKind)(void);

    public:
    	const char* Path() override;
        const char* Format() override;
        const char* MIME() override;

    public:
        ErrorOr<VoidPtr> LoadStart() override;
        VoidPtr FindSymbol(const char* name, Int32 kind) override;

    public:
        bool IsLoaded() noexcept;

    private:
    	Ref<StringView> fPath;
        VoidPtr fCachedBlob;
        bool fBad;

    };

    namespace Utils
    {
        /// \brief Much like Mac OS's UPP.
        /// It handles different kind of code.
        /// PowerPC <-> AMD64 for example.
        typedef struct UniversalProcedureTable
        {
        public:
            Char symbolName[kPefNameLen];
            VoidPtr symbolPtr;
            SizeT symbolArchitecture;

        } UniversalProcedureTableType;

        bool execute_from_image(PEFLoader& exec);
    }
}

#endif // ifndef _INC_CODE_MANAGER_
