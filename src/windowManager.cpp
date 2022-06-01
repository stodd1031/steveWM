#include "windowManager.h"

WindowManager::WindowManager()
{
    display = XOpenDisplay(nullptr);
    root = XDefaultRootWindow(display);
    XGrabKey(display, XKeysymToKeycode(display, XK_Escape), Mod1Mask, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(display, XKeysymToKeycode(display, XK_1), Mod1Mask, root, True, GrabModeAsync, GrabModeAsync);
    XGrabKey(display, XKeysymToKeycode(display, XK_Tab), Mod1Mask, root, True, GrabModeAsync, GrabModeAsync);
    //XGrabKey(display, XKeysymToKeycode(display, XK_Alt_L), NoEventMask, root, True, GrabModeAsync, GrabModeAsync);
    Cursor cursor = XCreateFontCursor(display, XC_left_ptr);
    XDefineCursor(display, root, cursor);

    XSelectInput(display, root,
                 SubstructureRedirectMask |
                     SubstructureNotifyMask);

    xMonitors = XRRGetMonitors(display, root, true, &monitorCnt);
}

void WindowManager::handleKeyPress(XKeyEvent *ev)
{

    switch (XkbKeycodeToKeysym(display, ev->keycode, 0, 0)) // switch event keysym
    {
    case XK_Escape: //case alt + escape close window manager
        XCloseDisplay(display);
        break;
    case XK_1: /* case alt + 1 open xterm */
        if (!fork())
        {
            //system("xterm");
            execlp("xterm", "xterm", (char *)NULL);
        }
        break;
    case XK_F4: /* case alt + 1 open xterm */
        if (ev->state & Mod1Mask)
        {
            XEvent event;
            event.xclient.type = ClientMessage;
            event.xclient.window = frameHandlesTOFrame[ev->window].getClientHandle();
            event.xclient.message_type = XInternAtom(display, "WM_PROTOCOLS", True);
            event.xclient.format = 32;
            event.xclient.data.l[0] = XInternAtom(display, "WM_DELETE_WINDOW", False);
            event.xclient.data.l[1] = CurrentTime;
            int result = XSendEvent(display, frameHandlesTOFrame[ev->window].getClientHandle(), False, NoEventMask, &event);
        }
        break;
    case XK_Tab:
        switchFrame += 1;
        if (switchFrame >= applications.size())
        {
            switchFrame = 0;
        }
        const Window application = applications[switchFrame]; 
        XRaiseWindow(display, application);
        XSetInputFocus(display, frameHandlesTOFrame[application].getClientHandle(), RevertToPointerRoot, CurrentTime);
        break;
    }
}

void WindowManager::handleKeyRelease(XKeyEvent *ev)
{
    // switch (XkbKeycodeToKeysym(display, ev->keycode, 0, 0))
    // {
    // case XK_Alt_L:
    //     std::cout << "alt" << std::endl;
    //     XUngrabKey(display, XKeysymToKeycode(display, XK_Alt_L), NoEventMask, root);
    //     //XSync(display, 1);
    //     break;
    // case XK_Tab:
    //     XGrabKey(display, XKeysymToKeycode(display, XK_Alt_L), NoEventMask, root, True, GrabModeAsync, GrabModeAsync);
    //     break;
    // }
}

void WindowManager::handleConfigureRequest(XConfigureRequestEvent *ev)
{
    XWindowChanges changes;
    changes.x = 0;
    changes.y = 25;
    changes.width = ev->width;
    changes.height = ev->height;
    changes.border_width = ev->border_width;
    changes.stack_mode = ev->detail;
    changes.sibling = ev->above;
    XConfigureWindow(display, ev->window, ev->value_mask, &changes);
    if (frameHandlesTOFrame.count(ev->parent)) //If parent is a frame
    {
        //Move frame to where client wanted to be
        frameHandlesTOFrame[ev->parent].moveResizeFrame(
            ev->x,
            ev->y,
            ev->width,
            (ev->height + 25));
    }
}

void WindowManager::handleMapRequest(XMapRequestEvent *ev)
{
    //Frame the client
    Frame frame = Frame(display, root, ev->window);

    //Add frame to array and dictionary
    frameHandlesTOFrame[frame.getFrameHandle()] = frame;
    applications.push_back(frame.getFrameHandle());

    //Map the client
    XMapWindow(display, ev->window);
}

void WindowManager::handleButtonPress(XButtonEvent *ev)
{
    switch (ev->button)
    {
    case Button1:
        if (ev->state & Mod1Mask) // If alt button is being pressed get window and curs info for window transformations
        {
            //Temp variables
            Window root_return;
            unsigned int border_width_return;
            unsigned int depth_return;
            //Get location and size of frame
            XGetGeometry(display, ev->window, &root_return,
                         &winIntX, &winIntY,
                         &winIntWidth, &winIntHeight,
                         &border_width_return,
                         &depth_return);
            //Get curs pos
            cursIntX = ev->x_root;
            cursIntY = ev->y_root;
        }
        if (frameHandlesTOFrame.count(ev->window))
        {
            //Bring frame to front, and client gets input
            XRaiseWindow(display, ev->window);
            XSetInputFocus(display, frameHandlesTOFrame[ev->window].getClientHandle(), RevertToPointerRoot, CurrentTime);
            // Sync for mouse grab, grab button in frame(Window client)
            XAllowEvents(display, ReplayPointer, ev->time);
            XSync(display, 0);

            active = ev->window;

            frameHandlesTOFrame[ev->window].handleFrameButtonPress(ev);
        }
        else
        {
            Window root2;
            Window parent;
            Window *children;
            unsigned int nchildren;
            XQueryTree(display, ev->window, &root2, &parent, &children, &nchildren);
            if (frameHandlesTOFrame.count(parent))
            {
                //Temp variables
                Window root_return;
                unsigned int border_width_return;
                unsigned int depth_return;
                //Get location and size of frame
                XGetGeometry(display, parent, &root_return,
                             &winIntX, &winIntY,
                             &winIntWidth, &winIntHeight,
                             &border_width_return,
                             &depth_return);
                //Get curs pos
                cursIntX = ev->x_root;
                cursIntY = ev->y_root;
            }
        }
        break;

    case Button3:
    {
        //Temp variables
        Window root_return;
        unsigned int border_width_return;
        unsigned int depth_return;
        //Get location and size of frame
        XGetGeometry(display, ev->window, &root_return,
                     &winIntX, &winIntY,
                     &winIntWidth, &winIntHeight,
                     &border_width_return,
                     &depth_return);
        //Get curs pos
        cursIntX = ev->x_root;
        cursIntY = ev->y_root;
        //Set corner to 0 to reevaluate which corner was currently pressed
        corner = 0;
        if (ev->x > winIntWidth / 2) //If curs x in window is greater than half window width add 1
        {
            corner += 1;
        }
        if (ev->y > winIntHeight / 2) //If curs y in window is greater than half window height add 2
        {
            corner += 2;
        }
    }
    break;
    }
}

void WindowManager::handleMotionNotify(XMotionEvent *ev)
{
    switch (ev->state)
    {
    case Button1Mod1Mask: // Case alt and left mouse button are/have been pressed
        frameHandlesTOFrame[ev->window].handleWindowDrag(
            ev,
            cursIntX,
            cursIntY,
            winIntX,
            winIntY);
        break;
    case Button1MotionMask:
        frameHandlesTOFrame[active].handleWindowDragOrResize(
            ev,
            cursIntX,
            cursIntY,
            winIntX,
            winIntY,
            winIntWidth,
            winIntHeight);
        break;

    case Button3Mod1Mask:
    {
        frameHandlesTOFrame[ev->window].handleWindowResize(
            ev,
            cursIntX,
            cursIntY,
            winIntX,
            winIntY,
            winIntWidth,
            winIntHeight,
            corner);
    }
    break;
    }
}

void WindowManager::handleUnmapNotify(XUnmapEvent *ev)
{
    if (!frameHandlesTOFrame.count(ev->event))
    {
        return;
    }
    if (ev->window == root)
    {
        return;
    }
    frameHandlesTOFrame.erase(ev->event);
    applications.erase(std::find(applications.begin(), applications.end(), ev->event));
    XDestroyWindow(display, ev->event);
}

void WindowManager::loop()
{
    XEvent ev;
    while (true)
    {
        XNextEvent(display, &ev);
        switch (ev.type)
        {
        case KeyPress:
            handleKeyPress(&ev.xkey);
            break;
        case ConfigureRequest:
            handleConfigureRequest(&ev.xconfigurerequest);
            break;
        case MapRequest:
            handleMapRequest(&ev.xmaprequest);
            break;
        case ButtonPress:
            handleButtonPress(&ev.xbutton);
            break;
        case KeyRelease:
            handleKeyRelease(&ev.xkey);
            break;
        case MotionNotify:
            handleMotionNotify(&ev.xmotion);
            break;
        case UnmapNotify:
            handleUnmapNotify(&ev.xunmap);
            break;
        }
    }
}