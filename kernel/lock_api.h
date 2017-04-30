#ifndef LOCK_API_H
#define LOCK_API_H

#include <kernel/spinlock.h>


#define GET_SHARED_VAR_SMP_SPIN(name)    do{\
	static int first##_name = 1;            \
	if(first##_name)                        \
	{                                       \
		spin_initlock(&name);               \
		first##_name = 0;                   \
	}                                       \
	spin_lock(&name);                       \
}while(0)     
/*
#define GET_SHARED_VAR_SMP_SPIN(name)    do{\
	spin_initlock(&name);                   \
	spin_lock(&name);                       \
}while(0)     
*/
#define UNLOCK_SHARED_VAR_SMP_SPIN(name) spin_unlock(&name);


#endif
