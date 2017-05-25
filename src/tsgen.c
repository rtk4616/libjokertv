#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TS_SIZE 188
// about 54MB of data
#define TS_LIMIT 300000

void help() {
	printf("tsgen utility for TS stream generation (pattern)\n");
	printf("Usage:\n");
	printf("	-f filename	TS stream output file\n");
	printf("	-s size		TS file size in bytes (default 54MB)\n");
}

int main(int argc, char **argv) {
	unsigned char pkt[TS_SIZE];
	int counter = 0;
	int pattern = 0;
	int i = 0, j = 0;
	FILE * ofd;
	int c;
	char *filename = "tsgen.ts";
	int size = TS_LIMIT;

	while ((c = getopt (argc, argv, "f:s:")) != -1) {
		switch (c)
		{
			case 's':
				size = atoi(optarg)/TS_SIZE;
				break;
			case 'f':
				filename = optarg;
				break;
			default:
				help();
				return 0;
		}
	}

	ofd = fopen(filename, "w+");
	if (ofd < 0)
		return -1;

	/* pattern should be rolled as 0x00 -> 0xff */
	size = 256 * (size/256);
	printf("ts pkt count=%d \n", size );

	for (i = 0; i < size; i++) {
		pkt[0x00] = 0x47;
		pkt[0x01] = 0x01;
		pkt[0x02] = 0x77;
		pkt[0x03] = 0x10 | counter;

		for (j = 4; j < TS_SIZE; j++) {
			pkt[j] = pattern;
		}

		pattern++;
		if (pattern > 0xFF)
			pattern = 0;

		counter++;
		if (counter > 0x0F)
			counter = 0;

		if (!fwrite(pkt, TS_SIZE, 1, ofd))
			return -1;
	}
	fclose(ofd);

	printf("TS stream generated. Please check %s file ... \n", filename);
}
