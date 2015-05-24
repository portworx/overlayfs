#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int main() {

#define BUF_SZ 512
	char wrbuf[BUF_SZ], rdbuf[BUF_SZ];
	int fd1, fd2;

	/* Pick a file from the lower layer that has never been written */
	const char *filename = "/usr/share/i18n/locales/ss_ZA";
	memset(wrbuf,'a',BUF_SZ);

	/* 
	 * Open it RO gets opened in the lower layer as long as it was never
	 * written to before.
	 */

	fd1 = open(filename, O_RDONLY);
	if (fd1 < 0) {
		fprintf(stderr, "Failed to open lower layer file: %s error %s\n",
				filename, strerror(errno));
		exit(1);
	}
	/* Open it RW so that it gets opened in the upper layer. */
	fd2 = open(filename, O_RDWR);

	if (fd2 < 0) {
		fprintf(stderr, "Failed to open upper layer file: %s error %s\n",

				filename, strerror(errno));
		exit(2);

	}

	/* Write to upper layer */
	if (write(fd2, wrbuf, BUF_SZ) != BUF_SZ) {
		fprintf(stderr, "Failed to write to (upper) file: %s error %s\n",
				filename, strerror(errno));
		exit(3);
	}

	/* Read from lower layer */
	if (read(fd1, rdbuf, BUF_SZ) != BUF_SZ) {
		fprintf(stderr, "Failed to read from (lower) file: %s error %s\n",
				filename, strerror(errno));
		exit(4);
	}

	/* Read from lower layer */
	if (memcmp(wrbuf, rdbuf, BUF_SZ) != 0) {
		fprintf(stderr, "Data Mismatch!\n");
		exit(5);
	}
	fprintf(stdout, "Success!\n");
}


