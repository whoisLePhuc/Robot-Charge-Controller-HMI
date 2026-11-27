# Windows (MinGW gcc on PATH): the same host tests as run.sh.
$ErrorActionPreference = "Stop"
Push-Location $PSScriptRoot
try {
    $c = "../../components"
    $tests = @(
        @{ name = "test_rcc_codec"; args = @("-I$c/protocol/rcc_codec/include", "test_rcc_codec.c", "$c/protocol/rcc_codec/src/rcc_codec.c") },
        @{ name = "test_hmi_present"; args = @("-Werror", "-I$c/ui/ui_present/include", "-I$c/common/hmi_common/include", "test_hmi_present.c", "$c/ui/ui_present/src/hmi_present.c") },
        @{ name = "test_rcc_link"; args = @("-Istubs", "-I$c/protocol/rcc_link/include", "-I$c/protocol/rcc_codec/include", "-I$c/common/hmi_common/include", "test_rcc_link.c", "$c/protocol/rcc_codec/src/rcc_codec.c") }
    )
    foreach ($t in $tests) {
        $exe = Join-Path $env:TEMP "$($t.name).exe"
        gcc -std=c11 -Wall -Wextra -Werror @($t.args) -o $exe
        if ($LASTEXITCODE -ne 0) { throw "$($t.name): compilation failed" }
        & $exe
        if ($LASTEXITCODE -ne 0) { throw "$($t.name): failed" }
    }
} finally { Pop-Location }
