# Changelog

## v0.2.0

* Bug fixes
  * Fixed Raspberry Pi/Chromium/Qt bug that caused touch events to be lost when
    there's a network connection, but no Internet.
  * Improve handling of C++ code exits

* New features
  * Support running under a non-root account. This is required now.
  * Improve default homepage
  * Add `set_zoom/2` to change the zoom level
  * Add helpers for setting permissions on the Raspberry Pi. Ideally these would
    move out of this project, but they're too convenient for now.

## v0.1.0

Initial release

