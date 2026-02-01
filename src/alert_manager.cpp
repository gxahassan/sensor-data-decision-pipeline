#include "alert_manager.hpp"
#include <cmath>

AlertManager::AlertManager() {
}

std::vector<AlertEvent> AlertManager::processAlerts(int timestep,
                                                     const std::map<int, Track>& tracks,
                                                     const std::set<int>& dropped_ids) {
    std::vector<AlertEvent> events;
    
    for (int id : dropped_ids) {
        if (active_alerts_.find(id) != active_alerts_.end()) {
            events.emplace_back(timestep, id, AlertEvent::CLEARED, 
                              ThreatLevel::LOW, "track dropped");
            active_alerts_.erase(id);
        }
    }
    
    for (const auto& [id, track] : tracks) {
        ThreatLevel threat = track.getThreatLevel();
        bool currently_alerted = (active_alerts_.find(id) != active_alerts_.end());
        
        if (threat == ThreatLevel::HIGH && !currently_alerted) {
            std::string reason = "high threat detected";
            
            double dist = std::sqrt(track.getX() * track.getX() + 
                                   track.getY() * track.getY());
            if (dist < 10.0) {
                reason = "entered protected zone";
            } else if (track.getSpeed() > 150.0) {
                reason = "very high speed";
            }
            
            events.emplace_back(timestep, id, AlertEvent::RAISED, threat, reason);
            active_alerts_.insert(id);
        }
        else if (threat != ThreatLevel::HIGH && currently_alerted) {
            events.emplace_back(timestep, id, AlertEvent::CLEARED, threat, "threat reduced");
            active_alerts_.erase(id);
        }
    }
    
    return events;
}
