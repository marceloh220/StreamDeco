$innoPath = @(
    (Join-Path ${env:ProgramFiles(x86)} 'Inno Setup 6'),
    (Join-Path $env:ProgramFiles 'Inno Setup 6'),
    (Join-Path $env:LOCALAPPDATA 'Programs\Inno Setup 6')
) | Where-Object { Test-Path (Join-Path $_ 'ISCC.exe') } | Select-Object -First 1

if (-not $innoPath) { throw 'Inno Setup 6 nao foi encontrado.' }

$userPath = [Environment]::GetEnvironmentVariable('Path', 'User')
$entries = @($userPath -split ';' | Where-Object { $_ })

if ($entries -notcontains $innoPath) {
    [Environment]::SetEnvironmentVariable('Path', (($entries + $innoPath) -join ';'), 'User')
}

if (($env:Path -split ';') -notcontains $innoPath) {
    $env:Path += ";$innoPath"
}

Get-Command ISCC.exe
