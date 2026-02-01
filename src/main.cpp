#include <iostream>
#include <map>
#include "model.hpp"
#include "scenario_loader.hpp"
#include "validator.hpp"
#include "track_manager.hpp"
#include "threat_evaluator.hpp"
#include "alert_manager.hpp"
#include "event_logger.hpp"

int main() {
    std::cout << "==============================================\n";
    std::cout << "Sensor Data Processing & Track Management\n";
    std::cout << "==============================================\n\n";
    
    Validator validator;
    TrackManager track_manager;
    ThreatEvaluator threat_evaluator;
    AlertManager alert_manager;
    EventLogger logger("../output/tracks.csv", "../output/alerts.csv");
    
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
        
        std::vector<DataQuality> qualities;
        int valid = 0, degraded = 0, rejected = 0;
        
        for (const auto& m : frame) {
            DataQuality quality = validator.assess(m);
            qualities.push_back(quality);
            
            std::cout << "    Track " << m.track_id 
                      << " at (" << m.x << ", " << m.y << ")"
                      << " speed=" << m.speed
                      << " quality=" << dataQualityToString(quality) << "\n";
            
            if (quality == DataQuality::VALID) valid++;
            else if (quality == DataQuality::DEGRADED) degraded++;
            else rejected++;
        }
        
        std::cout << "  Validation: VALID=" << valid 
                  << " DEGRADED=" << degraded 
                  << " REJECTED=" << rejected << "\n";
        
        track_manager.processFrame(timestep, frame, qualities);
        
        for (auto& [id, track] : track_manager.getActiveTracksMutable()) {
            ThreatLevel threat = threat_evaluator.evaluate(track);
            track.setThreatLevel(threat);
        }
        
        auto alert_events = alert_manager.processAlerts(timestep, 
                                                        track_manager.getActiveTracks(),
                                                        track_manager.getDroppedIds());
                for (const auto& [id, track] : track_manager.getActiveTracks()) {
            logger.logTrack(timestep, track);
        }
        
        for (const auto& event : alert_events) {
            logger.logAlert(event);
        }
                std::cout << "  Active tracks:\n";
        for (const auto& [id, track] : track_manager.getActiveTracks()) {
            std::cout << "    Track " << id 
                      << " state=" << trackStateToString(track.getState())
                      << " threat=" << threatLevelToString(track.getThreatLevel())
                      << " speed=" << track.getSpeed()
                      << " pos=(" << track.getX() << "," << track.getY() << ")\n";
        }
        
        for (int id : track_manager.getDroppedIds()) {
            std::cout << "    Track " << id << " → DROPPED\n";
        }
        
        if (!alert_events.empty()) {
            std::cout << "  Alerts:\n";
            for (const auto& event : alert_events) {
                if (event.type == AlertEvent::RAISED) {
                    std::cout << "    RAISED: Track " << event.track_id 
                              << " " << threatLevelToString(event.threat_level)
                              << " (" << event.reason << ")\n";
                } else {
                    std::cout << "    CLEARED: Track " << event.track_id 
                              << " (" << event.reason << ")\n";
                }
            }
        } else if (!alert_manager.getActiveAlerts().empty()) {
            std::cout << "  Alerts: " << alert_manager.getActiveAlerts().size() << " active\n";
        } else {
            std::cout << "  Alerts: none\n";
        }
        
        std::cout << "\n";
    }
    
    logger.flush();
    
    std::cout << "✅ Section 7 complete - event logging working!\n";
    std::cout << "   Logs written to: output/tracks.csv and output/alerts.csv\n";
    
    return 0;
}