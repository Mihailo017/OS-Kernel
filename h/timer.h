/*
 * timer.h
 *
 *  Created on: Jul 1, 2019
 *      Author: Mihailo Mladenovic
 */

#ifndef TIMER_H_
#define TIMER_H_

// Sistemski pozivi

void sys_call_dispatch(); // Explicitno zahtevanje promene konteksta
void sys_call_block(); // Bilo na semaforu ili waitToComplete

// Timer rutina za promenu konteksta
void interrupt timer();

//Inicijalizacija i restauracija tajmerske rutine

void timer_override();
void timer_restore();

//Zabrana preuzimanja

void softlock();
void softunlock();

#endif /* TIMER_H_ */
