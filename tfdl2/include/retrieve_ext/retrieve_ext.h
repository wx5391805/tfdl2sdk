#ifndef TFACC_RETRIEVE_EXT_H
#define TFACC_RETRIEVE_EXT_H

#include <vector>
#include "task.h"
#include "nvn_task.h"

namespace tfacc40t {

    /// init queue and retrieve engine
    /// 初始化handle对应的异步处理引擎
    /// 注意：不能初始化一个正在使用中的引擎
    /// 用来做ANN 1:N 和 n:N 小任务的引擎不要混用，分别申请两个不同的handle 来初始化两个engine。
    int InitQueueAndEngine(int* handle, int repoBatch, std::vector<int>& repoIDs);

    /// release queue and retrieve engine
    /// 释放handle对应的异步处理引擎
    int ReleaseQueueAndEngine(int* handle);

    /// 在多个仓库中异步1:N查询
    /// 注意：taskId应该小于1024*1024
    /// \param taskId:        任务ID, 应用层唯一且应小于1024*1024
    /// \param priority:      优先级，数值越大的任务优先级越高,  优先级最高的检索任务会抢断，优先级相同的任务会遵循内部调度原则
    /// \param queryNum:      查询数目
    /// \param queryData:     查询的特征数据
    /// \param topK :         对于每个询问返回的特征数量
    /// \param threshold :    询问的阈值，
    /// \param zero :         feature 0 值
    /// \param repoIds :      查询的库id列表，当repoIds.size() = 总库10%时，为ANN场景，当repoIds.size() == 1, 时为单个小n:N 场景
    /// \param handle :       检索实例的句柄
    /// \return               0 - 正常，<0 - 错误
    int Retrieve1vNAsync(int taskId,
                            int priority,
                            int queryNum, const std::vector<uint8_t>& queryData, int topK, int threshold, int zero,
                            const std::vector<int>& repoIds,
                            int *handle);


    /// 查询异步任务计算结果，取出已经计算完成的任务结果
    /// \param results      已经计算完成的任务
    /// \param handle 
    /// \return             一个TF_RET值代表运行成功，或失败的类型
    int Retrieve1vNAsyncFetch(std::vector<RetrieveTaskResult>& results,
                                 int* handle);

    /// 初始化NvN任务层
    /// 先调用RetrieveCreate，后调用InitNvNRetrieveExtEngine
    int InitRetrieveNvNEngine(int *handle, int featureDim, uint8_t zero);

    /// 释放NvN任务层
    /// 先调用ReleaseNvNRetrieveExtEngine、后调用RetrieveRelease
    int ReleaseRetrieveNvNEngine(int *handle);

    /// 异步n:N检索
    /// \param request:     N:N请求，字段说明见NvNRetrieveTaskRequest结构体
    /// \param handle
    /// \return:            一个TF_RET值代表运行成功，或失败的类型
    int RetrieveNvNAsync(NvNRetrieveTaskRequest* request,
                               int* handle);

    /// 查询N:N异步任务计算结果，取出已经计算完成的任务结果
    /// \param results      已经计算完成的任务
    /// \param handle
    /// \return             一个TF_RET值代表运行成功，或失败的类型
    int RetrieveNvNAsyncFetch(std::vector<NvNRetrieveTaskResult>& results,
                                    int* handle);

}

#endif
