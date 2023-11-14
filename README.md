# TSM_AES_2023-2024_GG_MJ

Made by GRAC Guilhem and JUNOD Mickaël

Supervisied by AYER Serge

During the class AdvEmbSof - Advanced Embeded Software at HES-SO Master, Autumn 2023

## Project Description

This project's goal is to code a bike computer with elements like pedal rotation, speedometer, temperature, display, ...
Moreover this project will have to respect software quality constraints (CI/CD, cppcheck, ...).

## Disclaimer

The first 2 tests in `TESTS/bike-computer/bike-system/` on `src/static_scheduling/bike_system.cpp`, were failling each time just because of a small time deviation (was expecting 1600ms +/- 2ms, got 1602.005ms).
After showing this issue with the teacher, we agreed to change to offset from 2000ms to 2100ms.
