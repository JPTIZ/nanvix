#include <sys/sem.h>
#include <nanvix/klib.h>
#include <semaphore.h>
#include <errno.h>

/**
 * @brief close a semaphore for a given process
 *		 
 * @param	idx		semaphore index (in semaphore
 *					table to close
 *
 * @returns returns 0 in case of successful completion
 *			returns SEM_FAILED otherwise
 */
PUBLIC int sys_semclose(int idx)
{
	int i;

	if (!SEM_IS_VALID(idx))
		return (-EINVAL);
	
	for (i = 0; i < PROC_MAX; i++)
	{
		if (semtable[idx].currprocs[i] == curr_proc->pid)
		{	
			semtable[idx].currprocs[i] = (-1);

			/*
			 * The semaphore is no longer accessible when 0 process use it
			 * and only if it has been unlinked once 
			 */
			if (semtable[idx].nbproc == 0 && (semtable[idx].state&UNLINKED))
				freesem(idx);

			return (0);
		}
	}
	
	return (-1);
}