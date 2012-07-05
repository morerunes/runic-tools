#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "big.h"
#include "debug.h"

int main(int argc, char **argv)
{
	if (argc < 3) {
		printf("Usage: test.c -f <path\\filename> [-o (true/false)](optional)\n");
		return EXIT_FAILURE;
	} else  {
		int exportall = 0;
		char *filename;
		for (int i = 0; i < argc; i++) {
			if (strcmp(argv[i], "-f") == 0) {
				filename = argv[i+1];
			} else if (strcmp(argv[i], "-o") == 0) {
				if (strcmp(argv[i+1], "true") == 0) {
					exportall++;
				}
			} else if (argv[i][0] == '-') {
				printf("Unrecognized Option '%s': Exiting", argv[i]);
				return EXIT_FAILURE;
			}
		}
		
		BigFile *bigfile = readBigFile(filename);

		printf("File Exported:\t%d\n", exportFileIndex(bigfile, 0, 5642, 1));
	}
	
	return 0;
}
