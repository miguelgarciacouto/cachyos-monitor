#pragma once 

#include "providers/IMetricProvider.hpp"
#include <thread> 
#include <mutex> 
#include <string> 

namespace cachy {

class RamProvider : public IMetricProvider {
public:
    RamProvider();
    ~RamProvider() override = default; 

    void Update() override;
    [[nodiscard]] std::string GetName() const override {
        return "Physical RAM";
    }
    [[nodiscard]] std::string GetLatestPayload() const override; 
    [[nodiscard]] double GetUsagePercentage() const;

private: 
    mutable std::mutex dataMutex_;
    std::string latestData_ {
        "Reading RAM..."
    }; 
    double usageFraction_ = 0.0;

    std::jthread workerThread_;

};

}