# StreamDeco Monitor .NET

Port Windows do aplicativo `StreamDecoMonitor/streamDeco_customtkinter_preview.py`, em C# e WPF, sem executar Python. O aplicativo original e o firmware permanecem inalterados.

## Requisitos

- Windows 10/11 x64.
- [SDK .NET 10](https://dotnet.microsoft.com/download/dotnet/10.0) e [Inno Setup 6](https://jrsoftware.org/isdl.php) para gerar o instalador. A publicacao autocontida nao requer instalar o .NET Runtime no computador de destino.
- Driver USB serial CH340 ou CP210x para a placa.
- Acesso ao NuGet na primeira compilacao: [LibreHardwareMonitorLib 0.9.4](https://www.nuget.org/packages/LibreHardwareMonitorLib/0.9.4), [System.IO.Ports 10.0.0](https://www.nuget.org/packages/System.IO.Ports/10.0.0) e [System.Management 10.0.0](https://www.nuget.org/packages/System.Management/10.0.0) sao restaurados automaticamente.

Para instalar as ferramentas de compilacao pelo Windows Package Manager, use `winget install --id Microsoft.DotNet.SDK.10 -e` e `winget install --id JRSoftware.InnoSetup -e`. Os drivers oficiais estao disponiveis em [WCH (CH340/CH341)](https://www.wch-ic.com/downloads/CH341SER_EXE.html) e [Silicon Labs (CP210x)](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers).

## Executar

No PowerShell, a partir da raiz do repositorio:

```powershell
dotnet run --project .\StreamDecoMonitor.DotNet\StreamDecoMonitor.DotNet.csproj
```

O repositorio pode estar em um compartilhamento de rede, e algumas politicas do Windows bloqueiam executaveis nessa localizacao. Para compilar para uma pasta local e executar pelo host .NET instalado:

```powershell
dotnet publish .\StreamDecoMonitor.DotNet\StreamDecoMonitor.DotNet.csproj -c Release --self-contained false -o "$env:LOCALAPPDATA\StreamDecoMonitor\app"
& "$env:LOCALAPPDATA\StreamDecoMonitor\app\StreamDecoMonitor.DotNet.exe"
```

Para diagnosticos detalhados na lateral e registro de todos os sensores, acrescente `--debug` ao comando de execucao. O executavel solicita direitos administrativos ao abrir para permitir que o LibreHardwareMonitor acesse sensores protegidos. Sem esses sensores, carga CPU, RAM e disco continuam sendo coletados.

## Comportamento

- Coleta em segundo plano a cada segundo: carga, temperatura e frequencia CPU/GPU, RAM em MiB, disco C: em GiB, hora e data locais.
- LibreHardwareMonitor fornece os sensores CPU/GPU. A carga CPU tem fallback em GetSystemTimes, frequencia CPU em WMI, memoria em GlobalMemoryStatusEx e disco em DriveInfo. NVIDIA tem fallback via nvidia-smi, quando disponivel.
- Mantem os sensores CPU Total, CPU Package, CPU Core #1, GPU Core e D3D 3D do original, incluindo fallback de temperatura/frequencia para GPU integrada. Sensores indisponiveis usam zero ou os fallbacks, sem dados simulados.
- Tela com indicadores circulares CPU/GPU, barras de temperatura/frequencia, barras RAM/disco, relogio e data, usando os assets existentes.
- Botao superior alterna Monitor, Performance, Clock e Diagnostics; o central pausa/retoma somente os widgets de metricas; o inferior retorna a Monitor. Relogio, coleta e serial continuam durante a pausa.
- Na inicializacao e em cada tentativa de reconexao pelo icone do dispositivo ou pelo menu da bandeja, o app procura portas compativeis e conecta automaticamente quando encontra um adaptador cadastrado. O seletor manual abre somente quando nenhum compativel e localizado (ou a busca falha); nele, a ausencia de um adaptador compativel e informada e qualquer porta pode ser escolhida. O comando explicito "Selecionar porta serial" continua abrindo o seletor sob demanda.
- Os adaptadores reconhecidos automaticamente ficam na lista `SerialAdapters.Known` em `SerialService.cs`, com nome, VID, PIDs e marcadores de nome. Para adicionar um modelo, inclua uma entrada com os IDs PnP e nomes usados pelo respectivo driver.
- O icone verde indica uma conexao realmente aberta, nao apenas uma porta selecionada.
- Menu de contexto ou bandeja permite selecionar qualquer COM manualmente e atualizar a lista de portas.
- Serial em tarefa separada, com fila limitada que mantem somente a amostra mais recente, retry de conexao a cada segundo, recuperacao de falha de envio e fechamento na saida.
- Fechar ou minimizar a janela envia para a bandeja. O menu oferece mostrar/minimizar, restaurar, reconectar, selecionar COM e sair.
- Instancia unica com o mesmo mutex global do Python, evitando executar ambas as versoes simultaneamente.
- Diagnostics mostra estado serial, porta, tentativas, falhas, ultimo evento/erro, metricas em texto e ultimo payload enviado com horario.
- Logs em `%LOCALAPPDATA%\StreamDecoMonitor\monitor.log`, com rotacao simples em 2 MB.

Este port usa APIs de Windows e WPF; nao substitui o suporte Linux do coletor Python. Frequencias e temperaturas dependem dos sensores e drivers disponiveis no computador.

## Protocolo Serial

115200 baud, 8 bits, sem paridade, 1 stop bit, sem handshake, DTR/RTS ativos e timeouts de leitura/escrita de 1000 ms, como o pyserial original. Nao adiciona newline nem BOM. O payload e UTF-8/ASCII, truncando os valores em direcao a zero como `int()` em Python:

```text
cpu_load, cpu_temp, cpu_freq, gpu_load, gpu_temp, gpu_freq, ram_used, ram_total, disk_used, disk_total, second, minute, hour, weekday, day, month, year/
```

Sao 17 inteiros separados exatamente por virgula e espaco. Domingo = 0, segunda = 1, ..., sabado = 6. Exemplo:

```text
12, 45, 3599, 78, 60, 1700, 8192, 16384, 120, 500, 9, 5, 14, 0, 8, 11, 2026/
```

## Verificacoes

Testes de protocolo e serial com transporte simulado, sem abrir portas reais:

```powershell
dotnet build .\StreamDecoMonitor.DotNet\Tests\ProtocolChecks.csproj -o "$env:TEMP\StreamDecoProtocolChecks"
dotnet "$env:TEMP\StreamDecoProtocolChecks\ProtocolChecks.dll"
```

Verificam payload literal, cultura pt-BR, truncamento, todos os dias da semana, retry de abertura, recuperacao de escrita, troca de porta, descarte de backlog e fechamento de conexoes.

Teste da interface com coleta real, sem serial fisica:

```powershell
dotnet "$env:LOCALAPPDATA\StreamDecoMonitor\app\StreamDecoMonitor.DotNet.dll" --smoke-test
dotnet "$env:LOCALAPPDATA\StreamDecoMonitor\app\StreamDecoMonitor.DotNet.dll" --debug --smoke-test
```

Verifica coleta, pausa/retomada, quatro modos em 960x640 e 1280x800, ocultacao/restauracao e ausencia de tentativas COM. Salva PNGs e `result.txt` em `%LOCALAPPDATA%\StreamDecoMonitor\smoke-test` e encerra automaticamente. Falhas retornam codigo de saida 1.

A confirmacao final de comunicacao exige conectar a placa, observar o ultimo payload em Diagnostics e conferir os valores no painel do firmware. Os testes automatizados nao substituem essa validacao fisica.

## Publicacao Autocontida

Para distribuir sem instalar o runtime, publique uma pasta autocontida (nao use trimming, pois WPF e sensores usam reflexao):

```powershell
dotnet publish .\StreamDecoMonitor.DotNet\StreamDecoMonitor.DotNet.csproj -c Release -r win-x64 --self-contained true -o "$env:LOCALAPPDATA\StreamDecoMonitor\publish"
```

Distribua a pasta inteira, incluindo assets e bibliotecas/drivers. Politicas de Controle de Aplicativo podem exigir aprovacao ou assinatura do executavel pela organizacao.

## Instalador Windows

Instale o Inno Setup 6 e execute `.\StreamDecoMonitor.DotNet\Installer\build-installer.ps1` no PowerShell a partir da raiz do repositorio. O script publica a versao autocontida x64 e gera `artifacts/StreamDecoMonitor-Setup.exe`.

O instalador requer direitos administrativos, cria um atalho no menu Iniciar e oferece um atalho opcional na area de trabalho. A opcao de inicializacao com o Windows cria uma tarefa agendada para executar com privilegios mais altos. O executavel tambem solicita elevacao quando iniciado manualmente. Para a tarefa de logon iniciar elevada, a conta interativa precisa ser administradora; uma conta padrao nao pode obter esses direitos por meio de um atalho ou tarefa.

## Smart App Control do Windows

Se o Smart App Control bloquear o instalador, nao existe uma lista de excecoes por aplicativo para esse recurso nas Configuracoes do Windows. Confira o estado em **Seguranca do Windows > Controle de aplicativos e navegador > Smart App Control**. No modo de imposicao, o Windows permite aplicativos com boa reputacao no servico de inteligencia da Microsoft ou assinados com um certificado confiavel. O instalador gerado localmente nao tem assinatura de codigo, portanto nao ha opcao **Executar assim mesmo** para esse bloqueio. Use uma versao assinada e confiavel ou, em um computador gerenciado, solicite ao administrador a aprovacao pela politica corporativa de controle de aplicativos. Uma exclusao no Microsoft Defender Antivirus nao libera um bloqueio do Smart App Control.

SmartScreen e um recurso separado. Se o aviso for apenas de reputacao do SmartScreen e oferecer **Mais informacoes > Executar assim mesmo**, use essa opcao somente depois de verificar a origem do instalador e o hash publicado, quando houver. Consulte a [documentacao da Microsoft sobre Smart App Control](https://learn.microsoft.com/en-us/windows/apps/develop/smart-app-control/overview).