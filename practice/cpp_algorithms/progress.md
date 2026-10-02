# C++ / 算法训练进度

## 当前题目

MatrixTranspose（分块矩阵转置）

- 开始日期：2026-10-02
- 当前状态：已出题，待实现普通版与分块版并提交性能观察。尚未通过本题验收。
- 框架验证：Debug / Release x64 均编译链接成功；运行按预期报告 `TODO: implement transposeNaive`，退出码 1。尚无本题性能数据。
- 类型：优化题；CPU / HPC；核心知识点为分块改善转置时的读写局部性。
- 题目：`practice/cpp_algorithms/matrix-transpose.md`
- 实现文件：
  - `include/MatrixTranspose/MatrixTranspose.cpp`（学员实现）
  - `include/MatrixTranspose/MatrixTranspose.h`（接口）
  - `include/MatrixTranspose/MatrixTransposeTests.cpp`（正确性测试与计时框架，不要修改）
- 编译和测试命令：
  ```powershell
  ./scripts/build-practice.ps1 -Run
  ./scripts/build-practice.ps1 -Benchmark
  ```
  当前 `src/main.cpp` 启用 `testMatrixTranspose()`，传入 `--benchmark` 时在正确性检查通过后运行性能测量。

  构建脚本自动发现 Visual Studio / MSVC；`-Run` 默认 Debug x64，`-Benchmark` 使用 Release x64。

## 仓库中已有练习

| 题目 | 文件 | 状态 |
|---|---|---|
| EntityAllocator | `include/EntityHandlePool/` | 已实现，尚未做本次协议下的正式评审 |
| scheduleTasks | `include/ScheduleTask/` | 已实现，尚未做本次协议下的正式评审 |
| RingQueue | `include/RingQueue/` | 已通过评审 |
| SparseSet | `include/SparseSet/` | 已通过评审（2026-09-29 测试通过） |
| removeInvisible | `include/VisibilityCull/` | 已通过评审 |
| DeferredRemove | `include/DeferredRemove/` | 已通过评审（2026-10-02，既有测试全部通过） |
| MatrixTraversal | `include/MatrixTraversal/` | 已通过评审（2026-10-02，类型修正、测试通过、核心性能解释正确） |
| MatrixTranspose | `include/MatrixTranspose/` | 进行中，待实现与测量 |

## 已发现的问题

- 已修复：MatrixTraversal 四处循环变量从 int 改为 size_t，与维度类型一致。
- MatrixTraversal 非阻塞清理：移除过时 TODO 注释和未使用的 `<stdexcept>`。
- 已修复：DeferredRemove::flush 现在对全部已标记实体清除标记，包括集合里不存在的实体；计数归零，下一帧可重新标记。
- 非阻塞遗留：C4805（uint8_t 与 bool 比较）、C4267（size_t 隐式转换为 uint32_t），以及未使用的 `<functional>`；另有既存源码编码 C4819 警告。
- `EntityAllocator::destroy` 中有 generation 溢出注释，未在本次处理。

## 已掌握知识点

- 实体句柄与 generation 校验。
- 任务依赖的拓扑排序。
- 环形队列；用 `optional<T>` 同时表达占用和对象寿命。
- 避免 `vector<bool>`。
- Sparse Set：sparse 按下标点查，dense 连续遍历；删除时更新被挪动实体的映射。
- `T` / `T&` / `T*` / `optional<T>` 不能混用。
- 正向扫描 dense 时，`remove` 后停在当前下标，检查刚换进来的实体。
- 延迟删除：标记阶段不改集合，flush 按实体编号删除，并同步清除标记与计数；当前实现时间 O(maxEntity)、空间 O(maxEntity)。
- 矩阵按行存储时，遍历顺序影响空间局部性和缓存行利用率；学员能够将该机制与实测差距联系起来。

## 仍然薄弱的知识点

- 中等题的独立实现。
- 当前延迟删除实现已通过验证，后续可训练标记稀疏时的处理成本与 generation 语义。

## 下一题的递进依据

上一题已理解行主序访问与缓存行利用率。本题增加目标写入，用分块同时考虑源与目标的局部性，并练习非整块边界。完成后根据实现和测量解释决定后续题目；generation 与延迟删除的结合保留为引擎方向候选。
