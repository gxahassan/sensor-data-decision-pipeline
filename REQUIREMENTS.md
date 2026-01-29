# Requirements for project
- System shall ingest simulated sensor measurements for multiple targets.
- System shall validate measurements and label them VALID/DEGRADED/REJECTED.
- System shall maintain tracks across time with lifecycle states (TENTATIVE/CONFIRMED/COASTING/DROPPED).
- System shall compute a threat level (LOW/MED/HIGH) using configurable rules.
- System shall generate alerts for high threats and suppress duplicates.
- System shall produce logs and an end-of-run summary report.
- System shall be deterministic given the same scenario + seed

Completing these functional requirements will define a successful project.

# Non-goals
- No real radar equations/signal processing
- No machine learning classification
- No real-time/threading guarantees