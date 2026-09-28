#pragma once
#include <cstdint>
#include <optional>
#include <utility>
#include <vector>
#include <cassert>


using TaskId = std::uint32_t;

// dependency {a, b} 表示：任务 a 必须在任务 b 之前执行。
[[nodiscard]]
std::optional<std::vector<TaskId>> scheduleTasks(
    std::uint32_t taskCount,
    const std::vector<std::pair<TaskId, TaskId>>& dependencies);

void testScheduleTasks();