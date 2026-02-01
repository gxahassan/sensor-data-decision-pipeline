#pragma once

#include <map>
#include <set>
#include <vector>
#include <string>
#include "model.hpp"

struct AlertEvent {
    enum Type {
        RAISED,
        CLEARED
    };
    
    int timestep;
    int track_id;
    Type type;
    ThreatLevel threat_level;
    std::string reason;
    
    AlertEvent(int t, int id, Type ty, ThreatLevel level, const std::string& r)
        : timestep(t), track_id(id), type(ty), threat_level(level), reason(r) {}
};

class AlertManager {
public:
    AlertManager();
    
    std::vector<AlertEvent> processAlerts(int timestep,
                                          const std::map<int, Track>& tracks,
                                          const std::set<int>& dropped_ids);
    
    const std::set<int>& getActiveAlerts() const { return active_alerts_; }
    
private:
    std::set<int> active_alerts_;
};
