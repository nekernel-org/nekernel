#include <KernelKit/Rsrc/Util.hxx>
#include <KernelKit/Rsrc/Cursor.rsrc>
#include <Drivers/PS2/PS2KernelMouse.hxx>
#include <NewKit/Defines.hpp>
#include <KernelKit/Framebuffer.hpp>

STATIC HCore::Int32 kPrevX = 0;
STATIC HCore::Int32 kPrevY = 0;
STATIC HCore::Int32 kX = 0;
STATIC HCore::Int32 kY = 0;
STATIC HCore::Int32 kMouseCycle = 0;
STATIC HCore::PS2KernelMouse kMousePS2;
STATIC HCore::Int32 kMousePacket[4];
STATIC HCore::Boolean kMousePacketReady = false;

#define kPS2LeftButton          0b00000001
#define kPS2MiddleButton        0b00000010
#define kPS2RightButton         0b00000100

#define kPS2XSign               0b00010000
#define kPS2YSign               0b00100000
#define kPS2XOverflow           0b01000000
#define kPS2YOverflow           0b10000000

EXTERN_C void _hal_mouse_handler()
{
#ifdef __DEBUG__
    HCore::UInt8 data = HCore::HAL::In8(0x60);

    switch (kMouseCycle)
    {
    case 0:
    {
        if (kMousePacketReady) break;
        if ((data & 0b00001000) == 0) break;

        kMousePacket[0] = data;
        ++kMouseCycle;

        break;
    }
    case 1:
    {
        if (kMousePacketReady) break;

        kMousePacket[1] = data;
        ++kMouseCycle;

        break;
    }
    case 2:
    {
        if (kMousePacketReady) break;

        kMousePacket[2] = data;
        ++kMouseCycle;
        kMousePacketReady = true;
        kMouseCycle = 0;

        break;
    }
    default:
        break;
    }
#endif
}

EXTERN_C void _hal_mouse_draw()
{
#ifdef __DEBUG__
    if (!kMousePacketReady) return;
    
    bool xNeg, yNeg, xOvf, yOvf;

    xNeg = (kMousePacket[0] & kPS2XSign);
    yNeg = (kMousePacket[0] & kPS2YSign);
    
    xOvf = (kMousePacket[0] & kPS2XOverflow);
    yOvf = (kMousePacket[0] & kPS2YOverflow);

    kX += xNeg ? (256 - kMousePacket[1]) : (256 - (-kMousePacket[1]));
    kY += yNeg ? (256 - kMousePacket[2]) : (256 - (-kMousePacket[2]));;

    if (xOvf) {
        kX += xNeg ? 255 : -255;
    }

    if (yOvf) {
        kY += yNeg ? 255 : -255;
    }

    if (kY > kHandoverHeader->f_GOP.f_Height)
    {
        return;
    }

    if (kX > kHandoverHeader->f_GOP.f_Width)
    {
        return;
    }

    KeClearZone(POINTER_HEIGHT, POINTER_WIDTH, kPrevX, kPrevY);

    KeInitRsrc();
    KeDrawRsrc(Pointer, POINTER_HEIGHT, POINTER_WIDTH, kX, kY);
    KeClearRsrc();

    kPrevX = kMousePacket[1];
    kPrevY = kMousePacket[2];

    kMousePacketReady = false;
#endif
}

/// @brief Inital kernel mouse initializer
/// @param  
EXTERN_C void _ke_init_mouse(void)
{
#ifdef __DEBUG__
    kMousePS2.Init();
#endif
}