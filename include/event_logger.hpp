#pragma once

#include <string>
#include <fstream>
#include "model.hpp"
#include "alert_manager.hpp"

class EventLogger {
public:
    EventLogger(const std::string& tracks_file, const std::string& alerts_file);
    ~EventLogger();
    
    void logTrack(int timestep, const Track& track);
    void logAlert(const AlertEvent& event);
    void flush();
    
private:
    std::ofstream tracks_stream_;
    std::ofstream alerts_stream_;
};
