$client = new-object System.Net.WebClient
$client.DownloadFile("https://retrage.github.io/edk2-nightly/bin/DEBUGX64_OVMF.fd","C:\micro-kernel-Amllx-trunk-patch-989e\Private\NewBoot\Source\OVMF.fd")  