#include "scenario_loader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

ScenarioLoader::ScenarioLoader(const std::string& filepath)
    : filepath_(filepath) {
}

bool ScenarioLoader::load() {
    measurements_.clear();
    
    std::ifstream file(filepath_);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open scenario file: " << filepath_ << "\n";
        return false;
    }
    
    std::string line;
    if (!std::getline(file, line)) {
        std::cerr << "Error: Scenario file is empty: " << filepath_ << "\n";
        return false;
    }
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::stringstream ss(line);
        std::string token;
        
        int timestep, track_id;
        double x, y, speed = 0.0;
        
        if (!std::getline(ss, token, ',')) continue;
        timestep = std::stoi(token);
        
        if (!std::getline(ss, token, ',')) continue;
        track_id = std::stoi(token);
        
        if (!std::getline(ss, token, ',')) continue;
        x = std::stod(token);
        
        if (!std::getline(ss, token, ',')) continue;
        y = std::stod(token);
        
        if (std::getline(ss, token, ',')) {
            if (!token.empty()) speed = std::stod(token);
        }
        
        measurements_.emplace_back(timestep, track_id, x, y, speed);
    }
    
    file.close();
    return true;
}
