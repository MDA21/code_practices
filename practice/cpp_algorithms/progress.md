# C++ / 算法训练进度

## 当前题目

SparseSet（稀疏集合 / ECS 组件存储）

- 开始日期：2026-09-29
- 当前状态：测试通过。已按请求重命名 `remove` / `insert` 局部变量。
- 实现文件：
  - `include/SparseSet/SparseSet.h`
  - `include/SparseSet/SparseSet.cpp`（测试，不要把解法写进测试）
- 编译和测试命令：
  ```
  cl /W4 /permissive- /EHsc /std:c++20 /I include src/main.cpp include/EntityHandlePool/EntityHandlePool.cpp include/ScheduleTask/ScheduleTask.cpp include/RingQueue/RingQueue.cpp include/SparseSet/SparseSet.cpp
  ```
  当前 `src/main.cpp` 只启用 `testSparseSet()`。

## 仓库中已有练习

| 题目 | 文件 | 状态 |
|---|---|---|
| EntityAllocator | `include/EntityHandlePool/` | 已实现，尚未做本次协议下的正式评审 |
| scheduleTasks | `include/ScheduleTask/` | 已实现，尚未做本次协议下的正式评审 |
| RingQueue | `include/RingQueue/` | 已通过评审 |
| SparseSet | `include/SparseSet/` | 已通过测试；局部变量已重命名 |

## 已发现的问题

- `EntityAllocator::destroy` 中有 generation 溢出注释，未在本次处理。

## 已掌握知识点

- 实体句柄与 generation 校验。
- 任务依赖的拓扑排序。
- 环形队列；用 `optional<T>` 同时表达占用和对象寿命。
- 避免 `vector<bool>`。
- Sparse Set：sparse 按下标点查，dense 连续遍历；删除时更新被挪动实体的映射。
- `T` / `T&` / `T*` / `optional<T>` 不能混用。

## 仍然薄弱的知识点

- 中等题的独立实现。
- 句柄到连续数据的映射、删除后如何保持 dense 无空洞。

## 下一题的递进依据

本题训练 sparse / dense 双数组。完成后可走向延迟释放，或给已有句柄池接上组件存储。
