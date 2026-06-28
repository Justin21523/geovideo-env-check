# Coding Agent Instructions

- Keep the root `CMakeLists.txt` as the single source of build configuration.
- Put project code under `src/`.
- New `.cpp`, `.h`, and `.hpp` files under `src/` are picked up automatically by the root CMake file through `file(GLOB_RECURSE ... CONFIGURE_DEPENDS ...)`.
- Do not manually append new source files to `add_executable()` unless the project intentionally moves away from automatic source discovery.
- Project-local includes must start at the repository root, for example:
  - `#include "src/app/MainWindow.h"`
  - `#include "src/core/domain/event/Event.h"`
- Keep `target_include_directories()` pointed at `${CMAKE_CURRENT_SOURCE_DIR}` so `src/...` includes resolve consistently.
- Keep `CMAKE_EXPORT_COMPILE_COMMANDS` enabled.
- The root `compile_commands.json` is a CMake-created symlink to the active build directory. Do not edit it manually.
- VS Code settings should stay generic and rely on CMake Tools plus `compile_commands.json`; avoid hardcoded project-specific Qt/OpenCV/include paths in `.vscode`.
- Keep IntelliSense error squiggles disabled in `.vscode/settings.json`; use CMake configure/build output as the source of truth for real C++ errors. This avoids false red underlines from stale VS Code caches or partial CMake configuration.
