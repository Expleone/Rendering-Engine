# Viewer

## Prerequisites (Linux)
Before compilation, the following dependencies must be installed on your system:

* `libgl-dev`
* `libvlc-dev`
* `vlc`
* `vlc-plugin-base`
* `vlc-plugin-video-output`

*(For Debian/Ubuntu-based distributions: `sudo apt install libgl-dev libvlc-dev vlc vlc-plugin-base vlc-plugin-video-output`)*

## Compilation
To configure and compile the project, execute the following commands from the project root directory:

```bash
cmake -B build
cmake --build build --config Release


