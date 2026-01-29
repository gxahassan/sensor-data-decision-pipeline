#include "model.hpp"

std::string dataQualityToString(DataQuality quality) {
    switch (quality) {
        case DataQuality::VALID:     return "VALID";
        case DataQuality::DEGRADED:  return "DEGRADED";
        case DataQuality::REJECTED:  return "REJECTED";
        default:                     return "UNKNOWN";
    }
}

std::string trackStateToString(TrackState state) {
    switch (state) {
        case TrackState::TENTATIVE:  return "TENTATIVE";
        case TrackState::CONFIRMED:  return "CONFIRMED";
        case TrackState::COASTING:   return "COASTING";
        case TrackState::DROPPED:    return "DROPPED";
        default:                     return "UNKNOWN";
    }
}

std::string threatLevelToString(ThreatLevel level) {
    switch (level) {
        case ThreatLevel::LOW:     return "LOW";
        case ThreatLevel::MEDIUM:  return "MEDIUM";
        case ThreatLevel::HIGH:    return "HIGH";
        default:                   return "UNKNOWN";
    }
}

Measurement::Measurement(int t, int id, double x_pos, double y_pos, double spd)
    : timestep(t), track_id(id), x(x_pos), y(y_pos), speed(spd) {
}

Track::Track(int id, double x, double y, int timestep)
    : id_(id),
      x_(x),
      y_(y),
      last_seen_(timestep),
      times_seen_(1),
      missed_updates_(0),
      state_(TrackState::TENTATIVE),
      threat_level_(ThreatLevel::LOW),
      last_quality_(DataQuality::VALID) {
}

void Track::update(double x, double y, int timestep, DataQuality quality) {
    x_ = x;
    y_ = y;
    last_seen_ = timestep;
    last_quality_ = quality;
    missed_updates_ = 0;
    
    if (quality == DataQuality::VALID || quality == DataQuality::DEGRADED) {
        times_seen_++;
    }
}

void Track::markCoasting() {
    missed_updates_++;
}
