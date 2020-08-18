#include <stdbool.h>
#include <stdio.h>

#include <X11/Xlib.h>

#define WINDOW_X 100
#define WINDOW_Y 100
#define WINDOW_W 640
#define WINDOW_H 480
#define WINDOW_BORDER_WIDTH 1
#define WINDOWS_MAX 8

typedef struct {
    Window window;
    int    screen;
    Atom   delWindow;
    XEvent event;
} SampleWindow;

typedef struct {
    Display     *display;
    size_t       windowsCount;
    SampleWindow windows[WINDOWS_MAX];
} SampleDisplay;

typedef enum {
    SET_UNKNOWN,
    SET_KEY_PRESS,
    SET_KEY_RELEASE,
    SET_KEY_PRESSSED,
} SampleEventType;

typedef struct {
    SampleEventType type;
} SampleEventUnknown;

typedef struct {
    SampleEventType type;
    SampleWindow *window;
    unsigned long time;
    unsigned key;
} SampleEventKey;

typedef SampleEventKey SampleEventKeyPress;
typedef SampleEventKey SampleEventKeyRelease;
typedef SampleEventKey SampleEventKeyPressed;

typedef union {
    SampleEventType       type;

    SampleEventKeyPress   keyPress;
    SampleEventKeyRelease keyRelease;
    SampleEventKeyPressed keyPressed;
} SampleEvent;

bool DisplayInit(SampleDisplay *display) {
    display->display = XOpenDisplay(NULL);
    if (display->display == NULL) {
        fprintf(stderr, "Cannot open display\n");

        return false;
    }

    display->windowsCount = 0;

    return true;
}

SampleWindow *DisplayCreateWindow(SampleDisplay *display, int screen) {
    SampleWindow *window = &display->windows[display->windowsCount];

    (display->windowsCount)++;

    window->window = XCreateSimpleWindow(display->display, RootWindow(display->display, screen),
     WINDOW_X, WINDOW_Y, WINDOW_W, WINDOW_H, WINDOW_BORDER_WIDTH,
     BlackPixel(display->display, screen), WhitePixel(display->display, screen));

    window->screen = screen;

    window->delWindow = XInternAtom(display->display, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(display->display, window->window, &window->delWindow, 1);

    XSelectInput(display->display, window->window, ExposureMask | KeyPressMask);

    XMapWindow(display->display, window->window);

    return window;
}

void DisplayDestroyWindow(const SampleDisplay *display, const SampleWindow *window) {
    XDestroyWindow(display->display, window->window);
}

void DisplayQuit(const SampleDisplay *display) {
    XCloseDisplay(display->display);
}

size_t DisplayGetScreensCount(const SampleDisplay *display) {
    return XScreenCount(display->display);
}

size_t DisplayGetEventsCount(const SampleDisplay *display) {
    return XPending(display->display);
}

void DisplayPopEvent(const SampleDisplay *display, SampleEvent *event) {
    XEvent xEvent;

    XNextEvent(display->display, &xEvent);

    switch(xEvent.type) {
        case KeyPress:
            event->type = SET_KEY_PRESS;
            event->keyPress.type = SET_KEY_PRESS;
            // event->keyPress->window =
            event->keyPress.time = xEvent.xkey.time;
            event->keyPress.key = xEvent.xkey.keycode;
            break;
        case KeyRelease:
            event->type = SET_KEY_RELEASE;
            event->keyPress.type = SET_KEY_RELEASE;
            // event->keyPress->window =
            event->keyPress.time = xEvent.xkey.time;
            event->keyPress.key = xEvent.xkey.keycode;
            break;
        case Expose:
            break;
        default:
            event->type = SET_UNKNOWN;
            break;
    }
}

int main(int argc, char **argv) {
    SampleDisplay  display;
    SampleWindow *window;

    DisplayInit(&display);
    window = DisplayCreateWindow(&display, 0);

    while (1) {
        XNextEvent(display.display, &window->event);

        switch (window->event.type) {
            case KeyPress:
                /* FALLTHROUGH */
                // if (window->event == KeyPress) {
                //     window->event->keycode
                //     XLookupKeysym(window->event, index);
                // }
                printf("%u\n", window->event.xkey.keycode);
            case ClientMessage:
                goto breakout;
            case Expose:
                /* draw the window */
                ; // XFillRectangle(display.display, window->window, DefaultGC(display.display, window->screen), 20, 20, 10, 10);
            /* NO DEFAULT */
        }
    }
breakout:
    DisplayDestroyWindow(&display, window);
    DisplayQuit(&display);

    return 0;
}
