$innoPath = @(
    (Join-Path ${env:ProgramFiles(x86)} 'Inno Setup 6'),
    (Join-Path $env:ProgramFiles 'Inno Setup 6'),
    (Join-Path $env:LOCALAPPDATA 'Programs\Inno Setup 6')
) | Where-Object { Test-Path (Join-Path $_ 'ISCC.exe') } | Select-Object -First 1

if (-not $innoPath) {
    $winget = Get-Command winget.exe -ErrorAction SilentlyContinue
    if (-not $winget) {
        throw 'Inno Setup 6 nao foi encontrado e o winget nao esta disponivel.'
    }

    & $winget.Source install --id JRSoftware.InnoSetup `
        --exact --accept-package-agreements --accept-source-agreements --silent
    if ($LASTEXITCODE -ne 0) {
        throw "Falha ao instalar o Inno Setup 6 pelo winget. Codigo de saida: $LASTEXITCODE"
    }

    $innoPath = @(
        (Join-Path ${env:ProgramFiles(x86)} 'Inno Setup 6'),
        (Join-Path $env:ProgramFiles 'Inno Setup 6'),
        (Join-Path $env:LOCALAPPDATA 'Programs\Inno Setup 6')
    ) | Where-Object { Test-Path (Join-Path $_ 'ISCC.exe') } | Select-Object -First 1

    if (-not $innoPath) { throw 'O Inno Setup 6 foi instalado, mas o compilador ISCC.exe nao foi localizado.' }
}

$userPath = [Environment]::GetEnvironmentVariable('Path', 'User')
$entries = @($userPath -split ';' | Where-Object { $_ })

if ($entries -notcontains $innoPath) {
    [Environment]::SetEnvironmentVariable('Path', (($entries + $innoPath) -join ';'), 'User')
}

if (($env:Path -split ';') -notcontains $innoPath) {
    $env:Path += ";$innoPath"
}

Get-Command ISCC.exe
