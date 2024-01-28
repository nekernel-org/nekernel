/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/Arch.hpp>

namespace hCore::HAL
{
namespace Detail
{
struct RegisterAMD64 final
{
    UIntPtr base;
    UShort limit;
};
} // namespace Detail

void GDTLoader::Load(Register64 &gdt)
{
    Detail::RegisterAMD64 *reg = new Detail::RegisterAMD64();
    MUST_PASS(reg);

    reg->base = gdt.Base;
    reg->limit = gdt.Limit;

    rt_cli();
    load_gdt(reg);
    rt_sti();
}

void IDTLoader::Load(Register64 &idt)
{
    Detail::RegisterAMD64 *reg = new Detail::RegisterAMD64();
    MUST_PASS(reg);

    reg->base = idt.Base;
    reg->limit = idt.Limit;

    rt_cli();
    load_idt(reg);
    rt_sti();
}

void GDTLoader::Load(Ref<Register64> &gdt)
{
    GDTLoader::Load(gdt.Leak());
}

void IDTLoader::Load(Ref<Register64> &idt)
{
    IDTLoader::Load(idt.Leak());
}
} // namespace hCore::HAL
