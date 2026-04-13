param(
    [string]$WorkspaceRoot = "C:/Users/austin/Documents/GitHub/nice-nano",
    # Use -Mode platformio  to build & upload via PlatformIO (adafruit-nrfutil DFU).
    # Use -Mode circuitpython (default) to flash the UF2 + deploy Python files.
    [ValidateSet("circuitpython", "platformio")]
    [string]$Mode = "circuitpython"
)

$ErrorActionPreference = "Stop"

$firmwareDir = Join-Path $WorkspaceRoot "firmware"

# ── PlatformIO mode ──────────────────────────────────────────────────────────
if ($Mode -eq "platformio") {
    Write-Host "=== PlatformIO build & upload ==="
    Write-Host "Prerequisites: 'pio' on PATH and 'adafruit-nrfutil' installed."
    Write-Host "Double-tap RST to enter DFU bootloader before running this."
    Write-Host ""

    Push-Location $firmwareDir
    try {
        Write-Host "Building..."
        & pio run
        if ($LASTEXITCODE -ne 0) { throw "pio build failed (exit $LASTEXITCODE)" }

        Write-Host ""
        Write-Host "Uploading via nrfutil..."
        & pio run --target upload
        if ($LASTEXITCODE -ne 0) { throw "pio upload failed (exit $LASTEXITCODE)" }

        Write-Host ""
        Write-Host "Upload complete. Open serial monitor with:  pio device monitor"
    } finally {
        Pop-Location
    }
    return
}

# ── CircuitPython mode (original behaviour) ──────────────────────────────────
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