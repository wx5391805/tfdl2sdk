//
// Created by wx on 12/18/18.
//

#ifndef TFGH_H
#define TFGH_H


#include <stdint.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <cstring>
#include <cmath>
#include <vector>
#include <cassert>
#include <chrono>

#define TFEXPORT __attribute__ ((visibility("default")))

#define MAX_HW_JPEG_Width 1920
#define MAX_HW_JPEG_Height 1200

#define TFG_VERSION 21

// #define USE_TFGS
#ifdef USE_TFGS
#include "EasyTypes.h"
#endif
namespace tfg {
    const int TF_SPLIT = (1 << 10);
    const int TF_YUV = (1 << 11);
    const int I420Pad = 1;
    enum TFSAMP {
        TFSAMP_UNSIGNED = (0),
        TFSAMP_RGB = (1 ),
        TFSAMP_BGR = (2 ),
        TFSAMP_RGB_SPLIT = (1 | TF_SPLIT ),
        TFSAMP_BGR_SPLIT = (2 | TF_SPLIT ),
        TFSAMP_I420Planar = (5 | TF_YUV),
        TFSAMP_Gray = (6 | TF_YUV)
    };
    enum INTERP_MODE{
        INTERP_Bilinear = 0,
        INTERP_NONE,
        INTERP_BOX
    };

    class TFEXPORT JPEGDEC;
    /*
     * Enable a GLOBAL hw decoder,call at the beginning .
     * @ReadJpeg can hardware decode jpeg when w,h is not larger than @maxWidth,@maxHeight.
     * otherwise decoder will cost extra time to auto expand.
     * @maxWidth ,@maxHeight should be less than 1920x1080.
     */
    int TFEXPORT EnableHwJpegDecoder(int maxWidth, int maxHeight);
    int TFEXPORT DisableHwJpegDecoder();

    /*
     * Create a session to Read kinds of img data
     * TFSession IS NOT thread-safe, every thread should create a session
     * */
    class HWAcc;
    class TFSession {
    private:
        void *handle = nullptr;
        uint8_t *yuvBuffer = nullptr;
        uint8_t *rgbaBuffer = nullptr;
        HWAcc   *hwAcc = nullptr;
        int yuvStride[3],yuvHeight[3];
        unsigned long bufferSize[2] = {0,0};
        int mWidth = 0,mHeight = 0,mSize = 0;
        TFSAMP mFormat = TFSAMP_UNSIGNED;

        JPEGDEC *jpegDec;
        TFSession();
        ~TFSession();
        int tfDecompressToYUV(uint8_t *src,int len,uint8_t *dst,int width,int pad,int height);
        friend class RGBImg;

    public:
        static TFEXPORT TFSession *CreateSession();
        static TFEXPORT TFSession *CreateSession(TFSession* from);
        int TFEXPORT ReadJpeg(const char *filename);
        int TFEXPORT ReadPng(const char *filename);
        int TFEXPORT ReadBmp(const char *filename);
        int TFEXPORT ReadImg(const char *filename);
        int TFEXPORT ReadJpeg(uint8_t *input,int len);
        int TFEXPORT ReadPng(uint8_t *input,int len);
        int TFEXPORT ReadBmp(uint8_t *input,int len);
        int TFEXPORT ReadImg(uint8_t *input,int len);

        //planeNum 1 for gray,planeNum 3 for I420Planar
        int TFEXPORT ReadI420_Planar(uint8_t *input,int width,int height,int stride[],int planeNum);
        //samp should be TFSAMP_BGR or TFSAMP_RGB
        int TFEXPORT ReadXGX24(uint8_t *input,int width,int height,TFSAMP samp);
        int TFEXPORT GetImgWidth();
        int TFEXPORT GetImgHeight();
        TFSAMP TFEXPORT GetFormat();
        int TFEXPORT BufferImg(uint8_t *buffer, int width, int height, TFSAMP format);
        void TFEXPORT Destroy();
    };
    int TFEXPORT GetImgBufferSize(int width,int height,TFSAMP format);
    //for rgb
    /**
     * rgb 格式缩放 #强制硬件模式
     * 
     * @param src 输入地址
     * @param srcWidth  原图宽度
     * @param srcHeight 原图高度
     * @param dst 输出地址
     * @param dstWidth  目标图宽度
     * @param dstHeight 目标图高度
     * @param channel 通道数，RGB类格式为3，Gray为1
     * @param split 默认false，指明原图是否是SPILIT格式
     * @param mode 缩放算法
     * @return 0 为成功,其他值表示发生错误
     */
    int TFEXPORT RGB_Scale(uint8_t *src, int srcWidth, int srcHeight, 
                           uint8_t *dst, int dstWidth, int dstHeight, size_t channel,bool split = false,INTERP_MODE mode = INTERP_Bilinear);
    /**
     * 从srcStride x srcHeight的原图中 crop 或者扩展出 dstStride x dstHeight大小区域
     * #自动选择是否硬件加速
     *
     * @param src 原图地址
     * @param srcStride 原图宽度
     * @param srcHeight 原图高度
     * @param left 左起始位置，可以为负，表示扩展
     * @param top  上起始位置，可以为负，表示扩展
     * @param dst 目标地址
     * @param dstStride 目标图宽度，可以超过原图尺寸，表示扩展
     * @param dstHeight 目标图高度，可以超过原图尺寸，表示扩展
     * @param channel 通道数，RGB类格式为3，Gray为1
     * @param dstWidth 默认为0，表示和dstStride相同，否则每行只拷贝原图中dstWidth 个元素
     * @param split 默认false，指明原图是否是SPLIT格式
     */
    int TFEXPORT RGB_Crop(uint8_t *src, int srcStride, int srcHeight, int left, int top, 
                          uint8_t *dst, int dstStride, int dstHeight, size_t channel, uint32_t dstWidth = 0,bool split = false);
    int TFEXPORT RGB_Split(uint8_t *src, uint8_t *dst, int width, int height,int channelAlignment = 0);
    int TFEXPORT RGB_Split(uint8_t *src, float *dst, float *mean, float *std, int width, int height, int channelAlignment = 0);
    int TFEXPORT RGB_Padding(uint8_t *src,int srcWidth,int srcHeight,int left,int top,uint8_t *dst,int dstWidth,int dstHeight,int channel,bool planar = false);
    int TFEXPORT BGRA_ToYUV(uint8_t *src,int width,int height ,uint8_t* dst,int mode);//0 for J420, 1 for NV12, 2 for BGR to I420
    int TFEXPORT YUV_ToBGRA(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_u,
               int src_stride_u,
               const uint8_t* src_v,
               int src_stride_v,
               uint8_t* dst_argb,
               int dst_stride_argb,
               int width,
               int height,int mode = 0);//0 for J420, 1 for 601, 2 for 709
    int TFEXPORT BGRA_ToBGR(const uint8_t* src_argb,
                int src_stride_argb,
                uint8_t* dst_rgb24,
                int dst_stride_rgb24,
                int width,
                int height);
    int TFEXPORT RGB_SaveBmp(const char *filename, uint8_t *src, int width,int height,TFSAMP samp);
    int TFEXPORT BGRA_CompressPng(uint8_t* src,int width,int height,uint8_t*dst,int *pngLen,uint8_t* tempBGR);

    //for yuv
    //deprecated ，用 I420_Planar_CropEx 代替
    int TFEXPORT I420_Planar_Crop(uint8_t *src,int srcWidth,int srcHeight,int left,int top,
                                  uint8_t *dst,int dstWidth,int dstHeight);
    /**
     * 从srcStride[0] x srcHeight的原图中 crop 或者扩展出 dstStride[0] x dstHeight大小区域
     * #自动选择是否硬件加速
     *
     * @param src 输入地址
     * @param srcStride 长度为3的数组，表示Y、U、V三个通道原图每行占用字节数，填null则默认为 {srcWidth,(srcWidth+1)/2,(srcWidth+1)/2}，
     *        U、V通道为0表示只含Y通道,可以超过原图尺寸，表示扩展
     * @param srcWidth  原图宽度
     * @param srcHeight 原图高度
     * @param left 左起始位置，可以为负，表示扩展
     * @param top  上起始位置，可以为负，表示扩展
     * @param dst 输出地址
     * @param dstStride 长度为3的数组，表示Y、U、V三个通道目标图每行占用字节数，填null则默认为 {dstWidth,(dstWidth+1)/2,(dstWidth+1)/2}，
     *        U、V通道为0表示只含Y通道,可以超过原图尺寸，表示扩展
     * @param dstWidth  目标图宽度
     * @param dstHeight 目标图高度
     * @param split 默认false，指明原图是否是SPLIT格式
     */
    int TFEXPORT I420_Planar_CropEx(uint8_t *src,int *srcStride,int srcWidth,int srcHeight,int left,int top,
                                    uint8_t *dst,int *dstStride,int dstWidth,int dstHeight);
    int TFEXPORT I420_Planar_Scale(uint8_t *src,int srcWidth,int srcHeight,
                                   uint8_t *dst,int dstWidth,int dstHeight,INTERP_MODE mode = INTERP_Bilinear);
    int TFEXPORT Test_I420ToNV12(const uint8_t* src_y,
               int src_stride_y,
               const uint8_t* src_u,
               int src_stride_u,
               const uint8_t* src_v,
               int src_stride_v,
               uint8_t* dst_y,
               int dst_stride_y,
               uint8_t* dst_uv,
               int dst_stride_uv,
               int width,
               int height);
    /**
     * yuv I420 planar 格式缩放，排列方式为 yyyyuuvv #强制硬件模式
     * 
     * @param src 输入地址
     * @param srcStride 长度为3的数组，表示Y、U、V三个通道原图每行占用字节数，填null则默认为 {srcWidth,(srcWidth+1)/2,(srcWidth+1)/2}，U、V通道为0表示只含Y通道
     * @param srcWidth  原图宽度
     * @param srcHeight 原图高度
     * @param dst 输出地址
     * @param dstWidth  目标图宽度
     * @param dstHeight 目标图高度
     * @param dstStride 长度为3的数组，表示Y、U、V三个通道目标图每行占用字节数，填null则默认为 {dstWidth,(dstWidth+1)/2,(dstWidth+1)/2}，U、V通道为0表示只含Y通道
     * @param mode 缩放算法
     * @return 0 为成功,其他值表示发生错误
     */
    int TFEXPORT I420_Planar_ScaleEx(uint8_t *src,const int *srcStride,int srcWidth,int srcHeight,
                                     uint8_t *dst,const int *dstStride,int dstWidth,int dstHeight,INTERP_MODE mode = INTERP_Bilinear);
    /**
     * yuv I420 planar 格式转RGB,#强制硬件模式
     * 
     * @param src 输入地址
     * @param srcStride 长度为3的数组，表示Y、U、V三个通道原图每行占用字节数，填null则默认为 {srcWidth,(srcWidth+1)/2,(srcWidth+1)/2}
     * @param width  原图宽度
     * @param height 原图高度
     * @param dst 输出地址
     * @param dstFormat 目标格式，必须为TFSAMP_BGR,TFSAMP_RGB,TFSAMP_BGR_Split,TFSAMP_RGB_Split
     * @return 0 为成功,其他值表示发生错误
     */                                     
    int TFEXPORT I420_Planar_ToRGB(uint8_t *src,const int *srcStride,int width,int height,
                                   uint8_t *dst,TFSAMP dstFormat);
    int TFEXPORT I420_Planar_Crop_Scale(uint8_t *src,int srcWidth,int srcHeight,int left,int top,int cropWidth,int cropHeight, uint8_t *dst,int dstWidth,int dstHeight);
    int TFEXPORT I420_Planar_CompressJpeg(uint8_t *src,int width,int height,uint8_t *dst, unsigned long *jpegSize,int Quality = 70);
    
    //merge operation
    /**
     * yuv I420 planar 将图像短边缩放到shortWidth(长边等比例缩放)，再在中心区域裁剪 dstWidth * dstHeight 的块
     * 
     * @param src 输入地址
     * @param srcStride 长度为3的数组，表示Y、U、V三个通道原图每行占用字节数，填null则默认为 {srcWidth,(srcWidth+1)/2,(srcWidth+1)/2}
     * @param srcWidth  原图宽度
     * @param srcHeight 原图高度
     * @param dst 输出地址
     * @param shortWidth 短边缩放长度
     * @param dstWidth  目标图宽度
     * @param dstHeight 目标图高度
     * @param dstFormat 目标格式，可转为TFSAMP_BGR,TFSAMP_RGB,TFSAMP_BGR_Split,TFSAMP_RGB_Split 或保持 TFSAMP_I420Planar
     * @return 0 为成功,其他值表示发生错误
     */  
    int TFEXPORT I420_Planar_CenterCrop(uint8_t *src,const int *srcStride,int srcWidth,int srcHeight,
                                        uint8_t *dst,int shortWidth,int dstWidth,int dstHeight,TFSAMP dstFormat);
                                        
    //tfg2.0
    class RGBImg
    {
private:
        void* impl = nullptr;
        TFSAMP _format;
        uint32_t _width,_height;
        uint32_t _stride;
        uint32_t _channels;
        void copyParm(const RGBImg& cp);
        void init();
public:
        /**
         * 构造RGBImg
         */
        TFEXPORT RGBImg();
        /**
         * 构造并读入图像文件，仅支持jpeg/png/bmp 格式
         *
         * @param filename 文件绝对路径
         */
        TFEXPORT RGBImg(const char *filename);
        TFEXPORT RGBImg(const RGBImg& cp);
        TFEXPORT RGBImg(RGBImg&& cp);
        TFEXPORT ~RGBImg();
        /**
         * 深拷贝赋值
         */
        TFEXPORT RGBImg& operator=(const RGBImg& cp);
        TFEXPORT RGBImg& operator=(RGBImg&& cp);
        
        /**
         * @return 图像宽度
         */
        TFEXPORT uint32_t  GetWidth(){return _width;}
        /**         
         * @return 图像高度
         */
        TFEXPORT uint32_t  GetHeight(){return _height;}
        /**
         * @return 图像宽维度stride
         */
        TFEXPORT uint32_t  GetStride(){return _stride;}
        /**
         * @return 图像通道数
         */
        TFEXPORT uint32_t  GetChannel(){return _channels;}
        /**
         * @return 图像格式，应该等于 @ref TFSAMP
         */
        TFEXPORT TFSAMP GetFormat(){return _format;}
        /**
         * @return size of @ref GetData()
         */
        TFEXPORT uint32_t GetDataSize();
        /**
         * @return 数据缓存
         */
        TFEXPORT uint8_t* GetData();
        /**
         * 读入图像文件，仅支持jpeg/png/bmp 格式
         *
         * @param filename 文件绝对路径
         * @return 从文件读取图像
         */
        TFEXPORT uint32_t  ReadImg(const char *filename);
        /**
         * 从内存读取图片，仅支持jpeg/png/bmp 格式
         *
         * @param input 图片二进制数据
         * @param len 数据长度
         * @return 从文件读取图像
         */
        TFEXPORT uint32_t  ReadImg(uint8_t* input,int len);

        /**
         * 将yuv I420 planar 格式数据载入RGBImg ，排列方式为 yyyyuuvv
         * 
         * @param input 二进制数据
         * @param width 图像宽度
         * @param height 数据高度
         * @param stride 每个通道宽维度stride，长度为3（I420）或1（gray）
         * @param planeNum 3（I420）或1（gray）
         * @return 0 为成功,其他值表示发生错误
         */
        TFEXPORT uint32_t  ReadI420_Planar(uint8_t *input,int width,int height,int stride[],int planeNum);
        //samp should be TFSAMP_BGR or TFSAMP_RGB
        /**
         * 将yuv RGB24 格式数据载入RGBImg， 排列方式为 BGRBGR... 或 RGBRGB...
         * 
         * @param input 二进制数据
         * @param width 图像宽度
         * @param height 数据高度
         * @param samp 数据格式，必须为TFSAMP_BGR 或 TFSAMP_RGB，指明两种排列方式之一 @ref TFSAMP
         * @return 0 为成功,其他值表示发生错误
         */
        TFEXPORT uint32_t  ReadXGX24(uint8_t *input,uint32_t width,uint32_t height,TFSAMP samp);
        /**
         * 导出bmp格式图像文件
         * 
         * @param filenname 输出文件路径
         * @return 0 为成功,其他值表示发生错误
         */
        TFEXPORT uint32_t  SaveBmp(const char *filenname);
        /**
         * 
         * 
         * @param 
         * @return 0 为成功,其他值表示发生错误
         */
        TFEXPORT uint32_t  DrawRect(int x,int y,int width,int height);
        /**
         * 缩放操作
         * 
         * @param width 图像宽度
         * @param height 图像高度
         * @param strideAlignment 宽维度对齐值，必须为2的幂次或0（等价于1，不作对齐）
         * @return 0 为成功,其他值表示发生错误
         */
        TFEXPORT uint32_t  Scale(uint32_t width,uint32_t height,uint32_t strideAlignment = 0);
        /**
         * 裁剪操作
         * 
         * @param x 起始列
         * @param y 起始行
         * @param width 裁剪宽度
         * @param height 裁剪高度
         * @param strideAlignment 宽维度对齐值，必须为2的幂次或0（等价于1，不作对齐）
         * @return 0 为成功,其他值表示发生错误
         */
        TFEXPORT uint32_t  Crop(int32_t x,int32_t y,uint32_t width,uint32_t height,uint32_t strideAlignment = 0);
        /**
         * 将图像短边缩放到shortWidth(长边等比例缩放)，再在中心区域裁剪width * height 的块
         * 
         * @param width 裁剪宽度
         * @param height 裁剪高度
         * @param shortWidth 短边缩放长度
         * @param strideAlignment 宽维度对齐值，必须为2的幂次或0（等价于1，不作对齐）
         * @return 0 为成功,其他值表示发生错误
         */
        TFEXPORT uint32_t  CenterCrop(uint32_t width,uint32_t height,uint32_t shortWidth = 256,uint32_t strideAlignment = 0);
        /**
         * 将图像扩展为width * height尺寸，原图置于中心
         * 
         * @param width 目标宽度
         * @param height 目标高度
         * @return 0 为成功,其他值表示发生错误
         */
        TFEXPORT uint32_t  CenterPadding(uint32_t width,uint32_t height);
        /**
         * 宽维度对齐
         * 
         * @param strideAlignment 宽维度对齐值，必须为2的幂次或0（等价于1，不作对齐）
         * @return 0 为成功,其他值表示发生错误
         */
        TFEXPORT uint32_t  StrideAlignment(uint32_t strideAlignment);
        /**
         * 将数据以SPLIT 格式导出到buffer，排列方式 BBBB... GGGG... RRRR... @ref TFSAMP
         *
         * @param buffer 导出地址
         * @param format BGR_SPLIT 或 RGB_SPLIT 指明三通道顺序
         * @param channelAlignment 通道维度对齐值，必须为2的幂次或0（等价于1，不作对齐）
         * @return 0 为成功,其他值表示发生错误
         */
        TFEXPORT uint32_t  SplitTo(uint8_t* buffer, uint32_t channelAlignment = 0, TFSAMP format = TFSAMP_BGR_SPLIT);
        /**
         * 将数据以SPLIT 格式，(value - mean)/std 量化为float，导出到buffer，排列方式 BBBB... GGGG... RRRR... @ref TFSAMP
         *
         * @param buffer 导出地址
         * @param mean mean值数组，长度为3，null表示0.0f
         * @param std std值数组，长度为3,null 表示 1.0f
         * @param format BGR_SPLIT 或 RGB_SPLIT 指明三通道顺序
         * @param channelAlignment 通道维度对齐值，必须为2的幂次或0（等价于1，不作对齐）
         * @return 0 为成功,其他值表示发生错误
         */
        TFEXPORT uint32_t  SplitToFloat(float* buffer,float* mean = nullptr,float* std = nullptr, uint32_t channelAlignment = 0, TFSAMP format = TFSAMP_BGR_SPLIT);
        /**
         * 深拷贝一个实例
         */
        TFEXPORT RGBImg  Clone() const;
    };

#ifdef USE_TFGS
    //for Streaming
#define RTSP_IMPL_TFG 0
#define RTSP_IMPL_FFMPEG 1
    typedef enum {
        CodecInvalid = -1,
        CodecH264 = 0,
        CodecH265,
        CodecAAC,
        CodecG711A,
        CodecG711U,
        CodecOpus,
        CodecL16,
        CodecMax = 0x7FFF
    } CodecId;
    
    typedef void (*rtsp_callback)(void *user_info, int _frameType, char *_pBuf, RTSP_FRAME_INFO* _frameInfo);
    typedef void (*onRTPServerCallback)(uint8_t* data,int len,int width,int height,CodecId codecId,bool isKey,int status,std::string streamId);
    class RTSPImpl;
    class RTSPSession
    {
    private:
        RTSPImpl *impl;
        RTSPSession(int impl);
        ~RTSPSession();
    public:
        TFEXPORT static RTSPSession *CreateSession(int impl =RTSP_IMPL_TFG);
        TFEXPORT void SetCallBack(rtsp_callback callback);
        TFEXPORT int OpenStream(const char *url,void *userInfo,EASY_RTP_CONNECT_TYPE connectType);
        TFEXPORT int CloseStream();
        TFEXPORT void Destroy();
    };
    // class RTPImpl impl;
    // class RTPSession
    // {
    // private:
    //     RTPImpl *impl;
    //     RTPSession();
    //     ~RTPSession();
    // public:
    //     TFEXPORT static RTPSession *CreateSession();
    //     TFEXPORT void StartListen(uint16_t port, int enable_tcp, const char *stream_id, onRTPServerCallback tfg_cb);
    //     TFEXPORT void Destroy();
    // };
    TFEXPORT void* StartRtpServer(uint16_t port, int enable_tcp, const char *stream_id, onRTPServerCallback tfg_cb);
    TFEXPORT void ReleasetRtpServer(void* server) ;
#endif
}
#endif //TFGH_H