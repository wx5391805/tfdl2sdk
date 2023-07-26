#ifndef NVN_TASK_H
#define NVN_TASK_H

namespace tfacc40t {

    /// N:N计算请求
    /// 示例：3个query检索2个db{分别5和4特征}，即3:5, 3:4
    /// 输入：
    /// queryFeatureNum: 3, queryFeatureData: {q1 ... q1, q2 ... q2, q3 ... q3}
    /// thresholds: {{tq1, tq2, tq3}, {tq1', tq2', tq3'}}
    /// topKs: {2, 2}
    /// limits: {20, 20}
    /// dbFeatureNums: {5, 4}
    /// dbDatas: {{f1 ... f1, ... f5 ... f5}, {f1' ... f1', ... f4' ... f4'}}
    /// 输出：
    /// outputCounts: {{2, 2, 2}, {1, 2, 2}}
    /// outputOffsets: {{1, 2, 2, 3, 1, 3}, {1', x, 1', 2', 3', 4'}}
    /// outputScores: {{100, 200, 100, 200, 100, 200}, {100, x, 100, 200, 100, 200}}
    /// outputFlags: {{0, 0, 0, 0, 0, 0}, {0, x, 0, 0, 0, 0}}
    struct NvNRetrieveTaskRequest {
        /// 一次N:N的task ID，用户层去重
        int taskId;
        /// query特征数
        int queryFeatureNum;
        /// query特征数据（queryFeatureNum*featureDim大小)
        uint8_t* queryFeatureData;
        /// query在每个db上的阈值
        std::vector<int *> thresholds;
        /// 本次N:N的query在每个db上的topK(topKs.size() == db数目)
        std::vector<int> topKs;
        /// 本次N:N的query在每个db上的截断参数(limits.size() == db数目)
        std::vector<int> limits;
        /// 每个db的特征数目大小，dbFeatureNums.size() == db数目
        std::vector<int> dbFeatureNums;
        /// 每个db的特征数据（sizeof(dbDatas[i]) == dbFeatureNums[i]*featureDim）
        std::vector<uint8_t *> dbDatas;
        /// 每个query在每个db上检索到的特征数目
        /// outputCounts.size() == db数目，outputCounts[i].size() == queryFeatureNum
        std::vector<int *> outputCounts;
        /// 返回的特征偏移列表
        /// outputOffsets[i]需要在外部申请至少(queryFeatureNum * topK[i] * sizeof(uint32_t))的空间
        /// 表示这批query在db[i]上检索到的db 偏移列表，每个query预留topK[i]个结果，结果按欧式距离从小到大排列
        /// vector的大小同db数目
        std::vector<uint32_t *> outputOffsets;
        /// 返回的分数列表
        /// outputScores[i]需要在外部申请至少(queryFeatureNum * topK[i] * sizeof(int32_t))的空间
        /// 表示这批query在db[i]上检索到的特征的欧式距离，结果从小到大排列
        /// vector的大小同db数目
        std::vector<int *> outputScores;
        /// 每个query的返回标记, outputFlags.size() == db数目, outputFlags[i].size() == queryFeatureNum
        /// outputFlags[i]需要在外部申请至少(queryFeatureNum * sizeof(int32_t))的空间
        /// outputFlags[i][j]:
        ///     bit[0]      - 1发生截断
        ///     bit[1-7]    - 未定义
        std::vector<int *> outputFlags;
    };

    /// N:N 结果返回值
    /// 返回值由request中准备好，详细的定义见request
    struct NvNRetrieveTaskResult {
        /// 任务ID
        int taskId;
        /// query特征数
        int queryFeatureNum;
        /// 本次N:N的query在每个db上的topK(topKs.size() == db数目)
        std::vector<int> topKs;
        /// 每个query在每个db上检索到的特征数目
        std::vector<int *> outputCounts;
        /// 返回的特征偏移列表
        std::vector<uint32_t *> outputOffsets;
        /// 返回的分数列表
        std::vector<int *> outputScores;
        /// 每个query的返回标记
        std::vector<int *> outputFlags;
    };

}


#endif
