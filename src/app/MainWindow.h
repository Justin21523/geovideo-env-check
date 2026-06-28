#pragma once

#include "src/app/AppContext.h"
#include "src/core/domain/common/EntityId.h"
#include "src/core/domain/project/Project.h"

#include <optional>
#include <QCloseEvent>
#include <QMainWindow>
#include <QString>
#include <Qt>

class QAction;
class QDockWidget;
class QWidget;


namespace geovideo {

class VideoViewerWidget;

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void handleNewProject();
    void handleOpenProject();
    void handleImportVideo();
    void handleRunAnalysis();
    void handleExportReport();
    void handleResetLayout();
    void handleAbout();
    void handleOpenLogFolder();

private:
    void setupWindow();
    void createActions();
    void createMenus();
    void createToolBars();
    void createCentralArea();
    void createDockPanels();
    void createStatusBar();
    void initializeApplicationContext();
    void arrangeDefaultDockLayout();
    void restoreApplicationState();
    void saveApplicationState();
    void logDomainModelSkeleton() const;

    std::optional<domain::Project> ensureWorkingProject();

    QDockWidget* createDockWidget(
        const QString& title,
        const QString& objectName,
        QWidget* content,
        Qt::DockWidgetArea area
    );

private:
    AppContext appContext_;
    domain::EntityId currentProjectId_;

    VideoViewerWidget* videoViewer_ = nullptr;

    QDockWidget* projectDock_ = nullptr;
    QDockWidget* eventDetailDock_ = nullptr;
    QDockWidget* spatialDock_ = nullptr;
    QDockWidget* timelineDock_ = nullptr;
    QDockWidget* searchDock_ = nullptr;

    QAction* newProjectAction_ = nullptr;
    QAction* openProjectAction_ = nullptr;
    QAction* importVideoAction_ = nullptr;
    QAction* runAnalysisAction_ = nullptr;
    QAction* exportReportAction_ = nullptr;
    QAction* resetLayoutAction_ = nullptr;
    QAction* aboutAction_ = nullptr;
    QAction* exitAction_ = nullptr;
    QAction* openLogFolderAction_ = nullptr;
};

} // namespace geovideo
