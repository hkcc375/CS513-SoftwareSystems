#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
	key_t semKey;
	int semIndentifier;
	int semctlStatus;

	semKey = ftok( ".", 1 );
	if( semKey == -1 )
	{
		perror( "Error while computing key!" );
		_exit( 1 );
	}

	semIndentifier = semget( semKey, 1, IPC_CREAT | 0777 );

	if( semIndentifier == -1 )
	{
		perror( "Error while creating semaphore!" );
		_exit( 1 );
	}

	printf( "Press enter to delete the semaphore!\n" );
	getchar();

	semctlStatus = semctl( semIndentifier, 0, IPC_RMID );

	if( semctlStatus == -1 )
	{
		perror( "Error while deleting semaphore!" );
		_exit( 0 );
	}
}