#ifndef X11_H
#define X11_H
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrender.h>
#include <X11/extensions/XInput2.h>
#include <renderer.h>
#include <iostream>

#define LINUX_CLASS_HINT "RedTTGMatrix"
#define LINUX_NAME_HINT "matrix"

void setX11Hints(const renderer *rnd);

void setupWindowForWallpaperMode(renderer *rnd);

void x11_SwapBuffers(renderer *rnd);

#endif //X11_H
