#pragma once

#include "model.hpp"

class ThreatEvaluator {
public:
    ThreatEvaluator();
    
    ThreatLevel evaluate(const Track& track) const;
    
private:
    double high_speed_threshold_;
    double very_high_speed_threshold_;
    double protected_zone_radius_;
    double warning_zone_radius_;
};
