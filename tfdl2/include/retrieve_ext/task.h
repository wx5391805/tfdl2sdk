#ifndef TFACC_RETRIEVE_TASK_H
#define TFACC_RETRIEVE_TASK_H

#include <map>
#include <vector>
#include <set>

namespace tfacc40t {

/// 检索的返回结果
struct RetrieveTaskResult {
public:

    /// topK 相似的feature id , 以及对应的 score
    /// outputIds.size() == queryNum, outputIds.size() == outputScores.size()
    /// outputScoresAndIds.size() == queryNum
    std::vector<std::vector<std::pair<int, uint64_t> > > outputScoresAndIds;
    int topK;
    int taskId;

    /// 初始化
    RetrieveTaskResult(int taskId, int queryNum, int topK);
    RetrieveTaskResult();

    void Sort();

    /// debug usage
    friend std::ostream& operator<< (std::ostream& out, const RetrieveTaskResult& result);
};

/// 检索查询条件
struct RetrieveTaskQuery {
public:

    /// 查询特征数量, 一般有三种使用场景
    /// 大库检索场景下，通常queryNum = 1, 一次搜索10% 的库
    /// 裸1:N 场景下，通常queryNum 比较大，一次搜索全部库
    /// n:N 小任务，此时queryNum 比较大，搜索的库 = 1(一个小任务的N在一个库中)
    int queryNum;

    /// topK, 返回前topK 个相似特征
    int topK;

    /// 检索的特征数据
    /// query feature data, datas.size() = queryNum * featureDim
    std::vector<uint8_t> datas;

    /// thresholds.size() = queryNum
    std::vector<int> thresholds;

    /// query data feature 的norm
    std::vector<int> norms;

    /// 要检索的库列表
    std::vector<int> queryRepoIds;

    RetrieveTaskQuery() {}

    friend std::ostream& operator<< (std::ostream& out, const RetrieveTaskQuery& q);
};

/// 代表一次检索任务
struct RetrieveTask {
public:

    /// 任务唯一ID
    int taskId;
    
    /// 任务优先级，数值越高优先级越高, 最小为0
    int priority;

    /// 查询条件
    RetrieveTaskQuery query;

    /// 计算结果
    RetrieveTaskResult result;

    /// 当前这个检索任务，已经检索完成的库的数量
    int searchedRepo;

    /// 当前这个检索任务，要检索的库数量
    int toSearchRepo;

    /// 初始化task
    RetrieveTask(int taskId, int priority, const RetrieveTaskQuery& query);

    /// 当前这个任务是否已经完成
    bool isFinished();

    /// debug usage
    friend std::ostream& operator<< (std::ostream& out, const RetrieveTask& t);

    /// 接收到的时间戳
    long long receivedTs;

    /// 分配到各repo上的时间戳
    long long assignedTs;

    /// 所有的repo上都retrieve完
    long long finishedTs;

    /// 最终排序完
    long long sortedTs;
};


}

#endif