#pragma once

#include "IMetricProvider.hpp"
#include <string>
#include <mutex>    
#include <thread>   

namespace cachy {

class PsiProvider : public IMetricProvider {

public: 
    PsiProvider();
    ~PsiProvider() override = default;

    void Update() override;

    [[nodiscard]] std::string GetName() const override;
    [[nodiscard]] std::string GetLatestPayload() const override;

private: 

    //Protected data
    mutable std::mutex dataMutex_;
    std::string latestData_{"Initializing PSI..."};

    std::string ReadProcPressure() const;

    //The background worker thread that polls /proc/pressure
    std::jthread workerThread_;

};

}