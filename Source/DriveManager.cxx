/*
 *	========================================================
 *
 *	hCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <FSKit/NewFS.hxx>
#include <NewKit/Utils.hpp>

#include <KernelKit/DebugOutput.hpp>
#include <KernelKit/DriveManager.hpp>

namespace hCore
{
    DriveSelector::DriveSelector() : fDrive(nullptr) {}
    
    DriveSelector::~DriveSelector()
    {
    	if (fDrive)
    	{
            this->Unmount();
    	}	
    }
	
    DriveTraits& DriveSelector::GetMounted() { MUST_PASS(fDrive != nullptr); return *fDrive; }
    
    bool DriveSelector::Mount(DriveTraits* drive)
    {
    	if (drive &&
            drive->fReady() &&
            fDrive == nullptr)
        {
            fDrive = drive;
            fDrive->fMount();

            kcout << "[Mount] Mount drive: " << fDrive->fName << "\n";

            return true;
        }
        
        return false;
    }

    DriveTraits* DriveSelector::Unmount()
    {
        if (!fDrive)
            return nullptr;

        auto drivePointer = fDrive;

        fDrive->fUnmount();
        fDrive = nullptr;

        kcout << "[Unmount] Mount drive: " << drivePointer->fName << "\n";

        return drivePointer;
    }
} // namespace hCore
