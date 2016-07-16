#include "./core/util.h"
#include "./core/db/mydb.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	openlog();

	closelog();

	return 1;
}