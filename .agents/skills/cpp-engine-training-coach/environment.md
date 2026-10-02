# 本仓库的设备与构建约定

## Windows / PowerShell

从仓库根目录执行：

```powershell
# 只检测工具链，不编译题目
./scripts/build-practice.ps1 -Probe
# 编译当前工程，默认 Debug / x64
./scripts/build-practice.ps1
# 编译成功后运行 src/main.cpp 当前启用的测试
./scripts/build-practice.ps1 -Run
# 性能题：Release 编译，先验收再测量
./scripts/build-practice.ps1 -Benchmark
```

本次已验证的终端为 PowerShell 7。若 Windows PowerShell 5.1 提示禁止运行脚本，可仅对这次进程执行以下命令，无需更改系统执行策略：

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File ./scripts/build-practice.ps1 -Run
```

脚本通过 `vswhere` 查找含 C++ 工具的 Visual Studio，并选择实际安装的 MSVC PlatformToolset，通过 MSBuild 命令行覆盖项目默认值。本机无需将 `cl` 加入全局 PATH。

共享 `.vcxproj` 的 PlatformToolset 使用 `$(DefaultPlatformToolset)`，随当前 Visual Studio 选择默认工具集，避免将单台设备的 `v145` 固定到仓库。VS2026 本机已验证；VS2022 需在另一台设备实际验证。`.sln` / `.slnx` 已忽略且不追踪，各设备可以自行打开 `.vcxproj` 创建并保存本地解决方案。命令行脚本直接构建 `.vcxproj`，不依赖 `.sln`。

默认 Debug 保留 `assert` 验收逻辑，现有工程已启用 C++20、`/W4` 和 `/permissive-`。不要用 Release 运行当前依赖 assert 的验收测试，其中部分断言包含状态修改操作。

MatrixTraversal 的独立正确性检查不依赖 `assert`，在 Release 中仍会执行，适用于 `-Benchmark`。性能测试只在正确性检查通过后运行。其他题目若仍使用 `assert`，继续以 Debug 验收。

现有源码混用 UTF-8 与非 UTF-8 编码，本机默认代码页下可能出现 C4819 警告。不要直接全局添加 `/utf-8`，这会让另一部分文件出现 C4828；配置适配保留源码原始字节，未来统一编码应作为单独工作进行。

构建产物位于 `bin/` 与 `obj/`，由 `.gitignore` 排除。脚本编译失败时立即返回非零退出码，不运行旧的可执行文件。运行只覆盖当前入口启用的测试，不能据此宣称所有历史题通过。

2026-10-02 探测到本机安装 Visual Studio Community 2026，工具集为 `v145`；此记录只是探测结果，后续以脚本实际发现为准。CUDA / MPI 尚未核实，相关练习出题前单独检测。

## 训练状态与验证范围

`practice/cpp_algorithms/progress.md` 是训练记录入口。2026-10-02 初次配置时 DeferredRemove 仅有声明，链接失败；同日学员提交修订后已通过既有测试。后续始终读取实际进度与源码，不以本环境说明判断题目是否完成，也不允许 Agent 为了验证配置补写解答。

配置验证可使用独立临时 C++ 程序检查编译器、链接器与运行能力。正式题目只有在学员提交后，才根据实际编译和测试结果更新训练状态。
