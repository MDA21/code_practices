# C++ / 算法训练进度

## 当前题目

DeferredRemove（本帧标记，帧末再从 SparseSet 删除）

- 开始日期：2026-09-30
- 当前状态：已出题。待学员在头文件里实现后提交。
- 实现文件：
  - `include/DeferredRemove/DeferredRemove.h`
  - `include/DeferredRemove/DeferredRemove.cpp`（测试，不要把解法写进测试，也不要改测试）
- 编译和测试命令：
  ```
  cl /W4 /permissive- /EHsc /std:c++20 /I include src/main.cpp include/EntityHandlePool/EntityHandlePool.cpp include/ScheduleTask/ScheduleTask.cpp include/RingQueue/RingQueue.cpp include/SparseSet/SparseSet.cpp include/VisibilityCull/VisibilityCull.cpp include/DeferredRemove/DeferredRemove.cpp
  ```
  当前 `src/main.cpp` 只启用 `testDeferredRemove()`。

## 仓库中已有练习

| 题目 | 文件 | 状态 |
|---|---|---|
| EntityAllocator | `include/EntityHandlePool/` | 已实现，尚未做本次协议下的正式评审 |
| scheduleTasks | `include/ScheduleTask/` | 已实现，尚未做本次协议下的正式评审 |
| RingQueue | `include/RingQueue/` | 已通过评审 |
| SparseSet | `include/SparseSet/` | 已通过评审（2026-09-29 测试通过） |
| removeInvisible | `include/VisibilityCull/` | 已通过评审 |
| DeferredRemove | `include/DeferredRemove/` | 进行中 |

## 已发现的问题

- `EntityAllocator::destroy` 中有 generation 溢出注释，未在本次处理。

## 已掌握知识点

- 实体句柄与 generation 校验。
- 任务依赖的拓扑排序。
- 环形队列；用 `optional<T>` 同时表达占用和对象寿命。
- 避免 `vector<bool>`。
- Sparse Set：sparse 按下标点查，dense 连续遍历；删除时更新被挪动实体的映射。
- `T` / `T&` / `T*` / `optional<T>` 不能混用。
- 正向扫描 dense 时，`remove` 后停在当前下标，检查刚换进来的实体。

## 仍然薄弱的知识点

- 中等题的独立实现。
- 本帧先标记、帧末再压缩的延迟删除。

## 下一题的递进依据

上一题处理的是遍历当下就 `remove`。本题把删除推迟到 `flush`：标记期间 dense 下标保持有效，帧末再压缩。完成后可把这套标记接到 `EntityAllocator` 的句柄上。
