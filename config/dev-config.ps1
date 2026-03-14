$Root = Split-Path -Parent $PSScriptRoot

# Keil MDK
$KeilUv4 = 'C:\\Keil5\\UV4\\UV4.exe'

# Keil project (absolute path for nested workspaces)
$ProjectFile = 'C:\Users\prohibit\Desktop\stm32f4_project\cubemx\EdgeGestureHub\MDK-ARM\EdgeGestureHub.uvprojx'
$TargetName = 'EdgeGestureHub'
$BuildLog = 'C:\Users\prohibit\Desktop\stm32f4_project\cubemx\EdgeGestureHub\MDK-ARM\EdgeGestureHub\EdgeGestureHub.build_log.htm'

# Build artifacts
$ArtifactPath = 'C:\Users\prohibit\Desktop\stm32f4_project\cubemx\EdgeGestureHub\MDK-ARM\EdgeGestureHub\EdgeGestureHub.axf'
$HexPath = 'C:\Users\prohibit\Desktop\stm32f4_project\cubemx\EdgeGestureHub\MDK-ARM\EdgeGestureHub\EdgeGestureHub.hex'

# ST-Link flashing
$StlinkCli = 'C:\\Program Files (x86)\\STMicroelectronics\\STM32 ST-LINK Utility\\ST-LINK Utility\\ST-LINK_CLI.exe'
$StlinkInterface = 'SWD'

# UART monitor
$SerialPort = 'auto'
$BaudRate = 115200
$LogPath = Join-Path $Root 'logs\\uart.log'

# Ensure runtime folders exist
$null = New-Item -ItemType Directory -Force -Path (Join-Path $Root 'Objects'), (Join-Path $Root 'logs')
