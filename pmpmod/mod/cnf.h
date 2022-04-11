

#include <pspkernel.h>
#include <pspdebug.h>

#define MAX_BUFFER 1024

struct ConfigFile
{
	PspFile file;
	char str_buf[MAX_BUFFER];
	int  line;
};

int ConfigOpen(const char *filename, struct ConfigFile *cnf);
const char* linkConfigReadNext(struct ConfigFile *cnf, const char **name);
void ConfigClose(struct ConfigFile *cnf);
