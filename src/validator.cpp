#include "validator.hpp"
#include <cmath>

Validator::Validator()
    : max_position_(1000.0),
      reject_speed_(300.0),
      degrade_speed_(100.0) {
}

DataQuality Validator::assess(const Measurement& m) const {
    if (m.speed < 0.0) {
        return DataQuality::REJECTED;
    }
    
    if (std::abs(m.x) > max_position_ || std::abs(m.y) > max_position_) {
        return DataQuality::REJECTED;
    }
    
    if (m.speed > reject_speed_) {
        return DataQuality::REJECTED;
    }
    
    if (m.speed > degrade_speed_) {
        return DataQuality::DEGRADED;
    }
    
    double boundary_threshold = max_position_ * 0.95;
    if (std::abs(m.x) > boundary_threshold || std::abs(m.y) > boundary_threshold) {
        return DataQuality::DEGRADED;
    }
    
    return DataQuality::VALID;
}
