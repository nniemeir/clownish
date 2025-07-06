#ifndef SIGNALS_H
#define SIGNALS_H

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void handler(int signal_num);
int init_sig_handler();

#endif