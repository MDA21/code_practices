#pragma once

#include "SparseSet/SparseSet.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

// 本帧只记录要删除的实体，flush 时才真正从 SparseSet 里 remove。
// 实体编号是 [0, maxEntity)。本题不处理 generation。
// 类和函数定义都写在这个头文件里。不要改 SparseSet，也不要改测试。
class DeferredRemove {
public:
    explicit DeferredRemove(std::uint32_t maxEntity) : pendingCount_(0), marks_(maxEntity) {

    }

    // entity >= maxEntity 时返回 false，状态不变。
    // 本帧第一次标记返回 true，pendingCount 加 1。
    // 本帧已经标记过再 mark，返回 true，pendingCount 不变。
    // mark 不得修改 components，也不得调用 remove。
    [[nodiscard]]
    bool mark(std::uint32_t entity) {
        if (entity >= marks_.size()) return false;
        if (marks_[entity] == true) return true;
        marks_[entity] = true;
        pendingCount_++;
        return true;
    }

    // entity >= maxEntity，或当前没有标记，返回 false。
    [[nodiscard]]
    bool isMarked(std::uint32_t entity) const {
        if (entity >= marks_.size()) return false;
        return marks_[entity];
    }

    [[nodiscard]]
    std::size_t pendingCount() const noexcept {
        return pendingCount_;
    }

    [[nodiscard]]
    std::uint32_t maxEntity() const noexcept {
        return marks_.size();
    }

    // 以这次调用时的 components.contains 为准：
    // - 已标记且存在的实体，remove 一次
    // - 已标记但不存在的实体，跳过
    // 返回实际删除的个数。
    // 返回前清空全部标记，pendingCount 变为 0。
    template <typename T>
    std::size_t flush(SparseSet<T>& components) {
        std::size_t numDeleted = 0;
        for (std::uint32_t i = 0;i < marks_.size();++i) {
            if (marks_[i]) {
                if (components.remove(i)) {
                    numDeleted++;
                }
                marks_[i] = false;
            }
        }
        pendingCount_ = 0;
        return numDeleted;
    }

private:
    // 自行设计成员变量
    std::size_t pendingCount_;
    std::vector<std::uint8_t> marks_;

};

void testDeferredRemove();
