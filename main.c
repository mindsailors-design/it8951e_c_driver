#include <pipewire/pipewire.h>
#include <spa/param/video/format-utils.h>

#include "it8951.h"

void on_stream_process(void *data)
{
    struct pw_stream *stream = data;
    struct pw_buffer *buffer;
    
    if ((buffer = pw_stream_dequeue_buffer(stream)) == NULL)
    {
        return;
    }
    
    struct spa_buffer *spa_buf = buffer->buffer;
    if (spa_buf->datas[0].data == NULL)
    {
        pw_stream_queue_buffer(stream, buffer);
        return;
    }
    
    uint8_t *frame_data = spa_buf->datas[0].data;
    size_t frame_size = spa_buf->datas[0].chunk->size;

    printf("Frame size: %d\n", frame_size);
    printf("Sending frame to e-paper display\n");

    pw_stream_queue_buffer(stream, buffer);
    
}
int main(int argc, char *argv[]) {
    printf("Screen grabbing demo with PipeWire\n");
    struct pw_main_loop *loop;
    struct pw_stream *stream;

    pw_init(&argc, &argv);

    loop = pw_main_loop_new(NULL);
    stream = pw_stream_new_simple(
        pw_main_loop_get_loop(loop),
        "screen-stream",
        pw_properties_new(
            PW_KEY_MEDIA_TYPE, "Video",
            PW_KEY_MEDIA_CATEGORY, "Capture",
            PW_KEY_MEDIA_ROLE, "Screen",
            NULL),
        &(struct pw_stream_events) {
            PW_VERSION_STREAM_EVENTS,
            .process = on_stream_process
        },
        stream);
    

    struct spa_pod_builder b = {0};
    uint8_t buffer[1024];
    spa_pod_builder_init(&b, buffer, sizeof(buffer));

    struct spa_rectangle size = SPA_RECTANGLE(1920, 1080);
    struct spa_fraction framerate = SPA_FRACTION(60, 1);


    const struct spa_pod *params[1];
    params[0] = spa_pod_builder_add_object(
        &b,
        SPA_TYPE_OBJECT_Format,     SPA_PARAM_EnumFormat,
        SPA_FORMAT_mediaType,       SPA_POD_Id(SPA_MEDIA_TYPE_video),
        SPA_FORMAT_mediaSubtype,    SPA_POD_Id(SPA_MEDIA_SUBTYPE_raw),
        SPA_FORMAT_VIDEO_format,    SPA_POD_Id(SPA_VIDEO_FORMAT_RGBA),
        SPA_FORMAT_VIDEO_size,      SPA_POD_Rectangle(&size),
        SPA_FORMAT_VIDEO_framerate, SPA_POD_Fraction(&framerate));

    pw_stream_connect(
        stream, 
        PW_DIRECTION_INPUT, 
        PW_ID_ANY,
        PW_STREAM_FLAG_DRIVER | PW_STREAM_FLAG_ALLOC_BUFFERS,
        params, 1);

    pw_main_loop_run(loop);

    pw_stream_destroy(stream);
    pw_main_loop_destroy(loop);
    pw_deinit();
 
    return 0;
}