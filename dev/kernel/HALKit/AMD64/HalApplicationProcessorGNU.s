.data

.global hal_ap_blob_start         /* Export the start symbol */
.global hal_ap_blob_end           /* Export the end symbol */

hal_ap_blob_start:
    .incbin "HALKit/AMD64/HalApplicationProcessorStartup.bin"
hal_ap_blob_end:
