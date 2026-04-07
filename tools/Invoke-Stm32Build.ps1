[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [ValidateSet("Configure", "Build", "Clean")]
    [string]$Action,
    [string]$WorkspaceRoot = (Get-Location).Path,
    [string]$SourceDir,
    [string]$BuildDir,
    [string]$BuildType = "Debug",
    [string]$ToolchainFile
)

Set-StrictMode -Version 3.0
$ErrorActionPreference = "Stop"

. "$PSScriptRoot\STM32.Common.ps1"

$resolvedWorkspace = Resolve-WorkspacePath -Path $WorkspaceRoot
$resolvedSource = Resolve-OptionalPath -Path $SourceDir -BasePath $resolvedWorkspace
if (-not $resolvedSource) {
    $resolvedSource = $resolvedWorkspace
}

$resolvedBuild = Resolve-OptionalPath -Path $BuildDir -BasePath $resolvedWorkspace
if (-not $resolvedBuild) {
    $resolvedBuild = Join-Path $resolvedWorkspace "build\Debug"
}

$resolvedToolchainFile = Resolve-OptionalPath -Path $ToolchainFile -BasePath $resolvedWorkspace
if (-not $resolvedToolchainFile) {
    $candidateToolchain = Join-Path $resolvedSource "cmake\gcc-arm-none-eabi.cmake"
    if (Test-Path -LiteralPath $candidateToolchain) {
        $resolvedToolchainFile = $candidateToolchain
    }
}

if ($Action -eq "Clean") {
    if (Test-Path -LiteralPath $resolvedBuild) {
        Remove-Item -LiteralPath $resolvedBuild -Recurse -Force
        Write-Host "已清理构建目录: $resolvedBuild"
    } else {
        Write-Host "构建目录不存在: $resolvedBuild"
    }
    exit 0
}

if (-not (Test-Path -LiteralPath (Join-Path $resolvedSource "CMakeLists.txt"))) {
    throw "没有找到 CMakeLists.txt。请先在 CubeMX 里把 Toolchain/IDE 设为 CMake 并生成工程。"
}

$tools = Get-Stm32ToolPaths
if (-not $tools.CMake) {
    throw "没有找到 CMake。请先安装 CMake 并加入 PATH。"
}

if (-not $tools.Ninja) {
    throw "没有找到 Ninja。请先安装 Ninja 并加入 PATH。"
}

if (-not $tools.ArmGcc) {
    throw "没有找到 arm-none-eabi-gcc。请先安装 Arm GNU Toolchain。"
}

$toolchainBin = Split-Path -Parent $tools.ArmGcc
$originalPath = $env:PATH
$env:PATH = "$toolchainBin;$originalPath"

try {
if ($Action -eq "Configure" -or -not (Test-Path -LiteralPath (Join-Path $resolvedBuild "CMakeCache.txt"))) {
    $configureArgs = @(
        "-S", $resolvedSource,
        "-B", $resolvedBuild,
        "-G", "Ninja",
        "-DCMAKE_BUILD_TYPE=$BuildType",
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
        "-DCMAKE_MAKE_PROGRAM=$($tools.Ninja)"
    )

    if ($resolvedToolchainFile) {
        $configureArgs += "-DCMAKE_TOOLCHAIN_FILE=$resolvedToolchainFile"
    }

    Invoke-Native -Command $tools.CMake -Arguments $configureArgs
}

    if ($Action -eq "Build") {
        Invoke-Native -Command $tools.CMake -Arguments @(
            "--build", $resolvedBuild,
            "--parallel"
        )
    }
}
finally {
    $env:PATH = $originalPath
}
