#include "x11.h"

static Window find_subwindow(renderer *rnd, Window win, int w, int h) {
	unsigned int i, j;
	Window troot, parent, *children;
	unsigned int n;

	for (i = 0; i < 10; i++) {
		XQueryTree(rnd->display, win, &troot, &parent, &children, &n);

		for (j = 0; j < n; j++) {
			XWindowAttributes attrs;

			if (XGetWindowAttributes(rnd->display, children[j], &attrs)) {
				if (attrs.map_state != 0 && (attrs.width == w && attrs.height == h)) {
					win = children[j];
					break;
				}
			}
		}

		XFree(children);
		if (j == n) {
			break;
		}
	}

	return win;
}

static Window find_desktop_window(renderer *rnd) {
	Window root = RootWindow(rnd->display, rnd->screenNum);
	Window win = root;

	win = find_subwindow(rnd, root, -1, -1);

	win = find_subwindow(rnd, win, rnd->opts->width, rnd->opts->height);

	rnd->root = root;
	rnd->desktop = win;

	return win;
}

void setupWindowForWallpaperMode(renderer *rnd) {
    int attr[] = {
		GLX_X_RENDERABLE    , True,
		GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
		GLX_RENDER_TYPE     , GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
		GLX_RED_SIZE        , 8,
		GLX_GREEN_SIZE      , 8,
		GLX_BLUE_SIZE       , 8,
		GLX_ALPHA_SIZE      , 8,
		GLX_DEPTH_SIZE      , 24,
		GLX_STENCIL_SIZE    , 8,
		GLX_DOUBLEBUFFER    , True,
		GLX_SAMPLE_BUFFERS  , rnd->antialiasSamples > 0 ? 1 : 0,
		GLX_SAMPLES         , rnd->antialiasSamples,
		None
	};

	rnd->screenNum = DefaultScreen(rnd->display);
	rnd->root = RootWindow(rnd->display, rnd->screenNum);

	rnd->opts->width = DisplayWidth(rnd->display, rnd->screenNum),
	rnd->opts->height = DisplayHeight(rnd->display, rnd->screenNum);

	if(!find_desktop_window(rnd)) {
		printf("Error: couldn't find desktop window\n");
		exit(1);
	}

	int elemc;
	rnd->fbcs = glXChooseFBConfig(rnd->display, rnd->screenNum, attr, &elemc);
	if (!rnd->fbcs) {
		printf("Couldn't get FB configs\n");
		exit(1);
	}

	for (int i = 0; i < elemc; i++) {
		rnd->vi = (XVisualInfo *)glXGetVisualFromFBConfig(rnd->display, rnd->fbcs[i]);
		if (!rnd->vi)
			   continue;

		rnd->pict = XRenderFindVisualFormat(rnd->display, rnd->vi->visual);
		XFree(rnd->vi);
		if (!rnd->pict)
			continue;

		rnd->fbc = rnd->fbcs[i];
		if (rnd->pict->direct.alphaMask > 0)
			break;
	}

	XFree(rnd->fbcs);

	rnd->vi = (XVisualInfo *)glXGetVisualFromFBConfig(rnd->display, rnd->fbc);
	if (!rnd->vi) {
		printf("Couldn't get a visual\n");
		exit(1);
	}

	// Window parameters
	rnd->swa.background_pixmap = ParentRelative;
	rnd->swa.background_pixel = 0;
	rnd->swa.border_pixmap = 0;
	rnd->swa.border_pixel = 0;
	rnd->swa.bit_gravity = 0;
	rnd->swa.win_gravity = 0;
	rnd->swa.override_redirect = True;
	rnd->swa.colormap = XCreateColormap(rnd->display, rnd->root, rnd->vi->visual, AllocNone);
	rnd->swa.event_mask = StructureNotifyMask | ExposureMask;
	unsigned long mask = CWOverrideRedirect | CWBackingStore | CWBackPixel | CWBorderPixel | CWColormap;

	rnd->window = XCreateWindow(rnd->display, rnd->root, 0, 0, rnd->opts->width, rnd->opts->height, 0,
			rnd->vi->depth, InputOutput, rnd->vi->visual, mask, &rnd->swa);

	XLowerWindow(rnd->display, rnd->window);

	// Set window type to desktop
	long value = XInternAtom(rnd->display, "_NET_WM_WINDOW_TYPE_DESKTOP", false);
	XChangeProperty(rnd->display, rnd->window,
					XInternAtom(rnd->display, "_NET_WM_WINDOW_TYPE", false),
					XA_ATOM, 32, PropModeReplace, (unsigned char *) &value, 1);
	XMapWindow(rnd->display, rnd->window);
}

void x11_SwapBuffers(renderer *rnd) {
	glXSwapBuffers(rnd->display, rnd->window);
}
void setX11Hints(const renderer *rnd) {
    Display *display = rnd->display;
    Window xwindow = rnd->window;

    // Set WM_CLASS to make the window recognizable
    XClassHint *classHint = XAllocClassHint();
    classHint->res_name = const_cast<char *>(LINUX_NAME_HINT);
    classHint->res_class = const_cast<char *>(LINUX_CLASS_HINT);
    XSetClassHint(display, xwindow, classHint);
    XFree(classHint);
}
