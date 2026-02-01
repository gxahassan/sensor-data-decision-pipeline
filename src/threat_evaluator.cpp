#include "threat_evaluator.hpp"
#include <cmath>
#include <algorithm>

ThreatEvaluator::ThreatEvaluator()
    : high_speed_threshold_(80.0),
      very_high_speed_threshold_(150.0),
      protected_zone_radius_(10.0),
      warning_zone_radius_(20.0) {
}

ThreatLevel ThreatEvaluator::evaluate(const Track& track) const {
    int score = 0;
    
    double speed = track.getSpeed();
    if (speed > very_high_speed_threshold_) {
        score += 2;
    } else if (speed > high_speed_threshold_) {
        score += 1;
    }
    
    double dist_from_origin = std::sqrt(track.getX() * track.getX() + 
                                        track.getY() * track.getY());
    
    if (dist_from_origin < protected_zone_radius_) {
        score += 2;
    } else if (dist_from_origin < warning_zone_radius_) {
        score += 1;
    }
    
    if (track.getState() == TrackState::CONFIRMED) {
        score += 1;
    }
    
    if (track.getState() == TrackState::COASTING) {
        score = std::max(0, score - 1);
    }
    
    if (score == 0) {
        return ThreatLevel::LOW;
    } else if (score == 1) {
        return ThreatLevel::MEDIUM;
    } else {
        if (track.getState() == TrackState::COASTING) {
            return ThreatLevel::MEDIUM;
        }
        return ThreatLevel::HIGH;
    }
}
