/*
 * Copyright(C) 2011-2016 Pedro H. Penna   <pedrohenriquepenna@gmail.com>
 *              2016-2016 Davidson Francis <davidsondfgl@gmail.com>
 * 
 * This file is part of Nanvix.
 * 
 * Nanvix is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Nanvix is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Nanvix. If not, see <http://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <nanvix/config.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>

/* Test flags. */
#define VERBOSE  (1 << 10)

/* Test flags. */
static unsigned flags = VERBOSE;

/*============================================================================*
 *                               demand_zero_test                             *
 *============================================================================*/

/**
 * @brief Demand zero test module.
 * 
 * @returns Zero if passed on test, and non-zero otherwise.
 */
static int demand_zero_test(void)
{
	const size_t size = PROC_SIZE_MAX/2; /* Buffer size.        */
	char *buffer;                        /* Buffer.             */
	struct tms timing;                   /* Timing information. */
	clock_t t0, t1;                      /* Elapsed times.      */

	buffer = malloc(size);
	assert(buffer != NULL);

	t0 = times(&timing);
	
	/* Fill buffer. */
	for (size_t i = 0; i < size; i++)
		buffer[i] = 1;
	
	t1 = times(&timing);
	
	/* Print timing statistics. */
	if (flags & VERBOSE)
		printf("  Elapsed: %d\n", t1 - t0);
	
	/* House keeping. */
	free(buffer);

	return (0);
}

/*============================================================================*
 *                                  io_test                                   *
 *============================================================================*/

/**
 * @brief I/O testing module.
 * 
 * @details Reads sequentially the contents of the hard disk
            to a in-memory buffer.
 * 
 * @returns Zero if passed on test, and non-zero otherwise.
 */
static int io_test(void)
{
	int fd;            /* File descriptor.    */
	struct tms timing; /* Timing information. */
	clock_t t0, t1;    /* Elapsed times.      */
	char *buffer;      /* Buffer.             */
	
	/* Allocate buffer. */
	buffer = malloc(MEMORY_SIZE);
	if (buffer == NULL)
		exit(EXIT_FAILURE);
	
	/* Open hdd. */
	fd = open("/dev/hdd", O_RDONLY);
	if (fd < 0)
		exit(EXIT_FAILURE);
	
	t0 = times(&timing);
	
	/* Read hdd. */
	if (read(fd, buffer, MEMORY_SIZE) != MEMORY_SIZE)
		exit(EXIT_FAILURE);
	
	t1 = times(&timing);
	
	/* House keeping. */
	free(buffer);
	close(fd);
	
	/* Print timing statistics. */
	if (flags & VERBOSE)
		printf("  Elapsed: %d\n", t1 - t0);
	
	return (0);
}

/*============================================================================*
 *                                sched_test                                  *
 *============================================================================*/

/**
 * @brief Performs some dummy CPU-intensive computation.
 */
static void work_cpu(void)
{
	int c;
	
	c = 0;
		
	/* Perform some computation. */
	for (int i = 0; i < 4096; i++)
	{
		int a = 1 + i;
		for (int b = 2; b < i; b++)
		{
			if ((i%b) == 0)
				a += b;
		}
		c += a;
	}
}

/**
 * @brief Performs some dummy IO-intensive computation.
 */
static void work_io(void)
{
	int fd;            /* File descriptor. */
	char buffer[2048]; /* Buffer.          */
	
	/* Open hdd. */
	fd = open("/dev/hdd", O_RDONLY);
	if (fd < 0)
		_exit(EXIT_FAILURE);
	
	/* Read data. */
	for (size_t i = 0; i < MEMORY_SIZE; i += sizeof(buffer))
	{
		if (read(fd, buffer, sizeof(buffer)) < 0)
			_exit(EXIT_FAILURE);
	}
	
	/* House keeping. */
	close(fd);
}

/**
 * @brief Scheduling test 0.
 * 
 * @details Spawns two processes and tests wait() system call.
 * 
 * @return Zero if passed on test, and non-zero otherwise.
 */
static int sched_test0(void)
{
	pid_t pid;
	
	pid = fork();
	
	/* Failed to fork(). */
	if (pid < 0)
		return (-1);
	
	/* Child process. */
	else if (pid == 0)
	{
		work_cpu();
		_exit(EXIT_SUCCESS);
	}
	
	wait(NULL);
	
	return (0);
}

/**
 * @brief Scheduling test 1.
 * 
 * @details Forces the priority inversion problem, to check how well the system
 *          performs on dynamic priorities.
 * 
 * @returns Zero if passed on test, and non-zero otherwise.
 */
static int sched_test1(void)
{
	pid_t pid;
		
	pid = fork();
	
	/* Failed to fork(). */
	if (pid < 0)
		return (-1);
	
	/* Parent process. */
	else if (pid > 0)
	{
		nice(-2*NZERO);
		work_cpu();
	}
	
	/* Child process. */
	else
	{
		nice(2*NZERO);
		work_io();
		_exit(EXIT_SUCCESS);
	}
		
	wait(NULL);
	
	return (0);
}

/**
 * @brief Scheduling test 1.
 * 
 * @details Spawns several processes and stresses the scheduler.
 * 
 * @returns Zero if passed on test, and non-zero otherwise.
 */
static int sched_test2(void)
{
	pid_t pid[4];
	
	for (int i = 0; i < 4; i++)
	{
		pid[i] = fork();
	
		/* Failed to fork(). */
		if (pid[i] < 0)
			return (-1);
		
		/* Child process. */
		else if (pid[i] == 0)
		{
			if (i & 1)
			{
				nice(2*NZERO);
				work_cpu();
				_exit(EXIT_SUCCESS);
			}
			
			else
			{	
				nice(-2*NZERO);
				pause();
				_exit(EXIT_SUCCESS);
			}
		}
	}
	
	for (int i = 0; i < 4; i++)
	{
		if (i & 1)
			wait(NULL);
			
		else
		{	
			kill(pid[i], SIGCONT);
			wait(NULL);
		}
	}
	
	return (0);
}

/*============================================================================*
 *                             Semaphores Test                                *
 *============================================================================*/

/**
 * @brief Creates a semaphore.
 */
#define SEM_CREATE(a, b) (assert(((a) = semget(b)) >= 0))

/**
 * @brief Initializes a semaphore.
 */
#define SEM_INIT(a, b) (assert(semctl((a), SETVAL, (b)) == 0))

/**
 * @brief Destroys a semaphore.
 */
#define SEM_DESTROY(x) (assert(semctl((x), IPC_RMID, 0) == 0))

/**
 * @brief Ups a semaphore.
 */
#define SEM_UP(x) (assert(semop((x), 1) == 0))

/**
 * @brief Downs a semaphore.
 */
#define SEM_DOWN(x) (assert(semop((x), -1) == 0))

/**
 * @brief Puts an item in a buffer.
 */
#define PUT_ITEM(a, b)                                \
{                                                     \
	assert(lseek((a), 0, SEEK_SET) != -1);            \
	assert(write((a), &(b), sizeof(b)) == sizeof(b)); \
}                                                     \

/**
 * @brief Gets an item from a buffer.
 */
#define GET_ITEM(a, b)                               \
{                                                    \
	assert(lseek((a), 0, SEEK_SET) != -1);           \
	assert(read((a), &(b), sizeof(b)) == sizeof(b)); \
}                                                    \

/**
 * @brief Producer-Consumer problem with semaphores.
 * 
 * @details Reproduces consumer-producer scenario using semaphores.
 * 
 * @returns Zero if passed on test, and non-zero otherwise.
 */
int semaphore_test3(void)
{
	pid_t pid;                  /* Process ID.              */
	int buffer_fd;              /* Buffer file descriptor.  */
	int empty;                  /* Empty positions.         */
	int full;                   /* Full positions.          */
	int mutex;                  /* Mutex.                   */
	const int BUFFER_SIZE = 32; /* Buffer size.             */
	const int NR_ITEMS = 512;   /* Number of items to send. */
	
	/* Create buffer.*/
	buffer_fd = open("buffer", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if (buffer_fd < 0)
		return (-1);
	
	/* Create semaphores. */
	SEM_CREATE(mutex, 1);
	SEM_CREATE(empty, 2);
	SEM_CREATE(full, 3);
		
	/* Initialize semaphores. */
	SEM_INIT(full, 0);
	SEM_INIT(empty, BUFFER_SIZE);
	SEM_INIT(mutex, 1);
	
	if ((pid = fork()) < 0)
		return (-1);
	
	/* Producer. */
	else if (pid == 0)
	{
		for (int item = 0; item < NR_ITEMS; item++)
		{
			SEM_DOWN(empty);
			SEM_DOWN(mutex);
			
			PUT_ITEM(buffer_fd, item);
				
			SEM_UP(mutex);
			SEM_UP(full);
		}

		_exit(EXIT_SUCCESS);
	}
	
	/* Consumer. */
	else
	{
		int item;
		
		do
		{
			SEM_DOWN(full);
			SEM_DOWN(mutex);
			
			GET_ITEM(buffer_fd, item);
				
			SEM_UP(mutex);
			SEM_UP(empty);
		} while (item != (NR_ITEMS - 1));
	}
					
	/* Destroy semaphores. */
	SEM_DESTROY(mutex);
	SEM_DESTROY(empty);
	SEM_DESTROY(full);
	
	close(buffer_fd);
	unlink("buffer");
	
	return (0);
}

/*============================================================================*
 *                                FPU test                                    *
 *============================================================================*/

/**
 * @brief Performs some dummy FPU-intensive computation.
 */
static void work_fpu(void)
{
	const int n = 16; /* Matrix size.    */
	float a[16][16];  /* First operand.  */
	float b[16][16];  /* Second operand. */
	float c[16][16];  /* Result.         */
	
	/* Initialize matrices. */
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			a[i][j] = 1.0;
			a[i][j] = 2.0;
			c[i][j] = 0.0;
		}
	}
	
	/* Perform matrix multiplication. */
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			for (int k = 0; k < n; k++)
				c[i][j] += a[i][k]*b[k][i];
		}
	}
}

/**
 * @brief FPU testing module.
 * 
 * @details Performs a floating point operation, while trying to
            mess up the stack from another process.
 * 
 * @returns Zero if passed on test, and non-zero otherwise.
 */
int fpu_test(void)
{
	pid_t pid;     /* Child process ID.     */
	float a = 6.7; /* First dummy operand.  */
	float b = 1.2; /* Second dummy operand. */
	float result;  /* Result.               */
	
	/* Perform a/b */
	__asm__ volatile(
		"flds %1;"
		"flds %0;"
		"fdivrp %%st,%%st(1);"
		: /* noop. */
		: "m" (b), "m" (a)
	);

	pid = fork();
	
	/* Failed to fork(). */
	if (pid < 0)
		return (-1);
	
	/*
	 * Child process tries
	 * to mess up the stack.
	 */
	else if (pid == 0)
	{
		work_fpu();
		_exit(EXIT_SUCCESS);
	}
	
	wait(NULL);

	/* But it's only in your context,
	 * so nothing changed for father process.
	 */
	__asm__ volatile(
		"fstps %0;"
		: "=m" (result)
	);

	/* 0x40b2aaaa = 6.7/1.2 = 5.5833.. */
	return (result == 0x40b2aaaa);
}


/*============================================================================*
 *                                   main                                     *
 *============================================================================*/

/**
 * @brief Prints program usage and exits.
 * 
 * @details Prints the program usage and exists gracefully.
 */
static void usage(void)
{
	printf("Usage: test [options]\n\n");
	printf("Brief: Performs regression tests on Nanvix.\n\n");
	printf("Options:\n");
	printf("  fpu    Floating Point Unit Test\n");
	printf("  io     I/O Test\n");
	printf("  ipc    Interprocess Communication Test\n");
	printf("  paging Paging System Test\n");
	printf("  sched  Scheduling Test\n");
	
	exit(EXIT_SUCCESS);
}

/**
 * @brief System testing utility.
 */
int main(int argc, char **argv)
{
	/* Missing arguments? */
	if (argc <= 1)
		usage();

	for (int i = 1; i < argc; i++)
	{
		/* I/O test. */
		if (!strcmp(argv[i], "io"))
		{
			printf("I/O Test\n");
			printf("  Result:             [%s]\n", 
				(!io_test()) ? "PASSED" : "FAILED");
		}
		
		/* Paging system test. */
		else if (!strcmp(argv[i], "paging"))
		{
			printf("Demand Zero Test\n");
			printf("  Result:             [%s]\n",
				(!demand_zero_test()) ? "PASSED" : "FAILED");
		}
		
		/* Scheduling test. */
		else if (!strcmp(argv[i], "sched"))
		{
			printf("Scheduling Tests\n");
			printf("  waiting for child  [%s]\n",
				(!sched_test0()) ? "PASSED" : "FAILED");
			printf("  dynamic priorities [%s]\n",
				(!sched_test1()) ? "PASSED" : "FAILED");
			printf("  scheduler stress   [%s]\n",
				(!sched_test2()) ? "PASSED" : "FAILED");
		}
		
		/* IPC test. */
		else if (!strcmp(argv[i], "ipc"))
		{
			printf("Interprocess Communication Tests\n");
			printf("  producer consumer [%s]\n",
				(!semaphore_test3()) ? "PASSED" : "FAILED");
		}

		/* FPU test. */
		else if (!strcmp(argv[i], "fpu"))
		{
			printf("Float Point Unit Test\n");
			printf("  Result [%s]\n",
				(!fpu_test()) ? "PASSED" : "FAILED");
		}
	
	
		/* Wrong usage. */
		else
			usage();
	}
	
	return (EXIT_SUCCESS);
}
