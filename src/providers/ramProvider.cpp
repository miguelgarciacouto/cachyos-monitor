#include "providers/ramProvider.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

namespace cachy {

RamProvider::RamProvider() {
    workerThread_ = std::jthread([this](std::stop_token stoken) {
        while (!stoken.stop_requested()) {
            Update();
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    });
}

void RamProvider::Update() {
    std::ifstream file ("/proc/meminfo");
    std::string line;
    unsigned long long total = 0, avail = 0; 

    while (std::getline(file, line)) {
        if (line.starts_with("MemTotal:")) {
            std::stringstream ss(line.substr(9));
            ss >> total;
        } 
        else if (line.starts_with("MemAvailable:")) {
            std::stringstream ss(line.substr(13));
            ss >> avail;
        }
    } 

    if (total == 0) return; 

    double totalGB = total / 1024.0 / 1024.0;
    double usedGB = (total - avail) / 1024.0 / 1024.0;
    double fraction = static_cast<double>(total - avail) / total;

    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << usedGB << " / " << totalGB << "GB";

    std::scoped_lock lock(dataMutex_);
    usageFraction_ = fraction;
    latestData_ = ss.str();
}

    std::string RamProvider::GetLatestPayload() const {
        std::scoped_lock lock(dataMutex_);
        return latestData_;
}

double RamProvider::GetUsagePercentage() const {
    std::scoped_lock lock(dataMutex_);
    return usageFraction_;
}

}