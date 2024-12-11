#include <pipewire/pipewire.h>
// #include <spa/param/video/format-utils.h>

#include "it8951.h"

int main(int argc, char *argv[]) {
    printf("RaspberryPi SPI test\n");

    pw_init(&argc, &argv);

    fprintf(stdout, "Compiled with libpipewire %s\n" 
                    "Linked with libpipewire %s\n",
                    pw_get_headers_version(),
                    pw_get_library_version());
    
 
    return 0;
}