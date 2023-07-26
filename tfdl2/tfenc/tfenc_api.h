#ifndef TFENC_API_HEAD
#define TFENC_API_HEAD
#include <stdint.h>

#define TFENC_SUCCESS             0
#define ERROR_INVALID_PARAM      -1
#define ERROR_UNKNOWN_RPC        -2
#define ERROR_NO_MEMORY          -3
#define ERROR_STREAM_OVERFLOW    -4
#define ERROR_TOO_MANY_SLICES    -5
#define ERROR_NO_CHANNEL_AVAILABLE   -6
#define ERROR_RESOURCE_UNAVAILABLE   -7
#define ERROR_NOT_ENOUGH_CORES       -8
#define ERROR_REQUEST_MALFORMED      -9
#define ERROR_CMD_NOT_ALLOWED        -10
#define ERROR_INVALID_CMD_VALUE      -11
#define ERROR_INTERMEDIATE_BUFFER_OVERFLOW  -12

#define ERROR_SEND_TO_SERVER         -13
#define ERROR_RECEIVE_FROM_SERVER    -14
#define ERROR_FAIL_TO_CONNECT        -15

#define TFENC_ERROR(x)  ((x) != TFENC_SUCCESS)

typedef void* TF_HANDLE;

typedef enum tf_e_profile
{
    PROFILE_AVC_BASELINE = 0,
    PROFILE_AVC_MAIN,
    PROFILE_AVC_HIGH,
    PROFILE_HEVC_MAIN,
    PROFILE_HEVC_MAIN10,

    PROFILE_JPEG,

    TF_PROFILE_INVALID,
}tf_profile;


/* Picture format. */
typedef enum
{
    PIXFMT_NV12,
    PIXFMT_NV12_10B,

    PIXFMT_INVALID,
}tf_pixfmt;

/* rate control mode. */
typedef enum
{
    RC_CBR = 0,
    RC_VBR,

    RC_INVALID,
}tf_rcmode;

typedef struct tfenc_setting_t
{
    /* input */
    tf_pixfmt  pix_format;
    uint32_t   width;
    uint32_t   height;

    tf_profile profile;
    uint32_t   level;

    uint32_t   bit_rate;
    uint32_t   max_bit_rate;
    uint32_t   gop;
    uint32_t   frame_rate;

    uint32_t   device_id;
    tf_rcmode  rc_mode;
}tfenc_setting;

typedef struct
{
    /* Callback function, called by encoder when ended frame encoding. */
    /*
       params:
         user_param: User passed paramter.
         data:       Encoded data.
         len:        Data lenght, means end of stream if 0.
    */ 
    void (*func)(void *user_param, void *data, int len);

    /* Parameter will be passed as user_param of callback function. */
    void * param;
}tfenc_callback;


#ifdef __cplusplus
extern "C" {
#endif

/*
  tfenc_encoder_create : create a encoder instance.

  params:
    hEnc     : Out paramter, handle of encoder.
    setting  : The setting of the encoder, please see the tfenc_setting struct.
    callback : Callback paramter, the func will be called when frame encoding end.

  return:
    TFENC_SUCCESS if success.
*/
int tfenc_encoder_create(TF_HANDLE *hEnc, tfenc_setting *settings, tfenc_callback callback);

/*
  tfenc_encoder_destroy : destroy an encoder instance.

  params:
    hEnc : The handle of the encoder. 

  return:
    TFENC_SUCCESS if success.
*/
int tfenc_encoder_destroy(TF_HANDLE hEnc);

/*
  tfenc_process_frame : Requst encoder to process one source frame.

  param:
    hEnc   : The handle of the encoder. 
    buffer : The pointer of source frame.
    size   : Buffer size, should be one frame size.
*/
int tfenc_process_frame(TF_HANDLE henc, uint8_t *buffer, size_t size);


int tfenc_restart_GOP(TF_HANDLE henc);

/*
  tfenc_query_device_num : query how many encoder devices.

  return:
    the number of the encoder.
*/
int tfenc_query_device_num();
#ifdef __cplusplus
}
#endif
#endif
