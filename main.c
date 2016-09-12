#include <sys/resource.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "gfa.h"

int main_fmt(int argc, char *argv[]);

void liftrlimit()
{
#ifdef __linux__
	struct rlimit r;
	getrlimit(RLIMIT_AS, &r);
	r.rlim_cur = r.rlim_max;
	setrlimit(RLIMIT_AS, &r);
#endif
}

double cputime()
{
	struct rusage r;
	getrusage(RUSAGE_SELF, &r);
	return r.ru_utime.tv_sec + r.ru_stime.tv_sec + 1e-6 * (r.ru_utime.tv_usec + r.ru_stime.tv_usec);
}

double realtime()
{
	struct timeval tp;
	struct timezone tzp;
	gettimeofday(&tp, &tzp);
	return tp.tv_sec + tp.tv_usec * 1e-6;
}

int main(int argc, char *argv[])
{
	int ret = 0, i;
	double t_start;

	liftrlimit();
	if (argc == 1) {
		fprintf(stderr, "Usage: gfa2 <command> <arguments>\n");
		fprintf(stderr, "Commands:\n");
		fprintf(stderr, "  fmt        re-format GFA1/GFA2 files\n");
		fprintf(stderr, "  version    show version number\n");
		return 1;
	}
	t_start = realtime();
	if (strcmp(argv[1], "fmt") == 0) ret = main_fmt(argc-1, argv+1);
	else if (strcmp(argv[1], "version") == 0) {
		puts(GFA_VERSION);
		return 0;
	} else {
		fprintf(stderr, "[E::%s] unknown command\n", __func__);
		return 1;
	}
	if (ret == 0) {
		fprintf(stderr, "[M::%s] Version: %s\n", __func__, GFA_VERSION);
		fprintf(stderr, "[M::%s] CMD:", __func__);
		for (i = 0; i < argc; ++i)
			fprintf(stderr, " %s", argv[i]);
		fprintf(stderr, "\n[M::%s] Real time: %.3f sec; CPU: %.3f sec\n", __func__, realtime() - t_start, cputime());
	}
	return ret;
}
