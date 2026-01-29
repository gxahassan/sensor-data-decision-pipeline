#include <iostream>
#include <map>
#include "model.hpp"
#include "scenario_loader.hpp"

int main() {
    std::cout << "==============================================\n";
    std::cout << "Sensor Data Processing & Track Management\n";
    std::cout << "==============================================\n\n";
    
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
    
    int min_t = frames.begin()->first;
    int max_t = frames.rbegin()->first;
    std::cout << "Timesteps: " << min_t << " → " << max_t << "\n\n";
    
    for (const auto& [timestep, frame] : frames) {
        std::cout << "Timestep " << timestep << ":\n";
        std::cout << "  " << frame.size() << " measurement(s)\n";
        
        for (const auto& m : frame) {
            std::cout << "    Track " << m.track_id 
                      << " at (" << m.x << ", " << m.y << ")"
                      << " speed=" << m.speed << "\n";
        }
        std::cout << "\n";
    }
    
    std::cout << "✅ Section 2 complete - scenario loaded and timesteps processed in order!\n";
    
    return 0;
}