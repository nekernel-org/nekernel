.data

.global hal_ap_blob_start         
.global hal_ap_blob_length          

hal_ap_blob_start:
    cli
    hlt
    jmp hal_ap_blob_start

hal_ap_blob_length:
    .long 4
