#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "tfenc_api.h"

volatile int encoding_ended = 1;

int get_buffer(FILE *fp, int width, int height, uint8_t *buffer)
{
    size_t ret;
    int size;
    uint8_t *temp;
    uint8_t *s, *d;
    uint8_t *u, *v;
    int i, j;
    if (!fp)
        return 1;

    size = width * height * 3 / 2;
    temp = (uint8_t *)malloc(size);

    
    ret = fread(temp, 1, size, fp);
    if (ret < size)
        goto err;

    s = temp;
    d = buffer;
    memcpy(d, s, width * height);

    d = buffer + width * height;
    u = temp + width * height;
    v = u + width * height / 4;

    for (i = 0; i < height; i += 2)
    {
        for (j = 0; j < width; j += 2)
        {
            *d++ = *u++;
            *d++ = *v++;
        }
    }

    free(temp);
    return 1;
err:
    free(temp);
    return 0;
}

void end_encoding(void * param, void* data, int len)
{
    FILE *fp;
    char *name = "mytest.265";
    printf("%s : data %p len %d\n", __func__, data, len);

    if (len != 0)
    {
#if 0
        fp = fopen(name, "ab");
        if (fp != NULL)
        {
            fwrite(data, 1, len, fp);
            fclose(fp);
        }
#endif
    }
    else
    {
       encoding_ended = 1;
    }
};

int main(int arvc, void** argv)
{
   int ret;
   TF_HANDLE henc;
   unsigned char * filename;
   FILE *fp;
   size_t size;
   tfenc_callback callback;


   /* encoding setting. */
   tfenc_setting setting;
   setting.pix_format = PIXFMT_NV12;
   setting.width = 1920;
   setting.height = 1080;
   setting.profile = PROFILE_HEVC_MAIN;
   setting.level = 51;
   setting.bit_rate = 4000000;
   setting.max_bit_rate = 4000000;
   setting.gop = 50;
   setting.frame_rate = 25;

   setting.device_id = 0;
   setting.rc_mode   = RC_CBR;

   filename = argv[1];

   /* callback parameter. */
   callback.param = NULL;
   callback.func = end_encoding;
  
   /* create an encoder instance. */
   ret = tfenc_encoder_create(&henc, &setting, callback);
   if (ret < 0)
   {
       printf("create encoder failed!\n");
       return -1;
   }

   uint8_t * frame;
   fp = fopen(filename, "rb");
   if (!fp)
   {
       printf("open file %s fail!\n", filename);
   }

   size = 1920 * 1080 * 3 / 2;
   frame = (uint8_t*)malloc(size);

   encoding_ended = 0;
   while(1)
   {
       /* Load source frame from fd, and convert to NV12 format if need. */
       if (get_buffer(fp, 1920, 1080, frame))
       {
            /* Send source frame to the encoder. */
            ret = tfenc_process_frame(henc, frame, size);
            if (ret < 0)
            {
                printf("call tfecn_process_frame failed!\n");
            }
       }
       else
       {
           break;
       }
   }

   /* Send EOS(end of frame) to the encoder. */
   tfenc_process_frame(henc, 0, 0);

   while (!encoding_ended)
   {
      usleep(1000);
   }

   printf("begin to destroy decoder %p\n", henc);
   tfenc_encoder_destroy(henc);

   printf("test done! exit!\n");
}
