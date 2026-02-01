#pragma once

#include <map>
#include <vector>
#include <set>
#include "model.hpp"

class TrackManager {
public:
    TrackManager();
    
    void processFrame(int timestep, 
                      const std::vector<Measurement>& measurements,
                      const std::vector<DataQuality>& qualities);
    
    const std::map<int, Track>& getActiveTracks() const { return tracks_; }
    std::map<int, Track>& getActiveTracksMutable() { return tracks_; }
    const std::set<int>& getUpdatedIds() const { return updated_this_frame_; }
    const std::set<int>& getDroppedIds() const { return dropped_this_frame_; }
    
private:
    void updateTrack(int timestep, const Measurement& m, DataQuality quality);
    void coastMissingTracks(int timestep);
    void applyStateTransitions();
    void removeDroppedTracks();
    
    std::map<int, Track> tracks_;
    std::set<int> updated_this_frame_;
    std::set<int> dropped_this_frame_;
    
    int confirm_threshold_;
    int drop_threshold_;
};
