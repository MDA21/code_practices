#include "EntityHandlePool/EntityHandlePool.h"
#include "ScheduleTask/ScheduleTask.h"
#include "RingQueue/RingQueue.h"
#include "SparseSet/SparseSet.h"
#include "VisibilityCull/VisibilityCull.h"
#include "DeferredRemove/DeferredRemove.h"

int main() {
	//testEntityAllocator();
	//testScheduleTasks();
	//testRingQueue();
	//testSparseSet();
	//testRemoveInvisible();
	testDeferredRemove();
}