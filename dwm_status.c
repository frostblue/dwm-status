#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h>
#include <signal.h>
#include <sys/sysinfo.h>

#include <X11/Xlib.h>

#define LOADAVG_SCALE 65536
#define STATUS_MAXLEN 128

#define eprintf(...) fprintf(stderr, __VA_ARGS__)


void convert_loadavg (unsigned long const [3], double [3]);
void signal_handler (int);
void xsetroot (char const *);


static int should_quit = 0;


void
convert_loadavg (unsigned long const loads[3], double dloads[3])
{
	for (int i = 0; i < 3; i++)
		dloads[i] = (double) loads[i] / (double) LOADAVG_SCALE;
}

void
signal_handler (int signal)
{
	switch (signal) {
	case SIGINT:
		printf("[dwm_status] SIGINT, quitting\n");
		should_quit = 1;
		break;
	case SIGTERM:
		printf("[dwm_status] SIGTERM, quitting\n");
		should_quit = 1;
	}
}

void
xsetroot (char const *name)
{
	Display *display;

	display = XOpenDisplay(0);
	if (!display) {
		eprintf("[dwm_status] can't open display\n");
		exit(EXIT_FAILURE);
	}

	XStoreName(display, DefaultRootWindow(display), name);
	XSync(display, 0);

	XCloseDisplay(display);
}

int
main (void)
{
	char status[STATUS_MAXLEN];
	struct sysinfo system_info;
	double load_avg[3];
	struct tm *t;
	time_t time_current;
	FILE *battery_file;
	char battery_percent_str[4] = {0};
	struct timespec ts = {
		.tv_sec = 2,
		.tv_nsec = 0
	};

	signal(SIGINT,  &signal_handler);
	signal(SIGTERM, &signal_handler);

	while (!should_quit) {
		sysinfo(&system_info);
		convert_loadavg(system_info.loads, load_avg);

		time(&time_current);
		t = localtime(&time_current);

		battery_file = fopen("/sys/class/power_supply/BAT0/capacity", "r");
		fgets(battery_percent_str, 4, battery_file);
		fclose(battery_file);

		snprintf(status, STATUS_MAXLEN, "%.2lf - %s%% - %02d:%02d - %02d/%02d",
				load_avg[0],
				battery_percent_str,
				t->tm_hour,
				t->tm_min,
				t->tm_mday,
				t->tm_mon + 1
		);
		xsetroot(status);
		nanosleep(&ts, NULL);
	}

	return EXIT_SUCCESS;
}
