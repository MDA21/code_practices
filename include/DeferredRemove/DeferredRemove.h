#pragma once

#include "SparseSet/SparseSet.h"

#include <cstddef>
#include <cstdint>

// 本帧只记录要删除的实体，flush 时才真正从 SparseSet 里 remove。
// 实体编号是 [0, maxEntity)。本题不处理 generation。
// 类和函数定义都写在这个头文件里。不要改 SparseSet，也不要改测试。
class DeferredRemove {
public:
    explicit DeferredRemove(std::uint32_t maxEntity);

    // entity >= maxEntity 时返回 false，状态不变。
    // 本帧第一次标记返回 true，pendingCount 加 1。
    // 本帧已经标记过再 mark，返回 true，pendingCount 不变。
    // mark 不得修改 components，也不得调用 remove。
    [[nodiscard]]
    bool mark(std::uint32_t entity);

    // entity >= maxEntity，或当前没有标记，返回 false。
    [[nodiscard]]
    bool isMarked(std::uint32_t entity) const;

    [[nodiscard]]
    std::size_t pendingCount() const noexcept;

    [[nodiscard]]
    std::uint32_t maxEntity() const noexcept;

    // 以这次调用时的 components.contains 为准：
    // - 已标记且存在的实体，remove 一次
    // - 已标记但不存在的实体，跳过
    // 返回实际删除的个数。
    // 返回前清空全部标记，pendingCount 变为 0。
    template <typename T>
    std::size_t flush(SparseSet<T>& components);

private:
    // 自行设计成员变量
};

void testDeferredRemove();
