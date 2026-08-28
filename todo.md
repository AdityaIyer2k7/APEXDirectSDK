To add:
- ~~Axis::setMotor(bool on), ::getMotor()~~ *Done*
- ~~Axis::moveBy, ::moveTo~~ *Done*
- Axis::homeHardstop : Waiting on ModuSystems
- Axis::homeLimit : Waiting on ModuSystems
- ~~configYAML -> home_loc (in units), span_pos, span_neg (how far can you go + or -)~~ *Done*

- `fetch/forceCurrentLoc` -> `fetch/setLocation`
- `Transport` should do low-level commands, not `Axis`
- `Gantry` class: take X1...Y2, Z, Pan, Tilt, config them, move them simulataneously, and run trajectories & safety
- `SafetySystem` class?
