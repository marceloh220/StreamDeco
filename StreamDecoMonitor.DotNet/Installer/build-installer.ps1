$ErrorActionPreference = 'Stop'

$projectDirectory = Split-Path -Parent $PSScriptRoot
$repositoryDirectory = Split-Path -Parent $projectDirectory
$publishDirectory = Join-Path $projectDirectory 'publish'
$artifactDirectory = Join-Path $repositoryDirectory 'artifacts'
$compiler = (Get-Command ISCC.exe -ErrorAction SilentlyContinue | Select-Object -First 1).Source

if (-not $compiler) {
    $candidates = @(
        (Join-Path ${env:ProgramFiles(x86)} 'Inno Setup 6\ISCC.exe'),
        (Join-Path $env:ProgramFiles 'Inno Setup 6\ISCC.exe'),
        (Join-Path $env:LOCALAPPDATA 'Programs\Inno Setup 6\ISCC.exe')
    )
    $compiler = $candidates | Where-Object { Test-Path $_ } | Select-Object -First 1
}

if (-not $compiler) {
    throw 'Inno Setup 6 nao encontrado. Instale-o e execute este script novamente.'
}

New-Item -ItemType Directory -Force -Path $artifactDirectory | Out-Null
& dotnet publish (Join-Path $projectDirectory 'StreamDecoMonitor.DotNet.csproj') `
    -c Release -r win-x64 --self-contained true -o $publishDirectory
if ($LASTEXITCODE -ne 0) {
    throw "dotnet publish falhou com codigo $LASTEXITCODE."
}

Push-Location $PSScriptRoot
try {
    & $compiler 'StreamDecoMonitor.iss'
    if ($LASTEXITCODE -ne 0) {
        throw "Inno Setup falhou com codigo $LASTEXITCODE."
    }
}
finally {
    Pop-Location
}

Write-Host "Instalador criado em $(Join-Path $artifactDirectory 'StreamDecoMonitor-Setup.exe')"