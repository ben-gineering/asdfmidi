#include "keyboard_x11.hpp"

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/Xatom.h>

struct KeyboardX11::Impl {
    Display *display = nullptr;
    Window window = 0;
    bool shutdownRequested = false;
    Atom wmDeleteWindow = 0;
};

KeyboardX11::KeyboardX11() : impl_(new Impl) {}

KeyboardX11::~KeyboardX11() {
    shutdown();
    delete impl_;
}

bool KeyboardX11::init(const char *displayName) {
    impl_->display = XOpenDisplay(displayName);
    if (!impl_->display) {
        return false;
    }

    // Ask X to send only a stream of KeyPress events for auto-repeat,
    // with a single KeyRelease when the key is actually released.
    // This avoids repeated Note On/Off while holding a key.
    int detectableSupported = 0;
    XkbSetDetectableAutoRepeat(impl_->display, True, &detectableSupported);

    int screen = DefaultScreen(impl_->display);
    Window root = RootWindow(impl_->display, screen);

    impl_->window = XCreateSimpleWindow(
        impl_->display,
        root,
        0, 0,
        200, 100,
        0,
        BlackPixel(impl_->display, screen),
        BlackPixel(impl_->display, screen));

    XStoreName(impl_->display, impl_->window, "asdfmidi keyboard");

    // Set up WM_DELETE_WINDOW protocol for graceful shutdown
    impl_->wmDeleteWindow = XInternAtom(impl_->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(impl_->display, impl_->window, &impl_->wmDeleteWindow, 1);

    XSelectInput(impl_->display, impl_->window, KeyPressMask | KeyReleaseMask | StructureNotifyMask);
    XMapWindow(impl_->display, impl_->window);

    XFlush(impl_->display);

    return true;
}

bool KeyboardX11::nextEvent(KeyEvent &ev) {
    if (!impl_->display) {
        return false;
    }

    XEvent xev;
    XNextEvent(impl_->display, &xev);

    // Handle window close request
    if (xev.type == ClientMessage) {
        XClientMessageEvent *cme = reinterpret_cast<XClientMessageEvent *>(&xev);
        if (static_cast<Atom>(cme->data.l[0]) == impl_->wmDeleteWindow) {
            impl_->shutdownRequested = true;
            return false;
        }
    }

    // Handle window destroy
    if (xev.type == DestroyNotify) {
        impl_->shutdownRequested = true;
        return false;
    }

    if (xev.type == KeyPress || xev.type == KeyRelease) {
        XKeyEvent *kev = reinterpret_cast<XKeyEvent *>(&xev);
        KeySym sym = XLookupKeysym(kev, 0);

        ev.keySym = static_cast<unsigned long>(sym);
        ev.type = (xev.type == KeyPress) ? KeyEventType::Press : KeyEventType::Release;
        return true;
    }

    return true;
}

void KeyboardX11::shutdown() {
    if (impl_->display) {
        if (impl_->window) {
            XDestroyWindow(impl_->display, impl_->window);
            impl_->window = 0;
        }
        XCloseDisplay(impl_->display);
        impl_->display = nullptr;
    }
}

void KeyboardX11::requestShutdown() {
    impl_->shutdownRequested = true;
}

bool KeyboardX11::shouldShutdown() const {
    return impl_->shutdownRequested;
}
