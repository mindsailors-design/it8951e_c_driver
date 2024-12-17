#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// callback type definition
typedef void (*FrameCallback)(int frame_number);

int captured_frames = 0;

// function that prints a message when a new frame arrives
void on_new_frame(int frame_number)
{
        // printf("New frame arrived: Frame %d\n", frame_number);
        captured_frames++;
}

// function to capture frames and invoke a callback
void capture_frame(Display *display, Window root, int width, int height, FrameCallback callback)
{
        for (int frame = 0; frame < 100; frame++)
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
        Window root = RootWindow(display, screen);
        printf("Screen size: %dx%d\n", width, height);

        int major, minor, pixmap_format;
        Bool shared_pixmap;
        if (!XShmQueryVersion(display, &major, &minor, &shared_pixmap))
        {
            fprintf(stderr, "XShm extension not supported\n");
            XCloseDisplay(display);
            return 1;
        }

        printf("XShm version: %d.%d, Shared Pixmaps: %s\n",
                major, minor, shared_pixmap ? "Yes" : "No");

        // Create shared memory segment
        XShmSegmentInfo shminfo;
        XImage *image = XShmCreateImage(display, DefaultVisual(display, screen),
                                        DefaultDepth(display, screen),
                                        ZPixmap, NULL, &shminfo, 1448, 1072);

        if (!image)
        {
                fprintf(stderr, "Failed to create XShmImage\n");
                XCloseDisplay(display);
                return 1;
        }

        shminfo.shmid = shmget(IPC_PRIVATE, image->bytes_per_line * image->height, IPC_CREAT | 0777); // shmget(key, )
        if (shminfo.shmid == -1)
        {
            fprintf(stderr, "Failed to allocate shared memory\n");
            XDestroyImage(image);
            XCloseDisplay(display);
            return 1;
        }

        shminfo.shmaddr = shmat(shminfo.shmid, NULL, 0);
        image->data = shminfo.shmaddr;
        shminfo.readOnly = False;

        // Attach shared memory to the X server
        if (!XShmAttach(display, &shminfo))
        {
            fprintf(stderr, "Failed to attach shared memory to X server\n");
            shmdt(shminfo.shmaddr);
            shmctl(shminfo.shmid, IPC_RMID, NULL);
            XDestroyImage(image);
            XCloseDisplay(display);
            return 1;
        }

        printf("Capturing frames using XShmGetImage...\n");

        // Capture frames
        for (int frame = 0; frame < 30; frame++)
        {
            if (!XShmGetImage(display, root, image, 0, 0,AllPlanes))
            {
                fprintf(stderr, "Failed to get image using XShmGetImage\n");
                break;
            }

            // Simulate processing of the captured frame
        //     printf("Captured frame %d\n", frame);

            captured_frames++;

        //     usleep(33333);
        }

        XShmDetach(display, &shminfo);
        shmdt(shminfo.shmaddr);
        shmctl(shminfo.shmid, IPC_RMID, NULL);
        XDestroyImage(image);
        XCloseDisplay(display);
        
        printf("Captured %d frames\n", captured_frames);

        return 0;
}