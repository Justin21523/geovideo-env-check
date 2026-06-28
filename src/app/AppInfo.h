#pragma once

#ifndef GEOVIDEO_APP_VERSION
#define GEOVIDEO_APP_VERSION "0.1.0"
#endif

namespace geovideo::app_info {

// 集中管理應用程式名稱，避免各個視窗或對話框各自寫死字串。
inline constexpr const char* Name = "GeoVideo Evidence Viewer";

// 版本由 CMake 傳入，之後 README、About、log 都會維持一致。
inline constexpr const char* Version = GEOVIDEO_APP_VERSION;

// 組織名稱會影響 Qt 設定儲存位置，後續 QSettings 會用到。
inline constexpr const char* Organization = "PortfolioLab";

// 這段文字會出現在 About 對話框與 README 專案定位中。
inline constexpr const char* Description =
    "Desktop video event retrieval and spatial visualization tool.";

} // namespace geovideo::app_info