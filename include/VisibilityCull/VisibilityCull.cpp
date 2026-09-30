#include "VisibilityCull.h"

#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <iostream>

void removeInvisible(SparseSet<Visible>& components) {
    std::size_t index = 0;
    while (index < components.size()) {
        if (!components.dataAt(index).visible) {
            components.remove(components.entityAt(index));
        }
        else {
            index++;
        }
    }
}

static void expectExactly(
    const SparseSet<Visible>& set,
    std::initializer_list<std::uint32_t> alive) {
    assert(set.size() == alive.size());
    for (std::uint32_t entity : alive) {
        assert(set.contains(entity));
        const Visible* component = set.get(entity);
        assert(component != nullptr);
        assert(component->visible);
    }
    for (std::size_t i = 0; i < set.size(); ++i) {
        const std::uint32_t entity = set.entityAt(i);
        assert(set.contains(entity));
        assert(set.dataAt(i).visible);
        assert(set.get(entity) == &set.dataAt(i));
    }
}

void testRemoveInvisible() {
    // 1. 空集合
    {
        SparseSet<Visible> set(4);
        removeInvisible(set);
        expectExactly(set, {});
    }

    // 2. 全部可见
    {
        SparseSet<Visible> set(8);
        assert(set.insert(1, Visible{ true }));
        assert(set.insert(4, Visible{ true }));
        assert(set.insert(6, Visible{ true }));

        removeInvisible(set);
        expectExactly(set, { 1, 4, 6 });
    }

    // 3. 只有一个，且不可见
    {
        SparseSet<Visible> set(4);
        assert(set.insert(2, Visible{ false }));

        removeInvisible(set);
        assert(set.size() == 0);
        assert(!set.contains(2));
    }

    // 4. 不可见的只有 dense 最后一个
    {
        SparseSet<Visible> set(8);
        assert(set.insert(0, Visible{ true }));
        assert(set.insert(3, Visible{ true }));
        assert(set.insert(5, Visible{ false }));

        removeInvisible(set);
        expectExactly(set, { 0, 3 });
        assert(!set.contains(5));
    }

    // 5. 不可见的在前，换进来的是可见实体
    {
        SparseSet<Visible> set(8);
        assert(set.insert(1, Visible{ false }));
        assert(set.insert(2, Visible{ true }));

        removeInvisible(set);
        expectExactly(set, { 2 });
        assert(!set.contains(1));
    }

    // 6. 两个都不可见
    {
        SparseSet<Visible> set(8);
        assert(set.insert(0, Visible{ false }));
        assert(set.insert(1, Visible{ false }));

        removeInvisible(set);
        assert(set.size() == 0);
        assert(!set.contains(0));
        assert(!set.contains(1));
    }

    // 7. 可见，然后连续两个不可见
    {
        SparseSet<Visible> set(8);
        assert(set.insert(2, Visible{ true }));
        assert(set.insert(4, Visible{ false }));
        assert(set.insert(6, Visible{ false }));

        removeInvisible(set);
        expectExactly(set, { 2 });
        assert(!set.contains(4));
        assert(!set.contains(6));
    }

    // 8. 四个都不可见
    {
        SparseSet<Visible> set(8);
        assert(set.insert(1, Visible{ false }));
        assert(set.insert(3, Visible{ false }));
        assert(set.insert(5, Visible{ false }));
        assert(set.insert(7, Visible{ false }));

        removeInvisible(set);
        assert(set.size() == 0);
        assert(!set.contains(1));
        assert(!set.contains(3));
        assert(!set.contains(5));
        assert(!set.contains(7));
    }

    // 9. 调用前已经 remove 过，dense 顺序不是插入顺序
    {
        SparseSet<Visible> set(8);
        assert(set.insert(1, Visible{ true }));
        assert(set.insert(2, Visible{ false }));
        assert(set.insert(3, Visible{ true }));
        assert(set.insert(4, Visible{ false }));
        assert(set.remove(1));

        removeInvisible(set);
        expectExactly(set, { 3 });
        assert(!set.contains(1));
        assert(!set.contains(2));
        assert(!set.contains(4));
    }

    // 10. 可见和不可见交替，实体编号不连续
    {
        SparseSet<Visible> set(16);
        assert(set.insert(10, Visible{ false }));
        assert(set.insert(2, Visible{ true }));
        assert(set.insert(8, Visible{ false }));
        assert(set.insert(0, Visible{ true }));
        assert(set.insert(15, Visible{ false }));
        assert(set.insert(4, Visible{ true }));

        removeInvisible(set);
        expectExactly(set, { 2, 0, 4 });
        assert(!set.contains(10));
        assert(!set.contains(8));
        assert(!set.contains(15));
    }

    std::cout << "All removeInvisible tests passed.\n";
}
