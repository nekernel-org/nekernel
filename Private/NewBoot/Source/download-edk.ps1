$client = new-object System.Net.WebClient
$output = "$PSScriptRoot\OVMF.fd"

$client.DownloadFile("https://retrage.github.io/edk2-nightly/bin/DEBUGX64_OVMF.fd", $output)
