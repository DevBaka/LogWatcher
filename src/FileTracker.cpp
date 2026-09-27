#include "FileTracker.h"
#include "LogViewerWidget.h"

FileTracker::FileTracker(QObject *parent)
    : QObject(parent)
    , m_fileWatcher(nullptr)
    , m_refreshTimer(nullptr)
    , m_refreshInterval(1.0)
    , m_enabled(true)
{
    m_fileWatcher = new QFileSystemWatcher(this);
    connect(m_fileWatcher, &QFileSystemWatcher::fileChanged,
            this, &FileTracker::onFileChanged);
    
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, &FileTracker::onRefreshTimer);
}

FileTracker::~FileTracker()
{
    if (m_refreshTimer->isActive()) {
        m_refreshTimer->stop();
    }
}

void FileTracker::addFile(const QString &filePath, LogViewerWidget *viewer)
{
    if (!m_viewers.contains(filePath)) {
        m_fileWatcher->addPath(filePath);
        m_viewers[filePath] = viewer;
    }
}

void FileTracker::removeFile(const QString &filePath)
{
    if (m_viewers.contains(filePath)) {
        m_fileWatcher->removePath(filePath);
        m_viewers.remove(filePath);
    }
}

void FileTracker::setRefreshInterval(double seconds)
{
    m_refreshInterval = seconds;
    
    if (m_enabled && m_refreshTimer->isActive()) {
        m_refreshTimer->setInterval(static_cast<int>(m_refreshInterval * 1000));
    }
}

void FileTracker::setEnabled(bool enabled)
{
    m_enabled = enabled;
    
    if (m_enabled) {
        m_refreshTimer->start(static_cast<int>(m_refreshInterval * 1000));
    } else {
        m_refreshTimer->stop();
    }
}

void FileTracker::onFileChanged(const QString &path)
{
    if (m_viewers.contains(path)) {
        LogViewerWidget *viewer = m_viewers[path];
        if (viewer && m_enabled) {
            viewer->refreshFile();
        }
        emit fileChanged(path);
    }
}

void FileTracker::onRefreshTimer()
{
    if (!m_enabled) {
        return;
    }
    
    // Refresh all tracked files
    for (auto it = m_viewers.constBegin(); it != m_viewers.constEnd(); ++it) {
        LogViewerWidget *viewer = it.value();
        if (viewer) {
            viewer->refreshFile();
        }
    }
}
