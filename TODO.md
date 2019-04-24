COMPLETE
========

X headwater_lcd moved into headwater_ui
X get multiplier_b working
X headwater_state moved out of headwater_api (only reference as a pointer)
X holding down rotary encoder button should execute changes immediately
X make atmega_io ns

NO parameterize / disable spi api in preprocessor
  - this isn't worth the savings right now. about 1k bytes program, 17 bytes data

NO remove button hold count
  - no real gains for removing it, could be useful

X move all the excess in atmega_headwater into appropriate modules

X move ui inputs into headwater_ui
  X struct of all inputs

NO all blocks in atmega_headwater (main, interrupts) should
  contain a single function call to a new central headwater module

  NO move remaining non-atmega specific code into headwater module
  X move remaining atmega specific code into other / new atmega modules

X stop on all mode changes may be a bit heavy...
  NO only stop on ext and midi?
  X does it need to stop at all? could just reset counts

X overflow issue where int playback stops after apx 65.535 seconds :p
  X play will no longer work until switch to ext and switch back

X loading a preset stops playback in internal mode

X bpm should be uint16 rather than int16

X multiplier rounding results in offset outputs from a and b that should be synced
  - ex. bpm = 135, a = 4, b = 8, outputs varying up to 2 samples (ms) apart

X multiplier set to 0 should not output ever (or minimum should be 1)
  - right now it behaves the same as 1, might be a ui / field minimum mismatch

X mode: internal
  X auto loop based on internal bpm
  X bpm UI enabled

X mode: external
  X don't auto loop based on internal bpm
  X after each play, re-calculate
    X bpm (for disiplay)
    X samples for multipliers

TODO HIGH
=========

- internal mode: only update clock speed (samples) after each beat

- mode: tap
  X auto loop based on internal bpm
  - after each play, re-calculate
    - bpm (for display and ongoing playback)
    - samples for multipliers

- mode: midi
  - calculate bpm from incoming midi clock
  - play every beat (1/24?) of incoming midi clock
  - passthrough to midi output?

- output midi clock (3rd x24 multiplier channel?) based on bpm

- ui range restriction stops display / value,
  but the uncommitted modifier keeps counting, which is bad
  - put min max on all fields?
  - store actual value instead of uncomitted modifier?
    - would have to use another method of detecting change...

TODO LOW
========

- external and midi modes: disable bpm in UI

- reset needs overhaul, play & reset are same now

- make lcd module work with various row / column sizes (specifically 8x2)

- should button change be handled in interrupt?

