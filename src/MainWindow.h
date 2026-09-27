#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QSplitter>
#include <QStatusBar>
#include <QTimer>
#include <QDoubleSpinBox>
#include <QToolBar>
#include <QMenuBar>
#include <QAction>

class ProjectExplorer;
class LogViewerWidget;
class FileTracker;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openFolder();
    void openFile();
    void closeCurrentTab();
    void closeAllTabs();
    void refreshIntervalChanged(double value);
    void toggleAutoScroll(bool enabled);
    void toggleLiveTracking(bool enabled);
    void searchInCurrentTab();
    void updateStatusBar();

private:
    void setupUi();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void applyDarkTheme();
    void connectSignals();
    LogViewerWidget* createLogViewer(const QString &filePath);
    void updateRefreshStatus();

    // UI Components
    QSplitter *m_splitter;
    ProjectExplorer *m_projectExplorer;
    QTabWidget *m_tabWidget;
    QStatusBar *m_statusBar;
    
    // Toolbar controls
    QDoubleSpinBox *m_refreshIntervalSpinBox;
    QAction *m_autoScrollAction;
    QAction *m_liveTrackingAction;
    
    // File tracking
    FileTracker *m_fileTracker;
    
    // State
    double m_refreshInterval;
    bool m_autoScroll;
    bool m_liveTracking;
};

#endif // MAINWINDOW_H
