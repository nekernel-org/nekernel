/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <Modules/PS2/PS2MouseInterface.hxx>
#include <Modules/CoreCG/FbRenderer.hxx>
#include <Modules/CoreCG/Rsrc/Cursor.rsrc>
#include <KernelKit/Framebuffer.hxx>
#include <NewKit/Defines.hxx>

/// @note forward decl.
EXTERN_C Kernel::Boolean _hal_draw_mouse();
EXTERN_C Kernel::Void _hal_init_mouse();

STATIC Kernel::Int32 kPrevX		 = 10;
STATIC Kernel::Int32 kPrevY		 = 10;
STATIC Kernel::Int32 kX			 = 10;
STATIC Kernel::Int32 kY			 = 10;
STATIC Kernel::Int32 kMouseCycle = 0;
STATIC Kernel::Char kMousePacket[4]		 = {};
STATIC Kernel::Boolean kMousePacketReady = false;

STATIC CGInit();

#define kPS2Leftbutton	 0b00000001
#define kPS2Middlebutton 0b00000010
#define kPS2Rightbutton	 0b00000100
#define kPS2XSign		 0b00010000
#define kPS2YSign		 0b00100000
#define kPS2XOverflow	 0b01000000
#define kPS2YOverflow	 0b10000000

using namespace Kernel;

EXTERN_C Void hal_handle_mouse()
{
	Kernel::UInt8 data = HAL::In8(0x60);

	switch (kMouseCycle)
	{
	case 0:
		if (kMousePacketReady)
			break;
		if ((data & 0b00001000) == 0)
			break;
		kMousePacket[0] = data;
		kMouseCycle++;
		break;
	case 1:
		if (kMousePacketReady)
			break;
		kMousePacket[1] = data;
		kMouseCycle++;
		break;
	case 2:
		if (kMousePacketReady)
			break;
		kMousePacket[2]	  = data;
		kMousePacketReady = true;
		kMouseCycle		  = 0;
		break;
	}

	// Notify PIC controller that we're done with it's interrupt.

	Kernel::HAL::Out8(0x20, 0x20);
	Kernel::HAL::Out8(0xA0, 0x20);
}

EXTERN_C Boolean _hal_left_button_pressed()
{
	return kMousePacket[0] & kPS2Leftbutton;
}
EXTERN_C Boolean _hal_right_button_pressed()
{
	return kMousePacket[0] & kPS2Rightbutton;
}
EXTERN_C Boolean _hal_middle_button_pressed()
{
	return kMousePacket[0] & kPS2Middlebutton;
}

/// @brief Draws the kernel's mouse.
EXTERN_C Boolean _hal_draw_mouse()
{
	if (!kMousePacketReady)
		return false;

	bool xNegative, yNegative, xOverflow, yOverflow;

	if (kMousePacket[0] & kPS2XSign)
	{
		xNegative = true;
	}
	else
		xNegative = false;

	if (kMousePacket[0] & kPS2YSign)
	{
		yNegative = true;
	}
	else
		yNegative = false;

	if (kMousePacket[0] & kPS2XOverflow)
	{
		xOverflow = true;
	}
	else
		xOverflow = false;

	if (kMousePacket[0] & kPS2YOverflow)
	{
		yOverflow = true;
	}
	else
		yOverflow = false;

	if (!xNegative)
	{
		kX += kMousePacket[1];
		if (xOverflow)
		{
			kX += 255;
		}
	}
	else
	{
		kMousePacket[1] = 256 - kMousePacket[1];
		kX -= kMousePacket[1];
		if (xOverflow)
		{
			kX -= 255;
		}
	}

	if (!yNegative)
	{
		kY -= kMousePacket[2];
		if (yOverflow)
		{
			kY -= 255;
		}
	}
	else
	{
		kMousePacket[2] = 256 - kMousePacket[2];
		kY += kMousePacket[2];
		if (yOverflow)
		{
			kY += 255;
		}
	}

	if (kX < 0)
		kX = 0;
	if (kX > kHandoverHeader->f_GOP.f_Width - 8)
		kX = kHandoverHeader->f_GOP.f_Width - 8;

	if (kY < 0)
		kY = 0;
	if (kY > kHandoverHeader->f_GOP.f_Height - 16)
		kY = kHandoverHeader->f_GOP.f_Height - 16;

	/// Draw mouse here.

	kPrevX = kX;
	kPrevY = kY;

	CGDrawBitMapInRegionA(Cursor, cCurHeight, cCurWidth, kY, kX);

	kMousePacketReady = false;
	return true;
}

/// @brief Init kernel mouse.
EXTERN_C Void _hal_init_mouse()
{
    kPrevX		 = 10;
    kPrevY		 = 10;
    kX			 = 10;
    kY			 = 10;
    kMouseCycle = 0;
    kMousePacketReady = false;

    Kernel::PS2MouseInterface ps2_mouse;
	ps2_mouse.Init();
}
