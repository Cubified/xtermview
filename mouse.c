#!/usr/bin/tcc -run

/*
 * mouse.c: a minimal application to print raw
 * escape codes from mouse/keyboard input
 *
 * Requires tcc (for "C script")
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void shutdown(int signo){
  printf("\x1b[?1003l");
  exit(0);
}

int main(){
  printf("\x1b[?1003hPress Ctrl+C to exit.\n");
  signal(SIGINT, shutdown);
  while(1){
    printf("%#x ", getchar());
  }
  return 0;
}
