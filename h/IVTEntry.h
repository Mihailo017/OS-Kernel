/*
 * IVTEntry.h
 *
 *  Created on: Jul 11, 2019
 *      Author: Mihailo Mladenovic
 */

#ifndef IVTENTRY_H_
#define IVTENTRY_H_

#include "Event.h"


typedef void interrupt (*pInterrupt)(...);

class IVTEntry {

private:
	IVTNo ivtNo;

public:
	IVTEntry(IVTNo, pInterrupt);
	~IVTEntry();

	static KernelEv* events[256];
	static pInterrupt routines[256];

	void signal();
	void callOld();
};

#define PREPAREENTRY(IVTNo,callold)\
		void interrupt inter##IVTNo(...);\
		IVTEntry newEntry##IVTNo(IVTNo, inter##IVTNo);\
		void interrupt inter##IVTNo(...){\
			newEntry##IVTNo.signal();\
			if(callold==1)\
				newEntry##IVTNo.callOld();\
		}\

#endif /* IVTENTRY_H_ */
