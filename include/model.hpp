#pragma once

#include <string>

enum class DataQuality {
    VALID,
    DEGRADED,
    REJECTED
};

std::string dataQualityToString(DataQuality quality);

enum class TrackState {
    TENTATIVE,
    CONFIRMED,
    COASTING,
    DROPPED
};

std::string trackStateToString(TrackState state);

enum class ThreatLevel {
    LOW,
    MEDIUM,
    HIGH
};

std::string threatLevelToString(ThreatLevel level);

struct Measurement {
    int timestep;
    int track_id;
    double x;
    double y;
    double speed;
    
    Measurement(int t, int id, double x_pos, double y_pos, double spd = 0.0);
};

class Track {
public:
    Track(int id, double x, double y, int timestep);
    void update(double x, double y, int timestep, DataQuality quality);
    void markCoasting();
    
    int getId() const { return id_; }
    double getX() const { return x_; }
    double getY() const { return y_; }
    int getLastSeenTime() const { return last_seen_; }
    int getTimesSeenConfirmed() const { return times_seen_; }
    int getMissedUpdates() const { return missed_updates_; }
    TrackState getState() const { return state_; }
    ThreatLevel getThreatLevel() const { return threat_level_; }
    DataQuality getLastQuality() const { return last_quality_; }
    
    void setState(TrackState state) { state_ = state; }
    void setThreatLevel(ThreatLevel level) { threat_level_ = level; }
    
private:
    int id_;
    double x_;
    double y_;
    int last_seen_;
    int times_seen_;
    int missed_updates_;
    TrackState state_;
    ThreatLevel threat_level_;
    DataQuality last_quality_;
};
