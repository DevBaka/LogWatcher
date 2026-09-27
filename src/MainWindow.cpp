#include "MainWindow.h"
#include "ProjectExplorer.h"
#include "LogViewerWidget.h"
#include "FileTracker.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QStyleFactory>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_splitter(nullptr)
    , m_projectExplorer(nullptr)
    , m_tabWidget(nullptr)
    , m_statusBar(nullptr)
    , m_refreshIntervalSpinBox(nullptr)
    , m_autoScrollAction(nullptr)
    , m_liveTrackingAction(nullptr)
    , m_fileTracker(nullptr)
    , m_refreshInterval(1.0)
    , m_autoScroll(true)
    , m_liveTracking(true)
{
    setupUi();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    applyDarkTheme();
    connectSignals();
    
    resize(1200, 800);
    setWindowTitle("LogWatcher");
}

MainWindow::~MainWindow()
{
    delete m_fileTracker;
}

void MainWindow::setupUi()
{
    // Create central widget and splitter
    m_splitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(m_splitter);
    
    // Create project explorer
    m_projectExplorer = new ProjectExplorer(this);
    m_splitter->addWidget(m_projectExplorer);
    
    // Create tab widget
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);
    m_tabWidget->setDocumentMode(true);
    m_splitter->addWidget(m_tabWidget);
    
    // Set splitter sizes
    m_splitter->setStretchFactor(0, 1);
    m_splitter->setStretchFactor(1, 3);
    m_splitter->setSizes({300, 900});
    
    // Create file tracker
    m_fileTracker = new FileTracker(this);
    m_fileTracker->setRefreshInterval(m_refreshInterval);
    m_fileTracker->setEnabled(m_liveTracking);
}

void MainWindow::setupMenuBar()
{
    QMenuBar *menuBar = this->menuBar();
    
    // File menu
    QMenu *fileMenu = menuBar->addMenu("&File");
    
    QAction *openFolderAction = fileMenu->addAction("Open &Folder");
    openFolderAction->setShortcut(QKeySequence("Ctrl+Shift+O"));
    connect(openFolderAction, &QAction::triggered, this, &MainWindow::openFolder);
    
    QAction *openFileAction = fileMenu->addAction("&Open File");
    openFileAction->setShortcut(QKeySequence("Ctrl+O"));
    connect(openFileAction, &QAction::triggered, this, &MainWindow::openFile);
    
    fileMenu->addSeparator();
    
    QAction *closeTabAction = fileMenu->addAction("Close &Tab");
    closeTabAction->setShortcut(QKeySequence("Ctrl+W"));
    connect(closeTabAction, &QAction::triggered, this, &MainWindow::closeCurrentTab);
    
    QAction *closeAllTabsAction = fileMenu->addAction("Close &All Tabs");
    connect(closeAllTabsAction, &QAction::triggered, this, &MainWindow::closeAllTabs);
    
    fileMenu->addSeparator();
    
    QAction *exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QKeySequence("Ctrl+Q"));
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    
    // View menu
    QMenu *viewMenu = menuBar->addMenu("&View");
    
    m_autoScrollAction = viewMenu->addAction("Auto &Scroll");
    m_autoScrollAction->setCheckable(true);
    m_autoScrollAction->setChecked(m_autoScroll);
    connect(m_autoScrollAction, &QAction::toggled, this, &MainWindow::toggleAutoScroll);
    
    m_liveTrackingAction = viewMenu->addAction("&Live Tracking");
    m_liveTrackingAction->setCheckable(true);
    m_liveTrackingAction->setChecked(m_liveTracking);
    connect(m_liveTrackingAction, &QAction::toggled, this, &MainWindow::toggleLiveTracking);
    
    // Search menu
    QMenu *searchMenu = menuBar->addMenu("&Search");
    
    QAction *searchAction = searchMenu->addAction("&Search in Current Tab");
    searchAction->setShortcut(QKeySequence("Ctrl+F"));
    connect(searchAction, &QAction::triggered, this, &MainWindow::searchInCurrentTab);
}

void MainWindow::setupToolBar()
{
    QToolBar *toolBar = addToolBar("Main Toolbar");
    
    // Open folder button
    QAction *openFolderAction = toolBar->addAction("Open Folder");
    connect(openFolderAction, &QAction::triggered, this, &MainWindow::openFolder);
    
    // Open file button
    QAction *openFileAction = toolBar->addAction("Open File");
    connect(openFileAction, &QAction::triggered, this, &MainWindow::openFile);
    
    toolBar->addSeparator();
    
    // Refresh interval label
    QLabel *refreshLabel = new QLabel("Refresh (s):", this);
    toolBar->addWidget(refreshLabel);
    
    // Refresh interval spinbox
    m_refreshIntervalSpinBox = new QDoubleSpinBox(this);
    m_refreshIntervalSpinBox->setRange(0.01, 3600.0);
    m_refreshIntervalSpinBox->setSingleStep(0.25);
    m_refreshIntervalSpinBox->setDecimals(2);
    m_refreshIntervalSpinBox->setValue(m_refreshInterval);
    m_refreshIntervalSpinBox->setToolTip("Refresh interval in seconds");
    connect(m_refreshIntervalSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::refreshIntervalChanged);
    toolBar->addWidget(m_refreshIntervalSpinBox);
    
    toolBar->addSeparator();
    
    // Auto scroll button
    m_autoScrollAction = toolBar->addAction("Auto Scroll");
    m_autoScrollAction->setCheckable(true);
    m_autoScrollAction->setChecked(m_autoScroll);
    connect(m_autoScrollAction, &QAction::toggled, this, &MainWindow::toggleAutoScroll);
    
    // Live tracking button
    m_liveTrackingAction = toolBar->addAction("Live Tracking");
    m_liveTrackingAction->setCheckable(true);
    m_liveTrackingAction->setChecked(m_liveTracking);
    connect(m_liveTrackingAction, &QAction::toggled, this, &MainWindow::toggleLiveTracking);
    
    toolBar->addSeparator();
    
    // Search button
    QAction *searchAction = toolBar->addAction("Search");
    connect(searchAction, &QAction::triggered, this, &MainWindow::searchInCurrentTab);
}

void MainWindow::setupStatusBar()
{
    m_statusBar = statusBar();
    m_statusBar->showMessage("Ready");
}

void MainWindow::applyDarkTheme()
{
    qApp->setStyle(QStyleFactory::create("Fusion"));
    
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    
    qApp->setPalette(darkPalette);
    
    // Additional styling for specific widgets
    QString styleSheet = R"(
        QTabWidget::pane {
            border: 1px solid #444;
            background: #252525;
        }
        QTabBar::tab {
            background: #353535;
            color: white;
            padding: 8px 12px;
            border: 1px solid #444;
            border-bottom: none;
        }
        QTabBar::tab:selected {
            background: #252525;
            border-bottom: 1px solid #252525;
        }
        QTabBar::tab:hover {
            background: #404040;
        }
        QSplitter::handle {
            background: #444;
        }
        QSplitter::handle:hover {
            background: #555;
        }
        QStatusBar {
            background: #353535;
            color: white;
        }
        QToolBar {
            background: #353535;
            border: none;
        }
        QMenuBar {
            background: #353535;
            color: white;
        }
        QMenuBar::item:selected {
            background: #404040;
        }
        QMenu {
            background: #353535;
            color: white;
            border: 1px solid #444;
        }
        QMenu::item:selected {
            background: #404040;
        }
        QTreeView {
            background: #252525;
            color: white;
            border: 1px solid #444;
        }
        QTreeView::item:selected {
            background: #2a82da;
            color: black;
        }
        QDoubleSpinBox {
            background: #252525;
            color: white;
            border: 1px solid #444;
            padding: 2px;
        }
        QLineEdit {
            background: #252525;
            color: white;
            border: 1px solid #444;
            padding: 2px;
        }
    )";
    
    qApp->setStyleSheet(styleSheet);
}

void MainWindow::connectSignals()
{
    connect(m_projectExplorer, &ProjectExplorer::fileOpened,
            this, [this](const QString &filePath) {
                createLogViewer(filePath);
            });
    
    connect(m_tabWidget, &QTabWidget::tabCloseRequested,
            this, [this](int index) {
                QWidget *widget = m_tabWidget->widget(index);
                LogViewerWidget *viewer = qobject_cast<LogViewerWidget*>(widget);
                if (viewer) {
                    m_fileTracker->removeFile(viewer->filePath());
                }
                m_tabWidget->removeTab(index);
                updateStatusBar();
            });
    
    connect(m_tabWidget, &QTabWidget::currentChanged,
            this, [this](int index) {
                updateStatusBar();
            });
}

void MainWindow::openFolder()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, "Select Folder", "");
    if (!folderPath.isEmpty()) {
        m_projectExplorer->setRootPath(folderPath);
        m_statusBar->showMessage("Opened folder: " + folderPath);
    }
}

void MainWindow::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Open Log File",
        "",
        "Log Files (*.log *.txt);;All Files (*)"
    );
    
    if (!filePath.isEmpty()) {
        createLogViewer(filePath);
    }
}

void MainWindow::closeCurrentTab()
{
    int currentIndex = m_tabWidget->currentIndex();
    if (currentIndex >= 0) {
        QWidget *widget = m_tabWidget->widget(currentIndex);
        LogViewerWidget *viewer = qobject_cast<LogViewerWidget*>(widget);
        if (viewer) {
            m_fileTracker->removeFile(viewer->filePath());
        }
        m_tabWidget->removeTab(currentIndex);
        updateStatusBar();
    }
}

void MainWindow::closeAllTabs()
{
    while (m_tabWidget->count() > 0) {
        QWidget *widget = m_tabWidget->widget(0);
        LogViewerWidget *viewer = qobject_cast<LogViewerWidget*>(widget);
        if (viewer) {
            m_fileTracker->removeFile(viewer->filePath());
        }
        m_tabWidget->removeTab(0);
    }
    updateStatusBar();
}

void MainWindow::refreshIntervalChanged(double value)
{
    m_refreshInterval = value;
    m_fileTracker->setRefreshInterval(m_refreshInterval);
    updateStatusBar();
}

void MainWindow::toggleAutoScroll(bool enabled)
{
    m_autoScroll = enabled;
    
    // Update all viewers
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        QWidget *widget = m_tabWidget->widget(i);
        LogViewerWidget *viewer = qobject_cast<LogViewerWidget*>(widget);
        if (viewer) {
            viewer->setAutoScroll(m_autoScroll);
        }
    }
}

void MainWindow::toggleLiveTracking(bool enabled)
{
    m_liveTracking = enabled;
    m_fileTracker->setEnabled(m_liveTracking);
    updateStatusBar();
}

void MainWindow::searchInCurrentTab()
{
    int currentIndex = m_tabWidget->currentIndex();
    if (currentIndex >= 0) {
        QWidget *widget = m_tabWidget->widget(currentIndex);
        LogViewerWidget *viewer = qobject_cast<LogViewerWidget*>(widget);
        if (viewer) {
            viewer->search("");
        }
    }
}

void MainWindow::updateStatusBar()
{
    int currentIndex = m_tabWidget->currentIndex();
    if (currentIndex >= 0) {
        QWidget *widget = m_tabWidget->widget(currentIndex);
        LogViewerWidget *viewer = qobject_cast<LogViewerWidget*>(widget);
        if (viewer) {
            QString status = QString("File: %1 | Tabs: %2 | Refresh: %3s | Live: %4 | Auto-Scroll: %5")
                .arg(viewer->fileName())
                .arg(m_tabWidget->count())
                .arg(m_refreshInterval, 0, 'f', 2)
                .arg(m_liveTracking ? "ON" : "OFF")
                .arg(m_autoScroll ? "ON" : "OFF");
            m_statusBar->showMessage(status);
        }
    } else {
        QString status = QString("Tabs: %1 | Refresh: %2s | Live: %3 | Auto-Scroll: %4")
            .arg(m_tabWidget->count())
            .arg(m_refreshInterval, 0, 'f', 2)
            .arg(m_liveTracking ? "ON" : "OFF")
            .arg(m_autoScroll ? "ON" : "OFF");
        m_statusBar->showMessage(status);
    }
}

void MainWindow::updateRefreshStatus()
{
    updateStatusBar();
}

LogViewerWidget* MainWindow::createLogViewer(const QString &filePath)
{
    // Check if file is already open
    for (int i = 0; i < m_tabWidget->count(); ++i) {
        QWidget *widget = m_tabWidget->widget(i);
        LogViewerWidget *viewer = qobject_cast<LogViewerWidget*>(widget);
        if (viewer && viewer->filePath() == filePath) {
            m_tabWidget->setCurrentIndex(i);
            return viewer;
        }
    }
    
    // Create new viewer
    LogViewerWidget *viewer = new LogViewerWidget(filePath, this);
    viewer->setAutoScroll(m_autoScroll);
    viewer->setLiveTracking(m_liveTracking);
    viewer->setRefreshInterval(m_refreshInterval);
    
    // Add to file tracker
    m_fileTracker->addFile(filePath, viewer);
    
    // Add to tab widget
    QString fileName = QFileInfo(filePath).fileName();
    m_tabWidget->addTab(viewer, fileName);
    m_tabWidget->setCurrentIndex(m_tabWidget->count() - 1);
    
    connect(viewer, &LogViewerWidget::refreshStatusChanged,
            this, [this](const QString &status) {
                updateStatusBar();
            });
    
    updateStatusBar();
    return viewer;
}
