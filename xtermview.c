/*
 * xtermview.c: a terminal-based X server viewer
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>
#include <signal.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xfixes.h>

Display *dpy;
Damage  root_dmg;
int w, h,
    tw, th,
    rw, rh,
    is_halting,
    dmg_evt, dmg_err;

/*
 * Help message
 */
void help(){
  printf("\x1b[36mxtermview: A terminal-based X server viewer.\n\x1b[33mUsage: xtermview\n\x1b[38;2;0;200;0mIf this text is green, truecolor is supported by this terminal. If not, xtermview may not work properly.\n\x1b[0m");
  exit(0);
}

/*
 * Handle interrupt and shut down
 * gracefully
 */
void shutdown(int signo){
  if(!is_halting){
    is_halting = 1; /* Prevent double free due to multiple interrupts being fired */
    /* Disable mouse, show the cursor */
    printf("\e[?1003l\x1b[?25h");
    XCloseDisplay(dpy);
    exit(0);
  }
}

/* 
 * Disable echo (printing of typed characters)
 * and canonical mode (stdin being read one line at a time
 * rather than one byte at a time)
 */
void raw(){
  struct termios raw;
  tcgetattr(0, &raw);
  raw.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(0, TCSAFLUSH, &raw);
}

/*
 * Obtain the contents of the X display,
 * draw them to the terminal
 */
void draw(){
  int x, y;
  unsigned long p;
  XImage *img;

  img = XGetImage(
    dpy,
    DefaultRootWindow(dpy),
    0, 0,
    w, h,
    AllPlanes,
    ZPixmap
  );

  /* Enable mouse, hide the cursor, set the cursor position to the top left corner */
  printf("\e[?1003h\x1b[?25l\x1b[0;0H");
  for(y=0;y<th;y++){
    for(x=0;x<tw;x++){
      p = XGetPixel(img, rw*x, rh*y);
      /* Set background color using truecolor (RGB) */
      printf(
        "\x1b[48;2;%i;%i;%im ",
        p >> 16,
        (p >> 8) & 0xff,
        p & 0xff
      );
    }
  }
  
  XDestroyImage(img);
}

/*
 * Adjust view window on terminal resize
 */
void resize(int signo){
  struct winsize s;
  ioctl(1, TIOCGWINSZ, &s);
  tw = s.ws_col;
  th = s.ws_row;
  rw = (w/tw);
  rh = (h/th);
  draw(0, 0);
}

/*
 * Send a mouse event
 */
void mouse(int button, int state, int x, int y){
  /*char buf[256];
  sprintf(buf, "xdotool mousemove %i %i", x, y);
  system(buf);
  */
}

/*
 * Send a keyboard event
 */
void keyboard(int state, char c){
  /*
  char buf[256];
  sprintf(buf, "xdotool key %c", c);
  system(buf);
  */
}

/*
 * Handle mouse and keyboard events
 * in a loop
 */
void loop(){
  int i, count;
  char c, buf[6];
  XEvent evt;
  XDamageNotifyEvent *dmg;
  XserverRegion region;
  XRectangle *area, rect;

  /* Read from both the X display as well as stdin */
  int n_ready_fds;
  struct timeval tv;
  fd_set in_fds;

  FD_ZERO(&in_fds);
  FD_SET(0, &in_fds);
  FD_SET(ConnectionNumber(dpy), &in_fds);
  tv.tv_usec = 0;
  tv.tv_sec = 10;

  n_ready_fds = select(2, &in_fds, NULL, NULL, &tv);
  if(n_ready_fds > 0){
    if(FD_ISSET(0, &in_fds)){
      if((c = getchar()) == 0x1b){
        read(0, buf, 6);
        if(buf[1] == 0x5b &&
           buf[2] == 0x4d &&
           (buf[3] == 0x20 ||
            buf[3] == 0x22 ||
            buf[3] == 0x23)){
          mouse(buf[3] == 0x22, buf[3] == 0x20, buf[4], buf[5]);
          getchar();getchar();getchar();getchar();getchar(); /* This is a terrible, terrible hack */
        }
      } else if(c != '\0'){
        keyboard(1, c);
      }
    }
    if(evt.type == dmg_evt+XDamageNotify){
      dmg = (XDamageNotifyEvent*)&evt;
      region = XFixesCreateRegion(dpy, NULL, 0);
      XDamageSubtract(
        dpy,
        root_dmg,
        None,
        region
      );
      area = XFixesFetchRegion(dpy, region, &count);
      if(area){
        for(i=0;i<count;i++){
          rect = area[i];
          draw();
//          draw(rect.x, rect.y);
        }
      }
      XFixesDestroyRegion(dpy, region);
      XFree(area);
      usleep(500000);
    }
  } else if(n_ready_fds == 0){
    draw();
  }

  while(XPending(dpy)){
    XNextEvent(dpy, &evt);
  }
}

/*
 * Set up signal handlers (Ctrl+C, window resize)
 */
void sighandlers(){
  is_halting = 0;
  signal(SIGINT,  shutdown);
  signal(SIGKILL, shutdown);
  signal(SIGQUIT, shutdown);
  signal(SIGWINCH,  resize);
}

/*
 * Set up X
 */
int x(){
  dpy = XOpenDisplay(NULL);
  if(dpy == NULL){
    printf("Error: Failed to open display.\n");
    return 1;
  }

  w = XWidthOfScreen(DefaultScreenOfDisplay(dpy));
  h = XHeightOfScreen(DefaultScreenOfDisplay(dpy));

  XDamageQueryExtension(dpy, &dmg_evt, &dmg_err);
  root_dmg = XDamageCreate(
    dpy,
    DefaultRootWindow(dpy),
    XDamageReportNonEmpty
  );

  return 0;
}

int main(int argc){
  if(argc > 1){ help(); }
  if(x()){ return 1; }
  sighandlers();
  raw();
  resize(0);
  draw();
  while(1){ loop(); }

  /* Unreachable */
  shutdown(0);

  return 0;
}
