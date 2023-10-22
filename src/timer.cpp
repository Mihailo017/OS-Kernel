/*
 * timer.cpp
 *
 *  Created on: Jul 1, 2019
 *      Author: Mihailo Mladenovic
 */

#include "timer.h"
#include "PCB.h"
#include "SCHEDULE.H"
#include "semaphor.h"
#include <stdlib.h>
#include <iostream.h>
#include "KerSem.h"

volatile unsigned tsp;
volatile unsigned tss;
volatile unsigned tbp;
volatile unsigned counter = 1;
volatile unsigned softLock = 0;
// Vrednosti index-a : (isteklo 55 milisekundi) -1, (dispatch) 0, (waitToComplete) 1
volatile int sys_handler_index = -1;

extern void tick();

void interrupt timer() {
	/* Ako je razlog prekida to sto je isteklo 55 milis i nit ima timeSlice
	* samim tim je i counter razlicit od nule, smanji counter */
	if (sys_handler_index == -1 && counter) counter--;
	/*  Prvi clan u ILI : Ako je isteko counter i timeSlice running niti nije 0
	 * (ako je timeSlice nula counter nije bitan) i nije zakljucano sa softLock
	 * onda moze promena konteksta
	 * Drugi clan u ILI : Ako je zahtevana promena konteksta sa dispatch() ili
	 * waitToComplete() onda moze promena konteksta*/

	if (sys_handler_index == -1)  KernelSem::timeBlockedProcessing();
	// Obradjivanje semafora sa vremenski blokiranim nitima

	if ((!counter && PCB::running->time && !softLock) || sys_handler_index != -1) {

		asm {
			mov tsp, sp
			mov tss, ss
			mov tbp, bp
		}

		PCB::running->sp = tsp;
		PCB::running->ss = tss;
		PCB::running->bp = tbp;

		// Znaci ako razlog za poziv tajmera nije waitToComplete, stavi u Scheduler
		// U suprotnom ne stavljaj
		// Isto ako je running == idleThread onda ne stavljaj u Scheduler
		if (sys_handler_index != 1 && PCB::running != PCB::idleThread) Scheduler::put((PCB*)PCB::running);

		// Uzmi iz Schedulera a ako vrati null postavi na idle
		if (!(PCB::running = Scheduler::get())) PCB::running = PCB::idleThread;

		counter = PCB::running->time;

		tsp = PCB::running->sp;
		tss = PCB::running->ss;
		tbp = PCB::running->bp;

		asm {
			mov sp, tsp
			mov ss, tss
			mov bp, tbp
		}

		PCB::signals_processing();
	}
	//Ako je doslo do prekida zbog isteka 55 milis onda pozovi staru rutinu prebacenu u 60h
	if (sys_handler_index == -1) {
		tick();
		asm int 60h
	} else // Ako nije postavi index na default vrednost
		sys_handler_index = -1;
}

// Sistemski pozivi

void sys_call_dispatch() { // Mora da je thread-safe jer se poziva iz korisnickog rezima
	asm cli
	sys_handler_index = 0;
	asm int 08h
	asm sti
}

void sys_call_block() {
	sys_handler_index = 1;
	asm int 08h
}

// Zabrana preuzimanja

void softlock() {
	asm pushf
	asm cli
	softLock = 1;
	asm popf
}
void softunlock() {
	asm pushf
	asm cli
	softLock = 0;
	asm popf
}

// Postavljanje i restauracija tajmera
unsigned oldTimerOFF, oldTimerSEG;

void timer_override() {
	asm{
		cli
		push es
		push ax

		mov ax,0
		mov es,ax

		mov ax, word ptr es:0022h //; pamti staru rutinu
		mov word ptr oldTimerSEG, ax
		mov ax, word ptr es:0020h
		mov word ptr oldTimerOFF, ax

		mov word ptr es:0022h, seg timer	 //postavlja
		mov word ptr es:0020h, offset timer //novu rutinu

		mov ax, oldTimerSEG	 //	postavlja staru rutinu
		mov word ptr es:0182h, ax //; na int 60h
		mov ax, oldTimerOFF
		mov word ptr es:0180h, ax

		pop ax
		pop es
		sti
	}
}

void timer_restore() {
	asm {
		cli
		push es
		push ax

		mov ax,0
		mov es,ax

		mov ax, word ptr oldTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es
		sti
	}
}
