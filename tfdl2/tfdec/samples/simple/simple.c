#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include "libtfdec.h"

static int fill_buffer(void *buffer, unsigned *size)
{
    char *out_file = "test.h264";

    FILE *fp = fopen(out_file, "rb");

    if (NULL == fp)
    {
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    *size = ftell(fp);

    fseek(fp, 0, SEEK_SET);
    fread(buffer, 1, *size, fp);
    fclose(fp);

    return 1;
}

void decoder_callback_func(TFDEC_HANDLE session, void * data, int size, unsigned long timestamp, unsigned int flag, void* pdata)
{
    static int times = 0;
    times++;

    /* Return the data back to the decoder. */
    tfdec_return_output(session, data);

    printf("-----Callback get data %p size %d time %d timestamp 0x%lx flag %d--\n", data, size, times, timestamp, flag);
}


void main()
{
    void * buffer = malloc(1024 * 1024 * 10);
    char **dev_list;
    int dev_num;
    int size, i;
    TFDEC_HANDLE pdata;
    TFDEC_HANDLE hsession;

    unsigned long ts   = 0xffffffff00000000;
    unsigned int  flag = TFDEC_BUFFER_FLAG_ENDOFFRAME;

    tfdec_version sdk, kernel;
    tfdec_query_version(&sdk, &kernel);
    printf("get sdk version %d.%d.%d kernel version %d.%d.%d\n",
        sdk.major, sdk.minor, sdk.build, kernel.major, kernel.minor, kernel.build);

    dev_list = tfdec_enum_devices(&dev_num);
    printf("Got device number %d\n", dev_num);
    if (dev_num != 0)
    {
        for (i = 0; i < dev_num; i++)
        {
            printf("device %d name %s\n", i, dev_list[i]);
        }
    }
    else
    {
        return;
    }
  

    hsession = tfdec_create(dev_list[0], DECODER_H264, 1920, 1080, 10, decoder_callback_func, &pdata);
    if(!hsession)
    {
         printf("-------------Can't create session on dev %s------------\n", dev_list[0]);
         return;
    }

    pdata = hsession;

    fill_buffer(buffer, &size);

    int frame_count = 10;

    for (i = 0; i < frame_count; i++)
    {
        /* If can't enqueue buffer, decoder maybe hang, or too much buffers are in the queue. */
        do
        {
            int ret;
            ret = tfdec_enqueue_buffer(hsession, buffer, size, ts + i, flag);
            if (ret)
            {
                break;
            }
            else
            {
                usleep(1000);
            }
        }while(1);
        printf("------enqueued buffer %d size %d----\n", i, size);
    }

    printf("Please input anykey to exit ... ...\n");
    getchar();

    tfdec_destroy(hsession);
    free(dev_list);
}
