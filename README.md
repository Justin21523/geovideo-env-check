# GeoVideo Evidence Viewer

GeoVideo Evidence Viewer is a C++ / Qt6 desktop application for reviewing video footage, indexing motion-related events, configuring spatial regions of interest, and visualizing incidents on a 2D/3D field map.

The project is designed as a portfolio-grade desktop tool for smart city, campus safety, facility monitoring, traffic technology, security review, and government-style field management workflows.

## Project Status

Current phase: **Phase 0.2 - Project Skeleton**

Implemented:

- CMake-based C++20 project structure
- Qt6 Widgets desktop application shell
- Dock-based workspace layout
- Central video viewer placeholder
- Event timeline placeholder
- Event detail placeholder
- Event search placeholder
- OpenGL spatial view placeholder
- clang-format configuration
- Initial README

Not implemented yet:

- Real video playback
- OpenCV frame extraction
- ROI editor
- Motion detection
- SQLite event database
- Evidence export

## Tech Stack

- C++20
- Qt6 Widgets
- Qt6 Multimedia
- Qt6 OpenGLWidgets
- OpenCV
- SQLite
- CMake
- Ninja

## Planned Core Features

- Video import and playback
- Frame stepping and timestamp navigation
- ROI editor for rectangle, polygon, and line-crossing rules
- Motion detection with OpenCV
- Event timeline and event review workflow
- SQLite-based event indexing
- Search by time, camera, ROI, severity, tags, and notes
- Snapshot and clip export
- 2D/3D spatial visualization with OpenGL
- Case report generation

## Build Requirements

Ubuntu packages used during development:

```bash
sudo apt install -y \
  build-essential \
  cmake \
  ninja-build \
  git \
  pkg-config \
  qt6-base-dev \
  qt6-base-dev-tools \
  qt6-tools-dev \
  qt6-tools-dev-tools \
  qt6-multimedia-dev \
  libqt6opengl6-dev \
  libopencv-dev \
  sqlite3 \
  libsqlite3-dev