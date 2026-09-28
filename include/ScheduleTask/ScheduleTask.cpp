#include "ScheduleTask.h"
#include <queue>
#include<functional>
#include <algorithm>

std::optional<std::vector<TaskId>> scheduleTasks(std::uint32_t taskCount,
	const std::vector<std::pair<TaskId, TaskId>>& dependencies) {
	std::vector<std::uint32_t> inDegree(taskCount, 0);
	std::vector<std::vector<TaskId>> graph(taskCount);
	std::priority_queue<TaskId, std::vector<TaskId>, std::greater<TaskId>> q;
	std::vector<TaskId> result;
	result.reserve(taskCount);

	std::vector<std::pair<TaskId, TaskId>> uniqueDependencies = dependencies;
	std::sort(uniqueDependencies.begin(), uniqueDependencies.end());
	auto newEnd = std::unique(uniqueDependencies.begin(), uniqueDependencies.end());
	uniqueDependencies.erase(newEnd, uniqueDependencies.end());

	for (const auto& [a, b] : uniqueDependencies) {
		if(a>=taskCount || b>=taskCount) {
			return std::nullopt;
		}

		graph[a].push_back(b);
		inDegree[b]++;
	}

	for(TaskId i = 0; i < taskCount; ++i) {
		if(inDegree[i] == 0) {
			q.push(i);
		}
	}

	
	while(!q.empty()) {
		auto current = q.top();
		q.pop();
		result.push_back(current);
		for(auto neighbor : graph[current]) {
			inDegree[neighbor]--;
			if(inDegree[neighbor] == 0) {
				q.push(neighbor);
			}
		}
	}
	
	if(result.size() != taskCount) {
		return std::nullopt;
	}
	return result;
}

void testScheduleTasks() {
	//empty tasks
	{
		const auto result = scheduleTasks(0, {});

		assert(result.has_value());
		assert(result->empty());
	}

	//no dependencies
	{
		const auto result = scheduleTasks(5, {});

		assert(result.has_value());
		assert(*result == std::vector<TaskId>({ 0, 1, 2, 3, 4 }));
	}
	
	//simple dependencies
	{
		const auto result = scheduleTasks(
			5,
			{
				{0, 1},
				{1, 2},
				{2, 3},
				{3, 4}
			});

		assert(result.has_value());
		assert(*result == std::vector<TaskId>({ 0, 1, 2, 3, 4 }));
	}

	//backward dependencies
	{
		const auto result = scheduleTasks(
			4,
			{
				{3, 2},
				{2, 1},
				{1, 0}
			});

		assert(result.has_value());
		assert(*result == std::vector<TaskId>({ 3, 2, 1, 0 }));
	}

	//multi tasks
	{
		const auto result = scheduleTasks(
			5,
			{
				{0, 3},
				{1, 3},
				{2, 4}
			});

		assert(result.has_value());
		assert(*result == std::vector<TaskId>({ 0, 1, 2, 3, 4 }));
	}

	//dynamic
	{
		const auto result = scheduleTasks(
			5,
			{
				{0, 2},
				{1, 2},
				{2, 3}
			});

		assert(result.has_value());
		assert(*result == std::vector<TaskId>({ 0, 1, 2, 3, 4 }));
	}

	//split
	{
		const auto result = scheduleTasks(
			6,
			{
				{0, 2},
				{0, 3},
				{1, 3},
				{2, 4},
				{3, 4},
				{4, 5}
			});

		assert(result.has_value());
		assert(*result == std::vector<TaskId>({ 0, 1, 2, 3, 4, 5 }));
	}

	//simple cycle
	{
		const auto result = scheduleTasks(
			2,
			{
				{0, 1},
				{1, 0}
			});

		assert(!result.has_value());
	}

	//complex cycle
	{
		const auto result = scheduleTasks(
			5,
			{
				{0, 1},
				{1, 2},
				{2, 3},
				{3, 1}
			});

		assert(!result.has_value());
	}

	//self dependency
	{
		const auto result = scheduleTasks(
			3,
			{
				{2, 2}
			});

		assert(!result.has_value());
	}

	//重复依赖
	{
		const auto result = scheduleTasks(
			3,
			{
				{0, 1},
				{0, 1},
				{0, 1},
				{1, 2}
			});

		assert(result.has_value());
		assert(*result == std::vector<TaskId>({ 0, 1, 2 }));
	}

	//大量重复依赖
	{
		const auto result = scheduleTasks(
			4,
			{
				{0, 2},
				{0, 2},
				{0, 2},
				{0, 2},

				{1, 2},
				{1, 2},
				{1, 2},

				{2, 3},
				{2, 3}
			});

		assert(result.has_value());
		assert(*result == std::vector<TaskId>({ 0, 1, 2, 3 }));
	}

	//非法起点编号
	{
		const auto result = scheduleTasks(
			3,
			{
				{3, 1}
			});

		assert(!result.has_value());
	}

	//非法终点编号
	{
		const auto result = scheduleTasks(
			3,
			{
				{1, 100}
			});

		assert(!result.has_value());
	}

	//多个互不相关子图
	{
		const auto result = scheduleTasks(
			6,
			{
				{0, 2},
				{1, 3},
				{4, 5}
			});

		assert(result.has_value());
		assert(*result == std::vector<TaskId>({ 0, 1, 2, 3, 4, 5 }));
	}

	//others
	{
		const auto result = scheduleTasks(
			7,
			{
				{0, 4},
				{1, 4},
				{1, 5},
				{2, 5},
				{4, 6},
				{5, 6}
			});

		assert(result.has_value());
		assert(*result == std::vector<TaskId>({ 0, 1, 2, 3, 4, 5, 6 }));
	}

	{
		const auto result = scheduleTasks(
			4,
			{
				{2, 1},
				{1, 0}
			});

		assert(result.has_value());
		assert(*result == std::vector<TaskId>({ 2, 1, 0, 3 }));
	}
}