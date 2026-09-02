param(
    [string]$Compiler = "gcc"
)

$ErrorActionPreference = "Stop"
$templateRoot = Split-Path -Parent $PSScriptRoot
$common = Join-Path $templateRoot "leetcode\editor\common"
$output = Join-Path $templateRoot "build\verify"

New-Item -ItemType Directory -Force -Path $output | Out-Null

$commonSources = @(
    (Join-Path $common "lc_nodes.c"),
    (Join-Path $common "cstl\vos_common.c"),
    (Join-Path $common "cstl\vos_vector.c"),
    (Join-Path $common "cstl\vos_priorityqueue.c")
)

$targets = @(
    @{ Name = "test_all"; Source = (Join-Path $templateRoot "tests\test_all.c") },
    @{ Name = "two_sum"; Source = (Join-Path $templateRoot "leetcode\editor\cn\two-sum.c") },
    @{ Name = "merge_two_lists"; Source = (Join-Path $templateRoot "leetcode\editor\cn\merge-two-sorted-lists.c") }
)

foreach ($target in $targets) {
    $executable = Join-Path $output ($target.Name + ".exe")
    $arguments = @(
        "-std=c11",
        "-Wall",
        "-Wextra",
        "-Wpedantic",
        "-Werror",
        "-g3",
        "-O0",
        ("-I" + $common),
        ("-I" + (Join-Path $common "cstl")),
        ("-I" + (Join-Path $common "third_party\uthash")),
        $target.Source
    ) + $commonSources + @("-o", $executable)

    & $Compiler @arguments
    if ($LASTEXITCODE -ne 0) {
        throw "Compilation failed: $($target.Name)"
    }
    & $executable
    if ($LASTEXITCODE -ne 0) {
        throw "Execution failed: $($target.Name)"
    }
}

Write-Host "C template verification passed." -ForegroundColor Green
