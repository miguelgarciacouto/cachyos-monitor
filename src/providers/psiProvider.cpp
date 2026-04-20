#include "providers/psiProvider.hpp"
#include <fstream>
#include <chrono>

namespace cachy {

PsiProvider::PsiProvider() {
    workerThread_ = std::jthread([this](std::stop_token stoken) {
        while(!stoken.stop_requested()) {
            Update();
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
}

void PsiProvider::Update() {
    std::string newData = ReadProcPressure();

    std::scoped_lock lock(dataMutex_);
    latestData_ = newData; 
}

std::string PsiProvider::GetName() const {
    return "PSI";
}

std::string PsiProvider::GetLatestPayload() const {
    std::scoped_lock lock(dataMutex_);
    return latestData_;
}

std::string PsiProvider::ReadProcPressure() const {
    //RAI 
    std::ifstream file("/proc/pressure/cpu");
    if (!file.is_open()) {
        return "Error: Unable to read /proc/pressure/psi";
        }

    std::string line;

    if(std::getline(file, line)) {
        return line;
    } 
    else {
        return "Error: /proc/pressure/psi is empty";
    }

}

}