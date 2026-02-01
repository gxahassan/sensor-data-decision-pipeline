#include "track_manager.hpp"

TrackManager::TrackManager()
    : confirm_threshold_(2),
      drop_threshold_(3) {
}

void TrackManager::processFrame(int timestep,
                                 const std::vector<Measurement>& measurements,
                                 const std::vector<DataQuality>& qualities) {
    updated_this_frame_.clear();
    dropped_this_frame_.clear();
    
    for (size_t i = 0; i < measurements.size(); i++) {
        if (qualities[i] == DataQuality::REJECTED) {
            continue;
        }
        updateTrack(timestep, measurements[i], qualities[i]);
    }
    
    coastMissingTracks(timestep);
    applyStateTransitions();
    removeDroppedTracks();
}

void TrackManager::updateTrack(int timestep, const Measurement& m, DataQuality quality) {
    auto it = tracks_.find(m.track_id);
    
    if (it == tracks_.end()) {
        tracks_.emplace(m.track_id, Track(m.track_id, m.x, m.y, timestep, m.speed));
    } else {
        it->second.update(m.x, m.y, timestep, quality, m.speed);
    }
    
    updated_this_frame_.insert(m.track_id);
}

void TrackManager::coastMissingTracks(int timestep) {
    for (auto& [id, track] : tracks_) {
        if (updated_this_frame_.find(id) == updated_this_frame_.end()) {
            track.markCoasting();
            
            if (track.getState() == TrackState::CONFIRMED) {
                track.setState(TrackState::COASTING);
            }
        }
    }
}

void TrackManager::applyStateTransitions() {
    for (auto& [id, track] : tracks_) {
        if (track.getState() == TrackState::TENTATIVE &&
            track.getTimesSeenConfirmed() >= confirm_threshold_) {
            track.setState(TrackState::CONFIRMED);
        }
        
        if (track.getState() == TrackState::COASTING &&
            updated_this_frame_.find(id) != updated_this_frame_.end()) {
            track.setState(TrackState::CONFIRMED);
        }
        
        if (track.getMissedUpdates() >= drop_threshold_) {
            track.setState(TrackState::DROPPED);
            dropped_this_frame_.insert(id);
        }
    }
}

void TrackManager::removeDroppedTracks() {
    for (int id : dropped_this_frame_) {
        tracks_.erase(id);
    }
}
