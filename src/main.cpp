#include <iostream>
#include <map>
#include "model.hpp"
#include "scenario_loader.hpp"
#include "validator.hpp"

int main() {
    std::cout << "==============================================\n";
    std::cout << "Sensor Data Processing & Track Management\n";
    std::cout << "==============================================\n\n";
    
    Validator validator;
    
    ScenarioLoader loader("../scenarios/demo.csv");
    if (!loader.load()) {
        return 1;
    }
    
    const auto& measurements = loader.getMeasurements();
    std::cout << "Loaded " << measurements.size() << " measurements\n\n";
    
    std::map<int, std::vector<Measurement>> frames;
    for (const auto& m : measurements) {
        frames[m.timestep].push_back(m);
    }
    
    if (frames.empty()) {
        std::cerr << "No measurements loaded.\n";
        return 1;
    }
    
    int min_t = frames.begin()->first;
    int max_t = frames.rbegin()->first;
    std::cout << "Timesteps: " << min_t << " → " << max_t << "\n\n";
    
    for (const auto& [timestep, frame] : frames) {
        std::cout << "Timestep " << timestep << ":\n";
        std::cout << "  " << frame.size() << " measurement(s)\n";
        
        int valid = 0, degraded = 0, rejected = 0;
        
        for (const auto& m : frame) {
            DataQuality quality = validator.assess(m);
            
            std::cout << "    Track " << m.track_id 
                      << " at (" << m.x << ", " << m.y << ")"
                      << " speed=" << m.speed
                      << " quality=" << dataQualityToString(quality) << "\n";
            
            if (quality == DataQuality::VALID) valid++;
            else if (quality == DataQuality::DEGRADED) degraded++;
            else rejected++;
        }
        
        std::cout << "  Summary: VALID=" << valid 
                  << " DEGRADED=" << degraded 
                  << " REJECTED=" << rejected << "\n\n";
    }
    
    std::cout << "✅ Section 3 complete - validation integrated!\n";
    
    return 0;
}