#include <stdio.h>

#include <exec/types.h>
#include <dos/dos.h>

#include <proto/dos.h>
#include <proto/exec.h>

#define EXIT_SUCCESS 0
#define EXIT_SYNTAX_ERROR 20
#define EXIT_DOS_ERROR 30

#define ARG_FILE	0
#define ARG_ADDRESS	1

static const STRPTR version = "\0$VER: load2mem 1.0 (11.04.2016)\0";
static const STRPTR id = "\0$Id$\0";

ULONG file_size(BYTE *path);
BOOL file_load(BYTE *path, BYTE *filebuf, ULONG filesize);
void usage(void);

void
usage(void)
{
	printf("load2mem FILE/K ADDRESS/K\n");
}

int
main(int argc, char *argv[])
{
	static LONG *argArray;
	struct RDArgs *result;

	ULONG size;
	BYTE *buf, *path;

	CONST_STRPTR argTemplate = "FILE/K,ADDRESS/K";

	if ((LONG) argArray[ARG_FILE] == 0) {
		usage();
		return EXIT_SYNTAX_ERROR;
	}
	if ((LONG) argArray[ARG_ADDRESS] == 0) {
		usage();
		return EXIT_SYNTAX_ERROR;
	}

	path = (STRPTR) argArray[ARG_FILE];

	size = file_size(path);
	buf = (BYTE*) strtol((STRPTR) argArray[ARG_ADDRESS], NULL, 16);

	printf("file is %lu bytes long, will load at %p\n", size, buf);

	file_load(path, buf, size);

	return EXIT_SUCCESS; 
}

ULONG
file_size(BYTE *path)
{ 
	BPTR file;
	ULONG size;
	struct FileInfoBlock *fib;
	struct Library *DOSBase;

	DOSBase = OpenLibrary("dos.library", 36L);
	if (!DOSBase) {
		printf("Error opening dos.library!\n");
		exit(EXIT_DOS_ERROR);
	}

	fib = (struct FileInfoBlock *) AllocDosObject(DOS_FIB, TAG_END);
	if (!fib) {
		printf("Couldn't allocate dos object!\n");

		CloseLibrary(DOSBase);
		exit(EXIT_DOS_ERROR);
	}

	if (file = Lock(path, SHARED_LOCK)) {

		if (Examine(file, fib)) 
			size = fib->fib_Size;
		else
			printf("Couldn't Examine() file!\n"); /* XXX */

		UnLock(file);
	} else {
		printf("Couldn't lock file!\n");

		FreeDosObject(DOS_FIB, fib);
		CloseLibrary(DOSBase);
		exit(EXIT_DOS_ERROR);
	}

	FreeDosObject(DOS_FIB, fib);
	CloseLibrary(DOSBase);
	
	return size;
}

/* load file to memory buffer */
BOOL
file_load(BYTE *path, BYTE *filebuf, ULONG filesize)
{
	BPTR fh;

	if ((fh = Open(path, MODE_OLDFILE)) == -1)  {	
		perror("Error openinig file");
		return FALSE;
	}

	printf("DEBUG: loading %lx bytes long file at %p\n", (ULONG) filesize, (void*) filebuf);

	if (Read(fh, filebuf, filesize) == -1) {
		perror("Error reading file");

		Close(fh);
		return FALSE;
	}

	Close(fh);

	return TRUE;
}

