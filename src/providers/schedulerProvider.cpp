#include "providers/schedulerProvider.hpp"
#include <fstream>
#include <string> 

namespace cachy {

schedulerProvider::schedulerProvider() {
    
    lastTime_ = std::chrono::steady_clock::now();
    lastCtxt_ = ReadCtxt();

    workerThread_ = std::jthread([this](std::stop_token stoken) {
        while (!stoken.stop_requested()) {
            Update();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
}

void schedulerProvider::Update() {
    auto now = std::chrono::steady_clock::now();
    unsigned long long currentCtxt = ReadCtxt();

    std::chrono::duration<double> elapsedSeconds = now - lastTime_;

    unsigned long long delta = currentCtxt - lastCtxt_;
    double ctxtRate = delta / elapsedSeconds.count();

    double rate = static_cast<double>(delta) / elapsedSeconds.count();

    lastCtxt_ = currentCtxt;
    lastTime_ = now; 

    std::scoped_lock lock(dataMutex_);
    currentRate_ = ctxtRate;
    latestData_ = std::to_string(static_cast<unsigned long long>(ctxtRate)) + " switches/s";

    history_.push_back(static_cast<int>(ctxtRate));
    if(history_.size() > 100) {
        history_.erase(history_.begin());
    }
}

double schedulerProvider::GetCurrentRate() const {
    std::scoped_lock lock(dataMutex_);
    return currentRate_;
}

std::string schedulerProvider::GetName() const {
    return "CPU Context Switch rate";
}

std::string schedulerProvider::GetLatestPayload() const {
    std::scoped_lock lock(dataMutex_);
    return latestData_; 
}

unsigned long long schedulerProvider::ReadCtxt() const {
    std::ifstream file("/proc/stat");
    if (!file.is_open()) {
        return 0;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.starts_with("ctxt")) {
            return std::stoull(line.substr(5));
        }
    }

    return 0;
}

std::vector<int> schedulerProvider::GetHistory() const {
    std::scoped_lock lock(dataMutex_);
    return history_;

}

}