#pragma once 

#include "IMetricProvider.hpp"
#include <mutex> 
#include <thread>
#include <string> 
#include <vector>

namespace cachy { 

struct ZramMetrics {
    double totalMiB = 0.0;
    double usedMiB = 0.0;
    double originalMiB = 0.0;
    double compressedMiB = 0.0;
    double ratio = 0.0;
};

class ZramProvider : public IMetricProvider {
public: 
    ZramProvider();
    ~ZramProvider() override = default;

    void Update() override;

    [[nodiscard]] std::string GetName() const override;
    [[nodiscard]] std::string GetLatestPayload() const override;
    [[nodiscard]] ZramMetrics GetMetrics() const;

private:
    mutable std::mutex dataMutex_;
    std::string latestData_{"Initializing zram..."};
    ZramMetrics currentMetrics_;

    void ReadZramStats();

    std::jthread workerThread_;
};

}