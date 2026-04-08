- what changed: 补做了把当前工程当作非 CubeMX / brownfield 样例的结构梳理，已确认最小可识别面落在根 `CMakeLists.txt`、`cmake/startup_stm32f407xx.s`、`cmake/STM32F407VETX_FLASH.ld`、`.vscode` 调试配置，以及 `cubemx/EdgeGestureHub` 下的 `Core` / `Drivers` 目录。
- current blocker or risk: `project-skills/edgegesture-handoff/SKILL.md` 和 `keil-cubemx-notes.md` 这两个交接引用路径当前不存在；另外样例同时保留了 `cmake/` 和 `MDK-ARM/` 两套启动入口，后续如果做自动抽取，需要先明确优先级，避免同时吃进 GCC 和 Keil 产物。
- exact next recommended step: 把 brownfield 自动识别规则固定为“优先根 `CMakeLists.txt`，再回退到 `startup_*.s` / `*.ld` / `.ioc` 文件名规则”，并把源目录、头文件目录、宏改成直接从现有 CMake target 提取。

- what changed: 修复了顶层 `CMakeLists.txt`，现在会在 `project(...)` 前接入 `cmake/gcc-arm-none-eabi.cmake`；更新了 `.vscode/launch.json`，调试入口直接指向 `build/Debug/EdgeGestureHub.elf`；同步修复了 `tools/STM32.Common.ps1` 和 `tools/Invoke-StlinkGdbServer.ps1`，空路径不再导致 `STM32: Start GDB Server` 失败。
- current blocker or risk: 链接阶段仍有 `_read/_write/_close/_lseek` 的标准库警告，以及 `variable-size enums` / `RWX` 警告；当前不阻塞编译和 GDB server 启动，但后续最好统一枚举选项并补最小 syscalls 约定。
- exact next recommended step: 在 VSCode 里重新点一次 `STM32: Debug via external GDB Server`；如果仍有问题，优先看是不是断点停不住或程序跑飞，而不是继续排查工具启动。
