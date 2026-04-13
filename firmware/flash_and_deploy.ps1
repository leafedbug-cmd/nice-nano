param(
    [string]$WorkspaceRoot = "C:/Users/austin/Documents/GitHub/nice-nano"
)

$ErrorActionPreference = "Stop"

$firmwareDir = Join-Path $WorkspaceRoot "firmware"
$uf2 = Join-Path $firmwareDir "adafruit-circuitpython-nice_nano-en_US-10.1.4.uf2"
$code = Join-Path $firmwareDir "code.py"
$libSrc = Join-Path $firmwareDir "circuitpython_nrf24l01"

if (-not (Test-Path $uf2)) {
    throw "UF2 not found: $uf2"
}
if (-not (Test-Path $code)) {
    throw "code.py not found: $code"
}
if (-not (Test-Path $libSrc)) {
    throw "Library folder not found: $libSrc"
}

$volumes = Get-Volume | Where-Object { $_.DriveLetter }
$boot = $volumes | Where-Object { $_.FileSystemLabel -eq "NICENANO" } | Select-Object -First 1
$cp = $volumes | Where-Object { $_.FileSystemLabel -eq "CIRCUITPY" } | Select-Object -First 1

if ($boot) {
    $bootPath = "$($boot.DriveLetter):/"
    Write-Host "Flashing CircuitPython to $bootPath"
    Copy-Item $uf2 -Destination $bootPath -Force
    Write-Host "UF2 copied. Board should reboot into CIRCUITPY."
} else {
    Write-Host "NICENANO drive not found. Double-tap RST and rerun this script."
}

$volumes = Get-Volume | Where-Object { $_.DriveLetter }
$cp = $volumes | Where-Object { $_.FileSystemLabel -eq "CIRCUITPY" } | Select-Object -First 1

if ($cp) {
    $cpPath = "$($cp.DriveLetter):/"
    $cpLib = Join-Path $cpPath "lib"

    Write-Host "Deploying code and library to $cpPath"
    New-Item -ItemType Directory -Path $cpLib -Force | Out-Null
    Copy-Item $code -Destination (Join-Path $cpPath "code.py") -Force
    Copy-Item $libSrc -Destination (Join-Path $cpLib "circuitpython_nrf24l01") -Recurse -Force
    Write-Host "Deployment complete."
} else {
    Write-Host "CIRCUITPY drive not found yet. Wait for reboot, then rerun this script."
}