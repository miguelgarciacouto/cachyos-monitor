#include "providers/zramProviders.hpp"
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace cachy {

ZramProvider::ZramProvider() {
    workerThread_ = std::jthread([this](std::stop_token stoken) {
        while(!stoken.stop_requested()) {
            Update();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

}

void ZramProvider::Update() {
    unsigned long long diskSizeBytes = 0;
    {
        std::ifstream sizeFile("/sys/block/zram0/disksize");
        if (!(sizeFile >> diskSizeBytes)) diskSizeBytes = 0;
    }

    unsigned long long orig = 0, compr = 0, totalMemUsed = 0;
    {
        std::ifstream statFile("/sys/block/zram0/mm_stat");
        if (!(statFile >> orig >> compr >> totalMemUsed)) {
            std::scoped_lock lock(dataMutex_);
            latestData_ = "ZRAM Stats Unavailable";
            return;
        }
    }

    ZramMetrics newMetrics;
    newMetrics.totalMiB = static_cast<double>(diskSizeBytes) / (1024.0 * 1024.0);
    newMetrics.usedMiB = static_cast<double>(totalMemUsed) / (1024.0 * 1024.0);
    newMetrics.originalMiB = static_cast<double>(orig) / (1024.0 * 1024.0);
    newMetrics.compressedMiB = static_cast<double>(compr) / (1024.0 * 1024.0);
    newMetrics.ratio = (compr > 0) ? static_cast<double>(orig) / static_cast<double>(compr) : 0.0;

    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) 
       << newMetrics.usedMiB << " / " << newMetrics.totalMiB << " MiB Used (" 
       << std::setprecision(2) << newMetrics.ratio << "x compression)";

    std::scoped_lock lock(dataMutex_);
    currentMetrics_ = newMetrics;
    latestData_ = ss.str();
}

std::string ZramProvider::GetName() const {
    return "ZRAM Compression";
}

std::string ZramProvider::GetLatestPayload() const {
    std::scoped_lock lock(dataMutex_);
    return latestData_;
}

ZramMetrics ZramProvider::GetMetrics() const {
    std::scoped_lock lock(dataMutex_);
    return currentMetrics_;
}

}