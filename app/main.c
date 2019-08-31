#include "lib.h"

void uentry(void){

	/* lab4 test code. */
	createSem();

	int j = 0;
	int pid2 = fork();
	if(pid2 == 0) {
		for(; j < 20; j++) {
			//printf("lockSem\n");
			lockSem();
			printf("PANG");	
		}
	} else {
		for(; j < 20; j++) {
			printf("PING");
			//printf("unlockSem\n");
			unlockSem();
			sleep(5);
		}
		destroySem();
	}

	exit(0);

	while(1);

	/* lab3 test code. */
	int i = 0;
	int pid = fork();

	if(pid == 0) {
		for(; i < 30; i++) {
			//printf("%d. I am the child process: Ping\n", i);
			printf("ping");
			sleep(9);
		}
	} else {
		for(; i < 30; i++) {
			//printf("%d. I am the parent process: Pang\n", i);
			printf("pang");
			sleep(3);
		}
	}

	exit(0);

	while(1);
	
	/* lab2 test code. */
	printf("the answer should be:\n");
	printf("#######################################################\n");
	printf("Hello, welcome to OSlab! I'm the body of the game. ");
	printf("Bootblock loads me to the memory position of 0x100000, and Makefile also tells me that I'm at the location of 0x100000. ");
	printf("~!@#\(^&*()_+`1234567890-=...... ");
	printf("Now I will test your printf: ");
	printf("1 + 1 = 2, 123 * 456 = 56088\n0, -1, -2147483648, -1412505855, -32768, 102030\n0, ffffffff, 80000000, abcdef01, ffff8000, 18e8e\n");
	printf("#######################################################\n");
	printf("\f");
	printf("your answer:\n");
	printf("=======================================================\n");
	printf("%s %s%scome %co%s", "Hello,", "", "wel", 't', " ");
	printf("%c%c%c%c%c! ", 'O', 'S', 'l', 'a', 'b');
	printf("I'm the %s of %s. %s 0x%x, %s 0x%x. ", "body", "the game", "Bootblock loads me to the memory position of", 0x100000, "and Makefile also tells me that I'm at the location of", 0x100000);
	printf("~!@#\(^&*()_+`1234567890-=...... ");
	printf("Now I will test your printf: ");
	printf("%d + %d = %d, %d * %d = %d\n", 1, 1, 1 + 1, 123, 456, 123 * 456);
	printf("%d, %d, %d, %d, %d, %d\n", 0, 0xffffffff, 0x80000000, 0xabcedf01, -32768, 102030);
	printf("%x, %x, %x, %x, %x, %x\n", 0, 0xffffffff, 0x80000000, 0xabcedf01, -32768, 102030);
	printf("=======================================================\n");
	printf("Test end!!! Good luck!!!\n");
	
	while(1);
	
}
