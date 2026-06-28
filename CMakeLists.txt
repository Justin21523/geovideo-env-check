cmake_minimum_required(VERSION 3.22)

project(
    GeoVideoEvidenceViewer
    VERSION 0.1.0
    DESCRIPTION "Desktop video event retrieval and spatial visualization tool"
    LANGUAGES CXX
)

# 統一專案的 C++ 標準，避免不同檔案或模組使用不同語言版本。
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 匯出 compile_commands.json，讓 clangd / VS Code 可以取得完整編譯資訊。
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 讓 VS Code、clangd 和其他 C++ 工具固定讀專案根目錄的 compile_commands.json。
# 實際內容仍由目前 build preset 產生，這裡只建立穩定入口。
if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E create_symlink
            "${CMAKE_BINARY_DIR}/compile_commands.json"
            "${CMAKE_SOURCE_DIR}/compile_commands.json"
    )
endif()

# 啟用 Qt 常用的自動處理流程，後續 QObject、resource、ui 檔都能正常整合。
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 開發階段先打開警告，讓 C++ 問題越早暴露越好。
option(GEOVIDEO_ENABLE_WARNINGS "Enable compiler warnings" ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Multimedia
    OpenGLWidgets
)

# OpenCV 後續會負責影片 frame analysis、motion detection、snapshot 等功能。
find_package(OpenCV REQUIRED)

# SQLite 負責 project、video source、ROI、event、report item 等資料。
find_package(SQLite3 REQUIRED)

# 專案所有程式碼都放在 src/ 底下，新增檔案後 CMake 會自動重新掃描。
file(GLOB_RECURSE GEOVIDEO_SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

add_executable(GeoVideoEvidenceViewer
    ${GEOVIDEO_SOURCE_FILES}
)

target_link_libraries(GeoVideoEvidenceViewer PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::OpenGLWidgets
    ${OpenCV_LIBS}
    SQLite::SQLite3
)

target_include_directories(GeoVideoEvidenceViewer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

target_compile_definitions(GeoVideoEvidenceViewer PRIVATE
    GEOVIDEO_APP_VERSION="${PROJECT_VERSION}"
)

# 將執行檔集中輸出到 build/bin，讓執行指令固定且好記。
set_target_properties(GeoVideoEvidenceViewer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

if(GEOVIDEO_ENABLE_WARNINGS)
    if(MSVC)
        target_compile_options(GeoVideoEvidenceViewer PRIVATE /W4 /permissive-)
    else()
        target_compile_options(GeoVideoEvidenceViewer PRIVATE
            -Wall
            -Wextra
            -Wpedantic
        )
    endif()
endif()
