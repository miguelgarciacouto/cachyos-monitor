#pragma once

#include "providers/IMetricProvider.hpp"
#include <string>
#include <mutex>
#include <thread>
#include <chrono> 
#include <vector>

namespace cachy {

class schedulerProvider : public IMetricProvider {
public: 
    schedulerProvider();
    ~schedulerProvider() override = default;

    void Update() override;

    [[nodiscard]] std::string GetName() const override;
    [[nodiscard]] std::string GetLatestPayload() const override;
    [[nodiscard]] double GetCurrentRate() const;
    [[nodiscard]] std::vector<int> GetHistory() const;

private: 
    mutable std::mutex dataMutex_;

    std::string latestData_{"Calculating CTXT rate..."};
    double currentRate_{0.0};

    std::vector<int> history_;

    unsigned long long lastCtxt_{0}; 
    std::chrono::steady_clock::time_point lastTime_;

    unsigned long long ReadCtxt() const;

    std::jthread workerThread_;

}; 

}