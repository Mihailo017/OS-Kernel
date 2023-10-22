/*
 * Main.cpp
 *
 *  Created on: Sep 17, 2019
 *      Author: Mihailo Mladenovic
 */

#include <iostream.h>
#include "PCB.h"
#include "SCHEDULE.H"
#include "User.h"
#include "timer.h"
#include "semaphor.h"
#include "Thread.h"
#include "user.h"
#include "List.h"
#include "WaitList.h"
#include "math.h"
#include <time.h>
#include "KerSem.h"

extern int userMain (int argc, char* argv[]);

int main (int argc, char* argv[]) {

	PCB::running->state = READY;
	int ret=0;
	timer_override();
	ret = userMain(argc, argv);
	timer_restore();
	delete KernelSem::semsWithWaiting;
	return ret;
}
