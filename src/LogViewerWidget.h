#ifndef LOGVIEWERWIDGET_H
#define LOGVIEWERWIDGET_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QTextCharFormat>
#include <QTimer>
#include <QFile>
#include <QFileSystemWatcher>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>

class LogViewerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LogViewerWidget(const QString &filePath, QWidget *parent = nullptr);
    ~LogViewerWidget();

    QString filePath() const { return m_filePath; }
    QString fileName() const { return m_fileName; }
    bool isModified() const { return m_isModified; }
    void setAutoScroll(bool enabled);
    void setLiveTracking(bool enabled);
    void setRefreshInterval(double seconds);
    bool autoScroll() const { return m_autoScroll; }
    bool liveTracking() const { return m_liveTracking; }
    
    void search(const QString &text, bool caseSensitive = false);
    void searchNext();
    void searchPrevious();

signals:
    void fileChanged(const QString &filePath);
    void refreshStatusChanged(const QString &status);

public slots:
    void refreshFile();
    void reloadFile();

private slots:
    void onFileChanged(const QString &path);
    void onRefreshTimer();
    void onSearchTextChanged();
    void onSearchNextClicked();
    void onSearchPreviousClicked();
    void onCaseSensitiveToggled(bool checked);

private:
    void setupUi();
    void setupSyntaxHighlighting();
    void loadFile();
    void appendNewContent();
    void highlightLine(int lineNumber);
    void applySyntaxHighlighting(const QString &line);
    QTextCharFormat getFormatForLevel(const QString &level);
    QString detectLogLevel(const QString &line);
    void updateLineNumbers();
    
    // UI Components
    QPlainTextEdit *m_textEdit;
    QLineEdit *m_searchEdit;
    QPushButton *m_searchNextButton;
    QPushButton *m_searchPreviousButton;
    QCheckBox *m_caseSensitiveCheckBox;
    QLabel *m_lineCountLabel;
    QWidget *m_searchWidget;
    
    // File handling
    QString m_filePath;
    QString m_fileName;
    QFile m_file;
    QFileSystemWatcher *m_fileWatcher;
    qint64 m_lastFileSize;
    qint64 m_lastFilePosition;
    
    // Refresh timer
    QTimer *m_refreshTimer;
    
    // State
    bool m_autoScroll;
    bool m_liveTracking;
    bool m_isModified;
    double m_refreshInterval;
    QString m_lastSearchText;
    bool m_lastCaseSensitive;
    
    // Syntax highlighting formats
    QTextCharFormat m_errorFormat;
    QTextCharFormat m_warningFormat;
    QTextCharFormat m_infoFormat;
    QTextCharFormat m_debugFormat;
    QTextCharFormat m_defaultFormat;
};

#endif // LOGVIEWERWIDGET_H
