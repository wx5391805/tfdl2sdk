#ifndef LIB_TFDEC_HEADER_H
#define LIB_TFDEC_HEADER_H

#define TFDEC_BUFFER_FLAG_ENDOFFRAME         0
#define TFDEC_BUFFER_FLAG_EOS                1

#define TFDEC_STATUS_SUCCESS                 1
#define TFDEC_STATUS_QUEUE_IS_FULL           0
#define TFDEC_STATUS_INVALID_ARG            -1
#define TFDEC_STATUS_INTERNAL_ERROR         -2


#define TFDEC_ERROR_INVALID_TAG             (-100)
#define TFDEC_ERROR_EMPTY_TAG_QUEUE         (-101)
#define TFDEC_ERROR_TAG_INTERNAL            (-102)
#define TFDEC_ERROR_CAN_NOT_DECODE          (-103)
#define TFDEC_ERROR_NO_ENOUGH_MEMORY        (-104)
#define TFDEC_ERROR_NOT_SUPPORTED_FORMAT    (-105)
#define TFDEC_ERROR_TIMEOUT                 (-106)

typedef enum TFDEC_DECODER_ROLE
{
    DECODER_H264,
    DECODER_HEVC,
    DECODER_VP8,
    DECODER_JPEG,
    DECODER_MPG2,
    DECODER_MPG4,
    DECODER_AVC,
    DECODER_H263
}TFDEC_DECODER_ROLE;


typedef void* TFDEC_HANDLE;

typedef struct
{
    int32_t major;
    int32_t minor;
    int32_t build;
}tfdec_version;

#ifdef __cplusplus
extern "C" {
#endif


/**
  * Callback function prototype. The callback funtion pointer should be pass to decoder lib when create a decode session.
    And the function will be called by decoder lib when a output buffer is available.

  @ param
    - void *buffer             : a output buffer pointer .
    - int  size                : buffer size
    - unsigned long  timestamp : timestamp appending to the output buffer.
    - unsigned int   flag      : buffer flag, indicate if end of stream.
                                 TFDEC_BUFFER_FLAG_ENDOFFRAME : this buffer is an end of frame
                                 TFDEC_BUFFER_FLAG_EOS        : this buffer is an end of stream

    - void * user_data         : pointer to user_data, which is passed in when creating a decode session.
 */
typedef void (*tfdec_callback_func)(TFDEC_HANDLE session, void * buffer, int size, unsigned long timestamp, unsigned int flag, void * user_data);


/**
  * Enum the decoder devices.
  @ param
    - int *dev_num             : output param, the number of available decoders.

 * @return the devices' name list, which can be used as the parameter of the tfdec_create() function.
    user application should call free() to free the memory if dev_num > 0.
 */
char **tfdec_enum_devices(int *dev_num);

/**
 * Opens the device file and returns a file descriptor.
 * @param
    - const char * dev_name     : Full path of the device file (e.g. "/dev/mv500")
    - TFDEC_DECODER_ROLE role   : what kind of session
    - int width                 : output buffer width
    - int height                : outpub buffer height
    - int out_buffer_num        : output buffer used by session, recommand value is 3 ~ 5. . Max is 20
    - func * pcallback          : call back function when a decoded output buffer is available
    - void *user_data           : user_data will be pass back when call pcallback

 * @return File descriptor that shall be used in the call to the other library functions.
 */
TFDEC_HANDLE  tfdec_create(const char *dev_name, TFDEC_DECODER_ROLE role, int width, int height, int out_buffer_num, tfdec_callback_func pcallback, void* user_data);

/**
 * Opens the device file with less memory usage and returns a file descriptor.
 * @param
    - const char * dev_name     : Full path of the device file (e.g. "/dev/mv500")
    - TFDEC_DECODER_ROLE role   : what kind of session
    - int width                 : output buffer width
    - int height                : outpub buffer height
    - int out_buffer_num        : output buffer used by session, recommand value is 3 ~ 5. . Max is 20
    - func * pcallback          : call back function when a decoded output buffer is available
    - void *user_data           : user_data will be pass back when call pcallback

 * @return File descriptor that shall be used in the call to the other library functions.
 */
TFDEC_HANDLE  tfdec_create_lite(const char *dev_name, TFDEC_DECODER_ROLE role, int width, int height, int out_buffer_num, tfdec_callback_func pcallback, void* user_data);


/**
  * Destroy a decode session
  @ param
    -TFDEC_HANDLE session : handle of a decoding session
*/
void tfdec_destroy(TFDEC_HANDLE session);


/**
  * Internal used
*/
TFDEC_HANDLE  tfdec_create_ex(const char *dev_name,  TFDEC_DECODER_ROLE role, int width, int height, int out_buffer_num, tfdec_callback_func pcallback, void* user_data, int use_shadow_output);


/**
  * Enqueue video bitstream to the decoder
  @param
    - TFDEC_HANDLE session    : handle of a decoding session.
    - void* buffer            : bit stream buffer for decoding.
    - int   size              : size of input buffer.
    - unsigned long timestamp : input buffer timestamp
    - unsigned int flag       : buffer flags, will returned with output buffer from callback function.
                                TFDEC_BUFFER_FLAG_ENDOFFRAME : this buffer is an end of frame
                                TFDEC_BUFFER_FLAG_EOS        : this buffer is an end of stream

  @return : return TFDEC_STATUS_SUCCESS        if success.
                   TFDEC_STATUS_QUEUE_IS_FULL  if decode queue is full
                   TFDEC_STATUS_INVALID_ARG    if arguments is invalid
                   TFDEC_STATUS_INTERNAL_ERROR if encount a internal error.
*/

int tfdec_enqueue_buffer(TFDEC_HANDLE session, void *buffer, int size, unsigned long timestamp, unsigned int flag);

/**
  * Ruturn back the an output buffer, which got by callback.
    Application must call this funtion to return a output buffer when no longer needed,
    otherwise, the decoder will be blocked due to no output buffers.

  @param
    - TFDEC_HANDLE session : handle of a decoding session.
    - void* buffer         : output buffer got by callback function.

  @return : 1 if success, or 0 if failed.
*/
int tfdec_return_output(TFDEC_HANDLE session, void * buffer);

/**
  * Query how many input buffer queue available.
  @param
    - TFDEC_HANDLE session : handle of a decoding session.

  @return : number of available input queue.
*/
int tfdec_query_input_queue(TFDEC_HANDLE session);


/**
  * Query the sdk and kernel driver version.
  @param
    - tfdec_vserion *sdk    : Address of sdk version data structure.
    - tfdec_vserion *kernel : Address of kernel driver data structure.

  @return : 1 if success, or 0 if failed.
*/
int tfdec_query_version(tfdec_version *sdk, tfdec_version *kernel);
#ifdef __cplusplus
}
#endif
#endif
