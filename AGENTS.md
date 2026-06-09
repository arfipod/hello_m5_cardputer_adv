# AGENTS.md

Instructions for Codex and other coding agents working in this repository.

- Do not use Arduino unless the user explicitly requests it.
- Keep hardware-specific code in `src/hardware`.
- Keep portable, host-testable code in `lib/`.
- Do not hardcode physical pins outside `src/hardware/BoardPins.hpp`.
- Prefer small, focused changes that preserve the template nature of the repo.
- Run validations after changes:
  - `pio test -e native`
  - `pio run -e cardputer_adv`
  - `pio run -e cardputer_adv_devkit_fallback`
- Update docs when architecture, build flags, pin mapping or validation flow changes.
- Do not add large dependencies without documenting why they are needed and whether they are required, optional or future-only.
- Do not copy the M5Stack UserDemo architecture wholesale. Use official examples only as hardware behavior references.
- Keep this repository general-purpose; do not turn it into a single application.
