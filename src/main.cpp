#include "EntityHandlePool/EntityHandlePool.h"
#include "ScheduleTask/ScheduleTask.h"
#include "RingQueue/RingQueue.h"
#include "SparseSet/SparseSet.h"
#include "VisibilityCull/VisibilityCull.h"
#include "DeferredRemove/DeferredRemove.h"
#include "MatrixTraversal/MatrixTraversal.h"
#include "MatrixTranspose/MatrixTranspose.h"
#include <exception>
#include <iostream>
#include <string_view>

int main(int argc, char* argv[]) {
	//testEntityAllocator();
	//testScheduleTasks();
	//testRingQueue();
	//testSparseSet();
	//testRemoveInvisible();
	//testDeferredRemove();
	try {
		//testMatrixTraversal();
		testMatrixTranspose();
		if (argc > 1 && std::string_view(argv[1]) == "--benchmark") {
			benchmarkMatrixTranspose();
		}
	}
	catch (const std::exception& error) {
		std::cerr << error.what() << '\n';
		return 1;
	}
}
