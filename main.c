#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// callback type definition
typedef void (*FrameCallback)(int frame_number);

// function that prints a message when a new frame arrives
void on_new_frame(int frame_number)
{
        printf("New frame arrived: Frame %d\n", frame_number);
}

// function to capture frames and invoke a callback
void capture_frame(Display *display, Window root, int width, int height, FrameCallback callback)
{
        for (int frame = 0; frame < 10; frame++)
        {
                XImage *image = XGetImage(display, root, 0, 0, width, height, AllPlanes, ZPixmap);
                if (!image)
                {
                        fprintf(stderr, "Failed to get XImage\n");
                        continue;
                }

                if (callback)
                {
                        callback(frame);
                }
                XDestroyImage(image);
                // usleep(3333);
        }
}

int main(int argc, char *argv[])
{
         Display *display = XOpenDisplay(NULL);
        if (!display)
        {
                fprintf(stderr, "Failed to open X display\n");
                return 1;
        }

        int screen = DefaultScreen(display);
        int width = DisplayWidth(display, screen);
        int height = DisplayHeight(display, screen);
        printf("Screen size: %dx%d\n", width, height);

        Window root = RootWindow(display, screen);

        printf("Starting frame capture. Press Ctrl-C to stop.\n");
        capture_frame(display, root, width, height, on_new_frame);

        XCloseDisplay(display);
        
        return 0;
}