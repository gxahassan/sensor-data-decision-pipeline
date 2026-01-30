#pragma once

#include "model.hpp"

class Validator {
public:
    Validator();
    
    DataQuality assess(const Measurement& m) const;
    
private:
    double max_position_;    // beyond this = REJECTED
    double reject_speed_;    // beyond this = REJECTED
    double degrade_speed_;   // beyond this = DEGRADED
};
