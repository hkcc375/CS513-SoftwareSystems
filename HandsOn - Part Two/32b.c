#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
	key_t semKey;
	int semIdentifier;
	int semctlStatus;
	int semopStatus;

	key_t shmKey;
	int shmIdentifier;
	ssize_t shmSize = 20;
	char *shmPointer;

	// ============ Creating / Getting Semaphore ==========
	union semun
	{
		int val;
		struct semid_ds *buf;
		unsigned short *array;
		struct seminfo *__buf;
	} semSet;

	semKey = ftok( ".", 3321 );
	if( semKey == -1 )
	{
		perror( "Error while computing key!" );
		_exit( 1 );
	}

	semIdentifier = semget( semKey, 1, 0 );
	if( semIdentifier == -1 )
	{
		semIdentifier = semget( semKey, 1, IPC_CREAT | 0700 );
		if( semIdentifier == -1 )
		{
			perror( "Error while creating semaphore!" );
			_exit( 1 );
		}

		semSet.val   = 1;
		semctlStatus = semctl( semIdentifier, 0, SETVAL, semSet );
		if( semctlStatus == -1 )
		{
			perror( "Error while initializing a binary sempahore!" );
			_exit( 1 );
		}
	}

	// ================================================

	shmKey = ftok( ".", 3322 );

	if( shmKey == -1 )
	{
		perror( "Error while computing key!" );
		_exit( 0 );
	}

	shmIdentifier = shmget( shmKey, shmSize, IPC_CREAT | 0700 );

	if( shmIdentifier == -1 )
	{
		perror( "Error while getting Shared Memory!" );
		_exit( 0 );
	}

	// printf("Shared Memory Identifier: %d\n", shmIdentifier);

	shmPointer = shmat( shmIdentifier, ( void * ) 0, 0 );

	if( *shmPointer == -1 )
	{
		perror( "Error while attaching address space!" );
		_exit( 0 );
	}

	// ========================================================

	struct sembuf semOp;
	semOp.sem_num = 0;
	semOp.sem_flg = 0;

	printf( "Press enter to lock the critical section!\n" );
	getchar();

	semOp.sem_op = -1;
	semopStatus  = semop( semIdentifier, &semOp, 1 );
	if( semopStatus == -1 )
	{
		perror( "Error while operating on semaphore!" );
		_exit( 1 );
	}

	printf( "Critical Section is now locked!\n" );

	printf( "Start of the critical section!\n" );

	// =========== Start of Critical Section ===========

	printf( "Writing to the shared memory!\n" );
	sprintf( shmPointer, "Yolo" );

	printf( "Press enter to read from the shared memory!\n" );
	getchar();

	// Reading from the shared memory
	printf( "%s\n", shmPointer );

	printf( "Press enter to exit the critical section!\n" );
	getchar();

	// =========== End of Critical Section =============

	semOp.sem_op = 1;
	semopStatus  = semop( semIdentifier, &semOp, 1 );
	if( semopStatus == -1 )
	{
		perror( "Error while operating on semaphore!" );
		_exit( 1 );
	}

	printf( "Critical section is now unlocked!\n" );
}