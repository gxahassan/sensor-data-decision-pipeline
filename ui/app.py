import streamlit as st
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path

st.set_page_config(page_title="Sensor Track Replay Viewer", layout="wide")

TRACKS_FILE = "../output/tracks.csv"
ALERTS_FILE = "../output/alerts.csv"
PROTECTED_ZONE_RADIUS = 10
WARNING_ZONE_RADIUS = 20

@st.cache_data
def load_data():
    tracks_path = Path(TRACKS_FILE)
    alerts_path = Path(ALERTS_FILE)
    
    if not tracks_path.exists():
        st.error(f"tracks.csv not found at {TRACKS_FILE}")
        st.info("Run the C++ sensor_system executable first to generate output files.")
        return None, None
    
    if not alerts_path.exists():
        st.error(f"alerts.csv not found at {ALERTS_FILE}")
        return None, None
    
    tracks = pd.read_csv(TRACKS_FILE)
    alerts = pd.read_csv(ALERTS_FILE)
    
    return tracks, alerts

def get_active_alerts(alerts_df, current_timestep):
    if alerts_df is None or alerts_df.empty:
        return {}
    
    active = {}
    relevant_alerts = alerts_df[alerts_df['timestep'] <= current_timestep].sort_values('timestep')
    
    for _, alert in relevant_alerts.iterrows():
        track_id = alert['track_id']
        if alert['event_type'] == 'RAISED':
            active[track_id] = {
                'threat_level': alert['threat_level'],
                'reason': alert['reason'],
                'timestep': alert['timestep']
            }
        elif alert['event_type'] == 'CLEARED' and track_id in active:
            del active[track_id]
    
    return active

def plot_tracks(df, show_labels, auto_scale, all_tracks_df, current_timestep, show_trails):
    fig, ax = plt.subplots(figsize=(8, 8))
    fig.patch.set_facecolor('black')
    ax.set_facecolor('black')
    
    protected = plt.Circle((0, 0), PROTECTED_ZONE_RADIUS, color='lime', fill=False, 
                          linestyle='--', linewidth=2, label='Protected Zone')
    warning = plt.Circle((0, 0), WARNING_ZONE_RADIUS, color='lime', fill=False, 
                        linestyle=':', linewidth=2, label='Warning Zone')
    ax.add_patch(protected)
    ax.add_patch(warning)
    
    ax.plot(0, 0, marker='o', color='lime', markersize=10, label='Origin')
    
    if show_trails:
        for track_id in df['track_id'].unique():
            track_history = all_tracks_df[
                (all_tracks_df['track_id'] == track_id) & 
                (all_tracks_df['timestep'] <= current_timestep)
            ].sort_values('timestep')
            
            if len(track_history) > 1:
                ax.plot(track_history['x'], track_history['y'], 
                       linestyle=':', linewidth=1.5, alpha=0.6, color='lightgray')
    
    threat_colors = {'LOW': 'green', 'MEDIUM': 'orange', 'HIGH': 'red'}
    state_markers = {'TENTATIVE': 'x', 'CONFIRMED': 'o', 'COASTING': '^', 'DROPPED': 's'}
    
    for threat in df['threat'].unique():
        for state in df['state'].unique():
            subset = df[(df['threat'] == threat) & (df['state'] == state)]
            if not subset.empty:
                ax.scatter(subset['x'], subset['y'], 
                          c=threat_colors.get(threat, 'gray'),
                          marker=state_markers.get(state, 'o'),
                          s=150, alpha=0.7, edgecolors='black', linewidth=1.5,
                          label=f'{state} - {threat}')
                
                if show_labels:
                    for _, track in subset.iterrows():
                        ax.annotate(str(track['track_id']), 
                                  (track['x'], track['y']),
                                  xytext=(5, 5), textcoords='offset points',
                                  fontsize=9, fontweight='bold', color='white')
    
    ax.axhline(0, color='white', linestyle='-', linewidth=0.5, alpha=0.3)
    ax.axvline(0, color='white', linestyle='-', linewidth=0.5, alpha=0.3)
    ax.grid(True, alpha=0.3, color='white')
    ax.set_xlabel('X Position', fontsize=11, color='white')
    ax.set_ylabel('Y Position', fontsize=11, color='white')
    ax.set_title('Track Positions', fontsize=13, fontweight='bold', color='white')
    ax.tick_params(colors='white')
    
    if auto_scale:
        if df.empty:
            ax.set_xlim(-30, 30)
            ax.set_ylim(-30, 30)
        else:
            margin = 5
            x_min, x_max = df['x'].min() - margin, df['x'].max() + margin
            y_min, y_max = df['y'].min() - margin, df['y'].max() + margin
            limit = max(abs(x_min), abs(x_max), abs(y_min), abs(y_max), 15)
            ax.set_xlim(-limit, limit)
            ax.set_ylim(-limit, limit)
    else:
        ax.set_xlim(-30, 30)
        ax.set_ylim(-30, 30)
    
    ax.set_aspect('equal')
    
    legend = ax.legend(loc='upper right', fontsize=8)
    legend.get_frame().set_facecolor('black')
    legend.get_frame().set_edgecolor('white')
    for text in legend.get_texts():
        text.set_color('white')
    
    return fig

st.title("Sensor Track Replay Viewer")
st.markdown("---")

tracks_df, alerts_df = load_data()

if tracks_df is None or alerts_df is None:
    st.stop()

if tracks_df.empty:
    st.warning("No track data available.")
    st.stop()

min_t = int(tracks_df['timestep'].min())
max_t = int(tracks_df['timestep'].max())

if 'current_t' not in st.session_state:
    st.session_state.current_t = min_t

st.sidebar.header("Controls")

st.sidebar.markdown("---")
st.sidebar.subheader("Playback")

col1, col2 = st.sidebar.columns(2)
with col1:
    if st.button("Prev"):
        if st.session_state.current_t > min_t:
            st.session_state.current_t -= 1
            st.rerun()

with col2:
    if st.button("Next"):
        if st.session_state.current_t < max_t:
            st.session_state.current_t += 1
            st.rerun()

current_timestep = st.sidebar.slider("Timestep", min_t, max_t, st.session_state.current_t, key='timestep_slider')

if current_timestep != st.session_state.current_t:
    st.session_state.current_t = current_timestep

st.sidebar.markdown("---")
st.sidebar.subheader("Filters")

all_states = tracks_df['state'].unique().tolist()
default_states = [s for s in ['CONFIRMED', 'COASTING', 'TENTATIVE'] if s in all_states]
selected_states = st.sidebar.multiselect("Track States", all_states, default=default_states)

all_threats = ['LOW', 'MEDIUM', 'HIGH']
selected_threats = st.sidebar.multiselect("Threat Levels", all_threats, default=all_threats)

show_labels = st.sidebar.checkbox("Show track labels", value=True, key="show_labels")
auto_scale = st.sidebar.checkbox("Auto-scale plot", value=False, key="auto_scale")
show_trails = st.sidebar.checkbox("Show track trails", value=True, key="show_trails")

current_tracks = tracks_df[tracks_df['timestep'] == current_timestep].copy()

if selected_states:
    current_tracks = current_tracks[current_tracks['state'].isin(selected_states)]
if selected_threats:
    current_tracks = current_tracks[current_tracks['threat'].isin(selected_threats)]

current_tracks['staleness'] = current_tracks['timestep'] - current_tracks['last_seen']

threat_order = {'HIGH': 0, 'MEDIUM': 1, 'LOW': 2}
state_order = {'CONFIRMED': 0, 'COASTING': 1, 'TENTATIVE': 2, 'DROPPED': 3}

current_tracks['threat_sort'] = current_tracks['threat'].map(threat_order)
current_tracks['state_sort'] = current_tracks['state'].map(state_order)
current_tracks = current_tracks.sort_values(['threat_sort', 'state_sort', 'track_id'])
current_tracks = current_tracks.drop(columns=['threat_sort', 'state_sort'])

col_plot, col_table = st.columns([1, 1])

with col_plot:
    st.subheader(f"Timestep {current_timestep}")
    if current_tracks.empty:
        st.info("No tracks match current filters.")
    else:
        fig = plot_tracks(current_tracks, show_labels, auto_scale, tracks_df, current_timestep, show_trails)
        st.pyplot(fig)
        plt.close(fig)

with col_table:
    st.subheader("Track Data")
    if current_tracks.empty:
        st.info("No tracks to display.")
    else:
        display_cols = ['track_id', 'state', 'threat', 'quality', 'speed', 
                       'x', 'y', 'missed', 'last_seen', 'staleness']
        display_df = current_tracks[display_cols].reset_index(drop=True)
        
        st.dataframe(display_df, use_container_width=True, height=400)
        
        st.caption(f"**{len(current_tracks)} track(s)** at timestep {current_timestep}")

st.markdown("---")
st.subheader("Alerts")

col_events, col_active = st.columns([1, 1])

with col_events:
    st.markdown("**Events at Current Timestep**")
    current_alerts = alerts_df[alerts_df['timestep'] == current_timestep]
    
    if current_alerts.empty:
        st.info("No alert events at this timestep.")
    else:
        for _, alert in current_alerts.iterrows():
            st.markdown(f"**{alert['event_type']}** Track {alert['track_id']} "
                       f"**{alert['threat_level']}** – _{alert['reason']}_")

with col_active:
    st.markdown("**Currently Active Alerts**")
    active_alerts = get_active_alerts(alerts_df, current_timestep)
    
    if not active_alerts:
        st.success("No active alerts.")
    else:
        for track_id, info in sorted(active_alerts.items()):
            st.markdown(f"Track **{track_id}** – {info['threat_level']} "
                       f"(raised at t={info['timestep']})")
            st.caption(f"   _{info['reason']}_")

st.sidebar.markdown("---")
st.sidebar.caption(f"Timesteps: {min_t} → {max_t}")
st.sidebar.caption(f"Total tracks: {tracks_df['track_id'].nunique()}")
st.sidebar.caption(f"Total alerts: {len(alerts_df)}")

st.sidebar.markdown("---")
with st.sidebar.expander("Dictionary", expanded=False):
    st.markdown("**Track States**")
    st.markdown("- **TENTATIVE**: New track, not yet confirmed")
    st.markdown("- **CONFIRMED**: Track validated by multiple detections")
    st.markdown("- **COASTING**: Track temporarily lost, position predicted")
    st.markdown("- **DROPPED**: Track lost permanently")
    
    st.markdown("**Threat Levels**")
    st.markdown("- **LOW**: Outside warning zone, low speed")
    st.markdown("- **MEDIUM**: In warning zone or moderate speed")
    st.markdown("- **HIGH**: In protected zone or high speed")
    
    st.markdown("**Data Quality**")
    st.markdown("- **VALID**: Measurement within normal parameters")
    st.markdown("- **DEGRADED**: Marginal measurement quality")
    st.markdown("- **REJECTED**: Measurement failed validation")
    
    st.markdown("**Zones**")
    st.markdown(f"- **Protected Zone**: Radius {PROTECTED_ZONE_RADIUS} (dashed)")
    st.markdown(f"- **Warning Zone**: Radius {WARNING_ZONE_RADIUS} (dotted)")
