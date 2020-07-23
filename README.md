## xtermview

A terminal-based X server viewer.  Currently a work in progress, but reasonably functional.

### Demo

![demo.gif](https://github.com/Cubified/xtermview/blob/master/demo.gif)

### Current Features

- Live-updating view of an X display's contents (both on key press as well as screen update)
- Terminal window resize detection
- Truecolor/16 million color support
- Interaction with server through mouse and keyboard within terminal (WIP)
- No curses/curses-like dependencies, made with raw ANSI escape codes

### Compiling and Running

`xtermview` depends upon Xlib, libXdamage, and libXfixes.  To compile and run:

     $ make
     $ ./xtermview

To exit, press `Ctrl+C`.

### To-Do

- Proper keyboard and mouse support (detection and handling is there, but dispatching to X is not)
- Compression/optimization (i.e. remove unnecessary color changes)
- Cleaner shutdown/exit when output buffer becomes flooded
