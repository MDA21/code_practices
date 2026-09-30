#pragma once

#include "SparseSet/SparseSet.h"

#include <cstddef>

// 一个实体当前是否还在屏幕上。
// 本题不处理 generation，实体编号仍是 SparseSet 的 [0, maxEntity)。
struct Visible {
    bool visible = false;
};

// 删掉当前 dense 区里 visible == false 的实体。
// 调用结束后：
// - 每个留下的实体 visible == true，且仍能 contains / get
// - 每个被删掉的实体不再 contains
// - dense 下标 [0, size()) 无空洞，每个现存实体恰好出现一次
// 只改这个函数。不要改 SparseSet，也不要改测试。
void removeInvisible(SparseSet<Visible>& components);

void testRemoveInvisible();
