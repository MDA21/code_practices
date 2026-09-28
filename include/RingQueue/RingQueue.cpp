#include <cassert>
#include <iostream>
#include <optional>
#include <utility>

#include "RingQueue.h"

// 不可复制，只能移动
struct RenderCommand {
    int passId;

    explicit RenderCommand(int id)
        : passId(id) {
    }

    RenderCommand(const RenderCommand&) = delete;
    RenderCommand& operator=(const RenderCommand&) = delete;

    RenderCommand(RenderCommand&&) noexcept = default;
    RenderCommand& operator=(RenderCommand&&) noexcept = default;
};

// 用来观察对象生命周期
struct Tracked {
    static inline int alive = 0;

    int value = 0;

    explicit Tracked(int v = 0)
        : value(v) {
        ++alive;
    }

    ~Tracked() {
        --alive;
    }

    Tracked(const Tracked&) = delete;
    Tracked& operator=(const Tracked&) = delete;

    Tracked(Tracked&& other) noexcept
        : value(other.value) {
        ++alive;
        other.value = -1;
    }

    Tracked& operator=(Tracked&& other) noexcept {
        if (this != &other) {
            value = other.value;
            other.value = -1;
        }
        return *this;
    }
};

void testRingQueue() {
    // ============================================================
    // 1. capacity == 0
    // ============================================================
    {
        RingQueue<int> queue(0);

        assert(queue.capacity() == 0);
        assert(queue.size() == 0);
        assert(queue.empty());
        assert(queue.full());

        assert(queue.front() == nullptr);
        assert(!queue.pop().has_value());

        assert(!queue.push(10));
        assert(!queue.push(20));

        assert(queue.size() == 0);
        assert(queue.empty());
        assert(queue.full());
    }

    // ============================================================
    // 2. capacity == 1
    // ============================================================
    {
        RingQueue<int> queue(1);

        assert(queue.capacity() == 1);
        assert(queue.empty());
        assert(!queue.full());

        assert(queue.push(10));

        assert(queue.size() == 1);
        assert(!queue.empty());
        assert(queue.full());
        assert(queue.front() != nullptr);
        assert(*queue.front() == 10);

        // 满队列不能覆盖原值
        assert(!queue.push(20));
        assert(queue.size() == 1);
        assert(*queue.front() == 10);

        auto value = queue.pop();

        assert(value.has_value());
        assert(*value == 10);

        assert(queue.empty());
        assert(!queue.full());
        assert(queue.size() == 0);
        assert(queue.front() == nullptr);

        // 再次使用同一个槽位
        assert(queue.push(30));
        assert(*queue.front() == 30);

        value = queue.pop();
        assert(value.has_value());
        assert(*value == 30);
        assert(queue.empty());
    }

    // ============================================================
    // 3. 空队列 pop / front
    // ============================================================
    {
        RingQueue<int> queue(3);

        assert(queue.empty());
        assert(queue.size() == 0);

        assert(queue.front() == nullptr);

        auto value = queue.pop();
        assert(!value.has_value());

        // 失败的 pop 不应该破坏状态
        assert(queue.empty());
        assert(queue.size() == 0);
        assert(queue.front() == nullptr);
    }

    // ============================================================
    // 4. 满队列继续 push
    // ============================================================
    {
        RingQueue<int> queue(3);

        assert(queue.push(10));
        assert(queue.push(20));
        assert(queue.push(30));

        assert(queue.full());
        assert(queue.size() == 3);

        // 不允许覆盖 10
        assert(!queue.push(40));

        assert(queue.size() == 3);
        assert(queue.full());
        assert(queue.front() != nullptr);
        assert(*queue.front() == 10);

        assert(queue.pop() == 10);
        assert(queue.pop() == 20);
        assert(queue.pop() == 30);

        assert(queue.empty());
    }

    // ============================================================
    // 5. 环绕
    // ============================================================
    {
        RingQueue<int> queue(3);

        assert(queue.push(10));
        assert(queue.push(20));
        assert(queue.push(30));

        assert(queue.pop() == 10);
        assert(queue.pop() == 20);

        // 这里应该写回底层数组前面已经空出的槽位
        assert(queue.push(40));
        assert(queue.push(50));

        assert(queue.full());
        assert(queue.size() == 3);

        // FIFO 顺序必须仍然正确
        assert(queue.pop() == 30);
        assert(queue.pop() == 40);
        assert(queue.pop() == 50);

        assert(queue.empty());
        assert(queue.size() == 0);
        assert(queue.front() == nullptr);
    }

    // ============================================================
    // 5.1 多轮环绕
    // ============================================================
    {
        RingQueue<int> queue(4);

        int nextPush = 0;
        int nextPop = 0;

        // 反复进行很多次 push/pop，
        // 强迫 read/write 多次绕过数组末尾。
        for (int round = 0; round < 100; ++round) {
            while (!queue.full()) {
                assert(queue.push(nextPush));
                ++nextPush;
            }

            assert(queue.size() == 4);

            for (int i = 0; i < 2; ++i) {
                auto value = queue.pop();

                assert(value.has_value());
                assert(*value == nextPop);

                ++nextPop;
            }

            assert(queue.size() == 2);
        }

        // 把最后剩下的元素全部读完
        while (!queue.empty()) {
            auto value = queue.pop();

            assert(value.has_value());
            assert(*value == nextPop);

            ++nextPop;
        }

        assert(nextPop == nextPush);
        assert(queue.empty());
    }

    // ============================================================
    // 6. 不可复制，只能移动
    // ============================================================
    {
        RingQueue<RenderCommand> commands(2);

        assert(commands.empty());

        assert(commands.push(RenderCommand{ 7 }));

        assert(commands.size() == 1);
        assert(commands.front() != nullptr);
        assert(commands.front()->passId == 7);

        assert(commands.push(RenderCommand{ 8 }));
        assert(commands.full());

        auto command1 = commands.pop();

        assert(command1.has_value());
        assert(command1->passId == 7);

        auto command2 = commands.pop();

        assert(command2.has_value());
        assert(command2->passId == 8);

        assert(commands.empty());
    }

    // ============================================================
    // 7. pop 后槽位中的对象应该被销毁
    // ============================================================
    {
        assert(Tracked::alive == 0);

        RingQueue<Tracked> queue(2);

        assert(Tracked::alive == 0);

        // 临时对象先构造，然后移动到 queue，
        // push 表达式结束后临时对象析构。
        assert(queue.push(Tracked{ 42 }));

        // 最终队列内部应该持有一个 Tracked
        assert(Tracked::alive == 1);
        assert(queue.size() == 1);

        {
            auto value = queue.pop();

            assert(value.has_value());
            assert(value->value == 42);

            /*
             * pop 返回后：
             *
             * 队列内部原来的对象应该已经被销毁；
             * 当前只剩 value 里的那个 Tracked。
             */
            assert(Tracked::alive == 1);

            assert(queue.empty());
            assert(queue.size() == 0);
            assert(queue.front() == nullptr);
        }

        // value 离开作用域，最后一个对象析构
        assert(Tracked::alive == 0);
    }

    // ============================================================
    // 8. front 始终指向当前队首
    // ============================================================
    {
        RingQueue<int> queue(4);

        assert(queue.push(100));
        assert(queue.push(200));
        assert(queue.push(300));

        assert(queue.front() != nullptr);
        assert(*queue.front() == 100);

        assert(queue.pop() == 100);

        assert(queue.front() != nullptr);
        assert(*queue.front() == 200);

        assert(queue.pop() == 200);

        assert(queue.front() != nullptr);
        assert(*queue.front() == 300);

        assert(queue.pop() == 300);

        assert(queue.front() == nullptr);
    }

    // ============================================================
    // 9. const RingQueue 也能调用查询接口
    // ============================================================
    {
        RingQueue<int> queue(2);

        assert(queue.push(123));

        const RingQueue<int>& constQueue = queue;

        assert(constQueue.capacity() == 2);
        assert(constQueue.size() == 1);
        assert(!constQueue.empty());
        assert(!constQueue.full());

        const int* front = constQueue.front();

        assert(front != nullptr);
        assert(*front == 123);
    }

    std::cout << "All RingQueue tests passed.\n";
}