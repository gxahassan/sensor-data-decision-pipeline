#include "event_logger.hpp"

EventLogger::EventLogger(const std::string& tracks_file, const std::string& alerts_file) {
    tracks_stream_.open(tracks_file);
    alerts_stream_.open(alerts_file);
    
    tracks_stream_ << "timestep,track_id,x,y,speed,state,threat,quality,missed,last_seen\n";
    alerts_stream_ << "timestep,track_id,event_type,threat_level,reason\n";
}

EventLogger::~EventLogger() {
    if (tracks_stream_.is_open()) {
        tracks_stream_.close();
    }
    if (alerts_stream_.is_open()) {
        alerts_stream_.close();
    }
}

void EventLogger::logTrack(int timestep, const Track& track) {
    tracks_stream_ << timestep << ","
                   << track.getId() << ","
                   << track.getX() << ","
                   << track.getY() << ","
                   << track.getSpeed() << ","
                   << trackStateToString(track.getState()) << ","
                   << threatLevelToString(track.getThreatLevel()) << ","
                   << dataQualityToString(track.getLastQuality()) << ","
                   << track.getMissedUpdates() << ","
                   << track.getLastSeenTime() << "\n";
}

void EventLogger::logAlert(const AlertEvent& event) {
    std::string event_type = (event.type == AlertEvent::RAISED) ? "RAISED" : "CLEARED";
    
    alerts_stream_ << event.timestep << ","
                   << event.track_id << ","
                   << event_type << ","
                   << threatLevelToString(event.threat_level) << ","
                   << event.reason << "\n";
}

void EventLogger::flush() {
    tracks_stream_.flush();
    alerts_stream_.flush();
}
