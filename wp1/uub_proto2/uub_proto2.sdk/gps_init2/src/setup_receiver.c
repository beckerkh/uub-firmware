/*
 * Placeholder PetaLinux user application.
 *
 * Replace this with your application code
 */
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	//printf("Hello, PetaLinux World!\n");
	//printf("cmdline args:\n");
	//while(argc--)
	//	printf("%s\n",*argv++);
	int gpsuart;
	gpsuart=open("/dev/ttyUL1", O_RDWR | O_NOCTTY );

	char buffsend[7]={64,64,67,106,41,13,10}; //@@Cj Identity message
	write(gpsuart,buffsend,sizeof(buffsend));

	char buffsend2[8]={64,64,71,99,1,37,13,10}; //@@Gc PPS control
	write(gpsuart,buffsend2,sizeof(buffsend2));

	char buffsend3[8]={64,64,72,97,1,40,13,10}; //@@Ha Position/time status
	write(gpsuart,buffsend3,sizeof(buffsend3));

	char buffsend4[8]={64,64,72,110,1,41,13,10}; //@@Hn T-RAIM/sawtooth
	write(gpsuart,buffsend4,sizeof(buffsend4));
	printf("The receiver's settings should now be correct\n");
	close(gpsuart);

	return 0;
}


