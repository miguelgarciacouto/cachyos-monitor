#pragma once

#include <string>

namespace cachy {

class IMetricProvider {

public: 
    virtual ~IMetricProvider() = default;

    virtual void Update() = 0;

    //A getter for the UI to retrieve the formatted data
    //[[nodiscard]] will warn the caller if they forget to use result 
    [[nodiscard]] virtual std::string GetName() const = 0;
    [[nodiscard]] virtual std::string GetLatestPayload() const = 0;

};

} 