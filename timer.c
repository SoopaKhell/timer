#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

long get_ms() { // returns milliseconds after jan 1 1970
	struct timeval current_time;
  	gettimeofday(&current_time, NULL);
	return current_time.tv_sec*1000+current_time.tv_usec/1000;
}

int main(int argc, char *argv[]) {
	system("stty -echo");
	while (1) {
		system("stty raw");
		while (1) {
			char ch = getchar();
			if (ch == ' ') {
				printf("Solving... ");
				break;
			} else if (ch == 'q' || ch == '') {
				system("stty cooked");
				exit(1);
			} else if (ch == 'p') {
				printf("shut your wiggly diggly");
			}
		}

		long sec_bef = get_ms(); //before ms timestamp

		// solving
		char finishch = getchar();
		if (finishch == '') {
			system("stty cooked");
			printf("\n");
			exit(1);
		}

		long sec_af = get_ms(); //after ms timestamp

		double time = roundf((sec_af-sec_bef)/10)/100;

		system("stty cooked");
		printf("%.2fs\n", time);

		if (argc > 1) { // if argument given
			if (strcmp(argv[1], "-g") == 0 || strcmp(argv[1], "--google") == 0) { // if argument is -g or --google
				// open the time in google
				char* command[90];
				sprintf(command, "setsid -f xdg-open \"https://www.google.com/search?q=what happened in %d\" > /dev/null 2>&1 &", (int)(time*100));
				system(command);
			}
		}
	}

  	return 0;
}
