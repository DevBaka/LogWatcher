#ifndef FILETRACKER_H
#define FILETRACKER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QFileSystemWatcher>
#include <QTimer>

class LogViewerWidget;

class FileTracker : public QObject
{
    Q_OBJECT

public:
    explicit FileTracker(QObject *parent = nullptr);
    ~FileTracker();

    void addFile(const QString &filePath, LogViewerWidget *viewer);
    void removeFile(const QString &filePath);
    void setRefreshInterval(double seconds);
    void setEnabled(bool enabled);

signals:
    void fileChanged(const QString &filePath);

private slots:
    void onFileChanged(const QString &path);
    void onRefreshTimer();

private:
    QFileSystemWatcher *m_fileWatcher;
    QTimer *m_refreshTimer;
    QMap<QString, LogViewerWidget*> m_viewers;
    double m_refreshInterval;
    bool m_enabled;
};

#endif // FILETRACKER_H
