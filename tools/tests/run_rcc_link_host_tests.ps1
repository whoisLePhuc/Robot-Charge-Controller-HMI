$ErrorActionPreference = "Stop"
Push-Location (Join-Path $PSScriptRoot "../..")
try {
    $exe = Join-Path $env:TEMP "rcc_link_host_test.exe"
    gcc -std=c11 -Wall -Wextra -Itools/tests/stubs -Ifirmware/src tools/tests/rcc_link_host_test.c firmware/src/rcc_codec.c -o $exe
    if ($LASTEXITCODE -ne 0) { throw "Host test compilation failed" }
    & $exe
    if ($LASTEXITCODE -ne 0) { throw "Host tests failed" }
} finally { Pop-Location }
