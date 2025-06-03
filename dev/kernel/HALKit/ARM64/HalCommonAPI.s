/* (c) 2024-2025 Amlal El Mahrouss */

.text

.global hal_flush_tlb

hal_flush_tlb:
    tlbi vmalle1
    ret
