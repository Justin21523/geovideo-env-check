#include "src/app/MainWindow.h"

#include "src/app/AppLogger.h"
#include "src/app/AppSettings.h"
#include "src/app/AppInfo.h"
#include "src/app/widgets/EventTimelineWidget.h"
#include "src/app/widgets/PlaceholderWidget.h"
#include "src/app/widgets/SpatialViewWidget.h"
#include "src/app/widgets/VideoViewerWidget.h"

#include "src/core/domain/event/Event.h"
#include "src/core/domain/project/Project.h"
#include "src/core/domain/spatial/Camera.h"
#include "src/core/domain/spatial/Roi.h"
#include "src/core/domain/video/VideoSource.h"

#include <QAction>
#include <QDesktopServices>
#include <QDockWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QKeySequence>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>

namespace geovideo {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupWindow();
    createActions();
    createMenus();
    createToolBars();
    createCentralArea();
    createDockPanels();
    createStatusBar();
    initializeApplicationContext();
    restoreApplicationState();
    logDomainModelSkeleton();

    qInfo() << "Main window initialized.";
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    // 關閉前保存視窗與 dock 狀態，讓下一次啟動可以恢復工作環境。
    saveApplicationState();

    QMainWindow::closeEvent(event);
}

void MainWindow::setupWindow()
{
    // Dock 設定放在主視窗初始化階段，讓後續面板都能支援分頁與巢狀停駐。
    setDockOptions(
        QMainWindow::AnimatedDocks |
        QMainWindow::AllowNestedDocks |
        QMainWindow::AllowTabbedDocks
    );

    setWindowTitle(QStringLiteral("GeoVideo Evidence Viewer"));
    resize(1440, 900);
}

void MainWindow::createActions()
{
    // File 類動作先建立骨架，後續會逐步接上 project 與 video workflow。
    newProjectAction_ = new QAction(QStringLiteral("New Project"), this);
    newProjectAction_->setShortcut(QKeySequence::New);
    connect(newProjectAction_, &QAction::triggered, this, &MainWindow::handleNewProject);

    openProjectAction_ = new QAction(QStringLiteral("Open Project"), this);
    openProjectAction_->setShortcut(QKeySequence::Open);
    connect(openProjectAction_, &QAction::triggered, this, &MainWindow::handleOpenProject);

    importVideoAction_ = new QAction(QStringLiteral("Import Video"), this);
    connect(importVideoAction_, &QAction::triggered, this, &MainWindow::handleImportVideo);

    exportReportAction_ = new QAction(QStringLiteral("Export Report"), this);
    connect(exportReportAction_, &QAction::triggered, this, &MainWindow::handleExportReport);

    exitAction_ = new QAction(QStringLiteral("Exit"), this);
    exitAction_->setShortcut(QKeySequence::Quit);
    connect(exitAction_, &QAction::triggered, this, &QWidget::close);

    // Analysis 類動作目前只顯示狀態，下一階段才會加入實際分析管線。
    runAnalysisAction_ = new QAction(QStringLiteral("Run Motion Scan"), this);
    connect(runAnalysisAction_, &QAction::triggered, this, &MainWindow::handleRunAnalysis);

    // View 類動作用來恢復預設面板位置，避免開發過程中 layout 被拖亂。
    resetLayoutAction_ = new QAction(QStringLiteral("Reset Layout"), this);
    connect(resetLayoutAction_, &QAction::triggered, this, &MainWindow::handleResetLayout);

    openLogFolderAction_ = new QAction(QStringLiteral("Open Log Folder"), this);
    connect(openLogFolderAction_, &QAction::triggered, this, &MainWindow::handleOpenLogFolder);

    aboutAction_ = new QAction(QStringLiteral("About"), this);
    connect(aboutAction_, &QAction::triggered, this, &MainWindow::handleAbout);
}

void MainWindow::createMenus()
{
    auto* fileMenu = menuBar()->addMenu(QStringLiteral("&File"));
    fileMenu->addAction(newProjectAction_);
    fileMenu->addAction(openProjectAction_);
    fileMenu->addSeparator();
    fileMenu->addAction(importVideoAction_);
    fileMenu->addSeparator();
    fileMenu->addAction(exportReportAction_);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction_);

    auto* analysisMenu = menuBar()->addMenu(QStringLiteral("&Analysis"));
    analysisMenu->addAction(runAnalysisAction_);

    auto* viewMenu = menuBar()->addMenu(QStringLiteral("&View"));
    viewMenu->addAction(resetLayoutAction_);

    auto* helpMenu = menuBar()->addMenu(QStringLiteral("&Help"));
    helpMenu->addAction(openLogFolderAction_);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutAction_);
}

void MainWindow::createToolBars()
{
    auto* mainToolBar = addToolBar(QStringLiteral("Main Toolbar"));
    mainToolBar->setObjectName(QStringLiteral("MainToolbar"));
    mainToolBar->setMovable(false);

    // Toolbar 只放高頻操作，避免一開始就變成按鈕大雜燴。
    mainToolBar->addAction(newProjectAction_);
    mainToolBar->addAction(openProjectAction_);
    mainToolBar->addAction(importVideoAction_);
    mainToolBar->addSeparator();
    mainToolBar->addAction(runAnalysisAction_);
    mainToolBar->addAction(exportReportAction_);
}

void MainWindow::createCentralArea()
{
    // 中央區域固定保留給影片檢視，後續 ROI overlay、播放控制都會集中在這裡。
    videoViewer_ = new VideoViewerWidget(this);
    setCentralWidget(videoViewer_);
}

void MainWindow::createDockPanels()
{
    projectDock_ = createDockWidget(
        QStringLiteral("Project Explorer"),
        QStringLiteral("ProjectExplorerDock"),
        new PlaceholderWidget(
            QStringLiteral("Project Explorer"),
            QStringLiteral("Manage cases, cameras, videos, ROIs, and reports."),
            {
                QStringLiteral("Case workspace"),
                QStringLiteral("Camera sources"),
                QStringLiteral("Video files"),
                QStringLiteral("ROI rules"),
                QStringLiteral("Report items"),
            },
            this
        ),
        Qt::LeftDockWidgetArea
    );

    eventDetailDock_ = createDockWidget(
        QStringLiteral("Event Detail"),
        QStringLiteral("EventDetailDock"),
        new PlaceholderWidget(
            QStringLiteral("Event Detail"),
            QStringLiteral("Review selected event metadata, snapshot, tags, and notes."),
            {
                QStringLiteral("Event type"),
                QStringLiteral("Severity"),
                QStringLiteral("Camera and ROI"),
                QStringLiteral("Timestamp"),
                QStringLiteral("Review status"),
            },
            this
        ),
        Qt::RightDockWidgetArea
    );

    spatialDock_ = createDockWidget(
        QStringLiteral("Spatial View"),
        QStringLiteral("SpatialViewDock"),
        new SpatialViewWidget(this),
        Qt::RightDockWidgetArea
    );

    timelineDock_ = createDockWidget(
        QStringLiteral("Event Timeline"),
        QStringLiteral("EventTimelineDock"),
        new EventTimelineWidget(this),
        Qt::BottomDockWidgetArea
    );

    searchDock_ = createDockWidget(
        QStringLiteral("Event Search"),
        QStringLiteral("EventSearchDock"),
        new PlaceholderWidget(
            QStringLiteral("Event Search"),
            QStringLiteral("Search events by time, camera, ROI, severity, tags, and notes."),
            {
                QStringLiteral("Keyword query"),
                QStringLiteral("Time range filter"),
                QStringLiteral("ROI filter"),
                QStringLiteral("Review status"),
                QStringLiteral("Saved searches"),
            },
            this
        ),
        Qt::BottomDockWidgetArea
    );

    arrangeDefaultDockLayout();
}

void MainWindow::arrangeDefaultDockLayout()
{
    // Reset layout 與初次建立都共用這段，避免預設 dock 位置邏輯散落各處。
    projectDock_->show();
    eventDetailDock_->show();
    spatialDock_->show();
    timelineDock_->show();
    searchDock_->show();

    projectDock_->setFloating(false);
    eventDetailDock_->setFloating(false);
    spatialDock_->setFloating(false);
    timelineDock_->setFloating(false);
    searchDock_->setFloating(false);

    addDockWidget(Qt::LeftDockWidgetArea, projectDock_);
    addDockWidget(Qt::RightDockWidgetArea, eventDetailDock_);
    addDockWidget(Qt::RightDockWidgetArea, spatialDock_);
    addDockWidget(Qt::BottomDockWidgetArea, timelineDock_);
    addDockWidget(Qt::BottomDockWidgetArea, searchDock_);

    // 右側使用分頁，讓事件詳細資料與空間視覺化都保有完整閱讀空間。
    tabifyDockWidget(eventDetailDock_, spatialDock_);
    eventDetailDock_->raise();

    // 下方也使用分頁，避免 timeline 和 search table 同時擠壓中央影片區。
    tabifyDockWidget(timelineDock_, searchDock_);
    timelineDock_->raise();

    resizeDocks(QList<QDockWidget*>{projectDock_}, QList<int>{280}, Qt::Horizontal);
    resizeDocks(QList<QDockWidget*>{timelineDock_}, QList<int>{170}, Qt::Vertical);
}

void MainWindow::restoreApplicationState()
{
    if (!app_settings::shouldRestoreMainWindowLayout()) {
        qInfo() << "Main window layout restore disabled.";
        return;
    }

    const QByteArray geometry = app_settings::mainWindowGeometry();

    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    }

    const QByteArray state = app_settings::mainWindowState();

    if (state.isEmpty()) {
        qInfo() << "No saved main window layout found.";
        return;
    }

    const bool restored = restoreState(state, app_settings::MainWindowLayoutVersion);

    if (restored) {
        qInfo() << "Main window layout restored.";
        statusBar()->showMessage(QStringLiteral("Layout restored."), 2500);
        return;
    }

    qWarning() << "Saved main window layout could not be restored.";
    statusBar()->showMessage(QStringLiteral("Saved layout could not be restored."), 4000);
}

void MainWindow::saveApplicationState()
{
    if (!app_settings::shouldRestoreMainWindowLayout()) {
        qInfo() << "Main window layout save skipped.";
        return;
    }

    app_settings::saveMainWindowGeometry(saveGeometry());
    app_settings::saveMainWindowState(saveState(app_settings::MainWindowLayoutVersion));

    qInfo() << "Main window layout saved.";
}

std::optional<domain::Project> MainWindow::ensureWorkingProject()
{
    if (!appContext_.isInitialized()) {
        qWarning() << "Working project requested before application context was initialized.";
        return std::nullopt;
    }

    if (currentProjectId_.isValid()) {
        const auto existingProjectResult =
            appContext_.projectService().findProjectById(currentProjectId_);

        if (existingProjectResult.isSuccess() && existingProjectResult.value().has_value()) {
            return existingProjectResult.value();
        }

        if (existingProjectResult.isFailure()) {
            qWarning().noquote()
                << "Failed to load current project:"
                << existingProjectResult.errorMessage();
        }
    }

    // 使用者直接 import video 時，自動建立一個工作專案，避免 UI 流程卡住。
    const auto createProjectResult =
        appContext_.projectService().createProject(QStringLiteral("Untitled Case"));

    if (createProjectResult.isFailure()) {
        qWarning().noquote()
            << "Failed to create implicit working project:"
            << createProjectResult.errorMessage();

        return std::nullopt;
    }

    currentProjectId_ = createProjectResult.value().id();

    qInfo().noquote()
        << "Implicit working project created:"
        << createProjectResult.value().displayTitle()
        << "| id:"
        << currentProjectId_.toString();

    return createProjectResult.value();
}

void MainWindow::logDomainModelSkeleton() const
{
    // 建立一組暫時 domain 物件，確認 core module 已經能被 app 正常引用。
    const domain::Project sampleProject = domain::Project::create(QStringLiteral("Untitled Case"));
    const domain::VideoSource sampleVideo =
        domain::VideoSource::create(sampleProject.id(), QStringLiteral("sample.mp4"));
    const domain::Camera sampleCamera =
        domain::Camera::create(sampleProject.id(), QStringLiteral("Camera 01"));
    const domain::Roi sampleRoi =
        domain::Roi::create(sampleVideo.id(), QStringLiteral("Entry Zone"), domain::RoiType::Polygon);
    const domain::Event sampleEvent = domain::Event::create(
        sampleProject.id(),
        sampleVideo.id(),
        domain::EventType::MotionDetected,
        domain::TimeRange(1000, 2500)
    );

    qInfo().noquote()
        << "Domain model skeleton ready:"
        << sampleProject.displayTitle()
        << "|"
        << sampleVideo.displayName()
        << "|"
        << sampleCamera.displayName()
        << "|"
        << sampleRoi.displayName()
        << "|"
        << sampleEvent.title();
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(QStringLiteral("Ready"));
}

void MainWindow::initializeApplicationContext()
{
    // AppContext 統一管理 database、repository、service，MainWindow 只看初始化結果。
    if (!appContext_.initialize()) {
        qCritical().noquote()
            << "Application context initialization failed:"
            << appContext_.lastErrorMessage();

        statusBar()->showMessage(QStringLiteral("Application context initialization failed."), 5000);
        return;
    }

    statusBar()->showMessage(
        QStringLiteral("Application context ready. Schema v%1").arg(appContext_.schemaVersion()),
        3000
    );

    qInfo().noquote()
        << "Application context ready:"
        << appContext_.databasePath()
        << "| schema version:"
        << appContext_.schemaVersion();

    const auto projectsResult = appContext_.projectService().listProjects();

    if (projectsResult.isSuccess()) {
        qInfo() << "Project service ready. Existing project count:"
                << projectsResult.value().size();
        return;
    }

    qWarning().noquote()
        << "Project service smoke query failed:"
        << projectsResult.errorMessage();
}

QDockWidget* MainWindow::createDockWidget(
    const QString& title,
    const QString& objectName,
    QWidget* content,
    Qt::DockWidgetArea area
)
{
    auto* dock = new QDockWidget(title, this);
    dock->setObjectName(objectName);
    dock->setWidget(content);
    addDockWidget(area, dock);
    return dock;
}

void MainWindow::handleNewProject()
{
    if (!appContext_.isInitialized()) {
        qWarning() << "New project requested before application context was initialized.";
        statusBar()->showMessage(QStringLiteral("Application context is not ready."), 3000);
        return;
    }

    const auto result = appContext_.projectService().createProject(QStringLiteral("Untitled Case"));

    if (result.isFailure()) {
        qWarning().noquote() << "Failed to create project:" << result.errorMessage();
        statusBar()->showMessage(QStringLiteral("Failed to create project."), 4000);
        return;
    }

    const domain::Project project = result.value();
    currentProjectId_ = project.id();

    qInfo().noquote()
        << "New project created:"
        << project.displayTitle()
        << "| id:"
        << project.id().toString();

    videoViewer_->clearLoadedFile();

    statusBar()->showMessage(
        QStringLiteral("New project created: %1").arg(project.displayTitle()),
        3000
    );
}

void MainWindow::handleOpenProject()
{
    statusBar()->showMessage(QStringLiteral("Project loading will be implemented in a later phase."), 3000);
}

void MainWindow::handleImportVideo()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("Import Video"),
        QString(),
        QStringLiteral("Video Files (*.mp4 *.avi *.mov *.mkv);;All Files (*)")
    );

    if (filePath.isEmpty()) {
        statusBar()->showMessage(QStringLiteral("Video import cancelled."), 3000);
        return;
    }

    const std::optional<domain::Project> project = ensureWorkingProject();

    if (!project.has_value()) {
        statusBar()->showMessage(QStringLiteral("No project is available for video import."), 4000);
        return;
    }

    const auto result =
        appContext_.videoImportService().registerVideoSource(project->id(), filePath);

    if (result.isFailure()) {
        qWarning().noquote() << "Video import failed:" << result.errorMessage();
        statusBar()->showMessage(QStringLiteral("Video import failed."), 4000);
        return;
    }

    const domain::VideoSource videoSource = result.value();

    qInfo().noquote()
        << "Video source registered:"
        << videoSource.displayName()
        << "| id:"
        << videoSource.id().toString()
        << "| project:"
        << project->id().toString();

    videoViewer_->setLoadedVideoSource(videoSource);

    statusBar()->showMessage(
        QStringLiteral("Video source imported: %1 (%2)")
            .arg(videoSource.displayName())
            .arg(videoSource.resolutionText()),
        4000
    );
}

void MainWindow::handleRunAnalysis()
{
    qInfo() << "Run motion scan requested, but pipeline is not connected yet.";

    statusBar()->showMessage(QStringLiteral("Motion scan pipeline is not connected yet."), 3000);
}

void MainWindow::handleExportReport()
{
    statusBar()->showMessage(QStringLiteral("Report export will be implemented in a later phase."), 3000);
}

void MainWindow::handleResetLayout()
{
    // 清除已保存 layout，再恢復目前執行中的預設 dock 排列。
    app_settings::clearMainWindowLayout();
    arrangeDefaultDockLayout();

    qInfo() << "Main window layout reset.";

    statusBar()->showMessage(QStringLiteral("Layout reset."), 3000);
}

void MainWindow::handleOpenLogFolder()
{
    const QString path = app_logger::logFilePath();

    if (path.isEmpty()) {
        qWarning() << "Log file path is empty.";
        statusBar()->showMessage(QStringLiteral("Log file is not available."), 3000);
        return;
    }

    const QFileInfo logFileInfo(path);
    const QUrl folderUrl = QUrl::fromLocalFile(logFileInfo.absolutePath());

    if (!QDesktopServices::openUrl(folderUrl)) {
        qWarning().noquote() << "Failed to open log folder:" << logFileInfo.absolutePath();
        statusBar()->showMessage(QStringLiteral("Could not open log folder."), 3000);
        return;
    }

    qInfo().noquote() << "Opened log folder:" << logFileInfo.absolutePath();
}

void MainWindow::handleAbout()
{
    QMessageBox::about(
        this,
        QStringLiteral("About GeoVideo Evidence Viewer"),
        QStringLiteral(
            "<b>GeoVideo Evidence Viewer</b><br>"
            "Version %1<br><br>"
            "A C++ / Qt6 desktop application for video event retrieval, "
            "ROI-based review, and spatial visualization."
        )
            .arg(QString::fromUtf8(geovideo::app_info::Version))
    );
}

} // namespace geovideo
