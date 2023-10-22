/*
 * PCB.h
 *
 *  Created on: Jun 30, 2019
 *      Author: Mihailo Mladenovic
 */
#ifndef _PCB_H_
#define _PCB_H_
#include "timer.h"
#include "Thread.h"
#include "semaphor.h"
#include "List.h"

// Poslednje stanje je: READY_AFTER_TIMEBLOCKED
enum State { NEW, READY, BLOCKED, FINISHED, WAITING, READY_ATB };

struct PCBNode {
	PCB* pcb;
	PCBNode* next;
	PCBNode(PCB* pcb) {
		this->pcb = pcb;
		next = 0;
	}
};

StackSize const maxStackSize = (1 << 16); // 64KB
StackSize const minStackSize = 30; // Otprilike tolko treba min

class PCB {

private:
	volatile unsigned *stack;
	ID id;
	static ID nextID;
	PCBNode* waitingHead;
	volatile PCBNode* readyListPrev;
	Semaphore* waitToCompleteSem;


	void setup_stack(StackSize stackSize);
	static void finish();

	PCB(void (*idle)());
	static void idle();

	//Dodato za signali
	List* signals;
	List** handlers;
	unsigned local_blocked_signals;
	static unsigned global_blocked_signals;

public:
	volatile unsigned sp;
	volatile unsigned bp;
	volatile unsigned ss;
	Time time;
	State state;
	PCB* parent;
	Thread* myThread;

	// Pokazivac na tekucu nit
	volatile static PCB* running;

	// Lista svih spremnih niti
	volatile static PCBNode* readyListHead;

	// Idle nit za kad Scheduler vrati 0
	static PCB* const idleThread;

	// Konstruktori i destruktori
	PCB(StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice, Thread* thread);
	~PCB();

	// Metode za rad sa nitima
	void start();
	void waitToComplete();

	//Set i get metode
	ID getID();
	Thread* getThread();
	static ID getRunningId();
	static Thread * getThreadById(ID id);

	// Metode za rad sa listama
	void static addToList(PCBNode** head, PCB* pcb);

	// Staticka metoda koja poziva polimorfnu run()
	static void wrapper();

	// Dodato za signali
	void signal(SignalId);
	void registerHandler(SignalId signal, SignalHandler handler);
	void unregisterAllHandlers(SignalId id);
	void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);
	void blockSignal(SignalId signal);
	static void blockSignalGlobally(SignalId signal);
	void unblockSignal(SignalId signal);
	static void unblockSignalGlobally(SignalId signal);

	static void signals_processing();
	static void killThread();

	static Thread* mainThread;
};

#endif
