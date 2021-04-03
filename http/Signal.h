#include <csignal>
typedef void Sigfunc(int); /* for signal handlers */
Sigfunc *Signal(int signo, Sigfunc *func);