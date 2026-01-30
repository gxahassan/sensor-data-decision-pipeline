#pragma once

#include <string>
#include <vector>
#include "model.hpp"

class ScenarioLoader {
public:
    ScenarioLoader(const std::string& filepath);
    
    bool load();
    const std::vector<Measurement>& getMeasurements() const { return measurements_; }
    
private:
    std::string filepath_;
    std::vector<Measurement> measurements_;
};
