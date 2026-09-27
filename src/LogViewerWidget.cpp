#include "LogViewerWidget.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextCursor>
#include <QTextBlock>
#include <QScrollBar>
#include <QFileInfo>
#include <QMessageBox>
#include <QRegularExpression>

LogViewerWidget::LogViewerWidget(const QString &filePath, QWidget *parent)
    : QWidget(parent)
    , m_textEdit(nullptr)
    , m_searchEdit(nullptr)
    , m_searchNextButton(nullptr)
    , m_searchPreviousButton(nullptr)
    , m_caseSensitiveCheckBox(nullptr)
    , m_lineCountLabel(nullptr)
    , m_searchWidget(nullptr)
    , m_filePath(filePath)
    , m_fileName(QFileInfo(filePath).fileName())
    , m_file(filePath)
    , m_fileWatcher(nullptr)
    , m_lastFileSize(0)
    , m_lastFilePosition(0)
    , m_refreshTimer(nullptr)
    , m_autoScroll(true)
    , m_liveTracking(true)
    , m_isModified(false)
    , m_refreshInterval(1.0)
    , m_lastCaseSensitive(false)
{
    setupUi();
    setupSyntaxHighlighting();
    loadFile();
}

LogViewerWidget::~LogViewerWidget()
{
    if (m_fileWatcher) {
        delete m_fileWatcher;
    }
    if (m_refreshTimer) {
        delete m_refreshTimer;
    }
}

void LogViewerWidget::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Search widget
    m_searchWidget = new QWidget(this);
    QHBoxLayout *searchLayout = new QHBoxLayout(m_searchWidget);
    searchLayout->setContentsMargins(5, 5, 5, 5);
    
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Search...");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &LogViewerWidget::onSearchTextChanged);
    searchLayout->addWidget(m_searchEdit);
    
    m_caseSensitiveCheckBox = new QCheckBox("Case Sensitive", this);
    connect(m_caseSensitiveCheckBox, &QCheckBox::toggled, this, &LogViewerWidget::onCaseSensitiveToggled);
    searchLayout->addWidget(m_caseSensitiveCheckBox);
    
    m_searchPreviousButton = new QPushButton("Previous", this);
    connect(m_searchPreviousButton, &QPushButton::clicked, this, &LogViewerWidget::onSearchPreviousClicked);
    searchLayout->addWidget(m_searchPreviousButton);
    
    m_searchNextButton = new QPushButton("Next", this);
    connect(m_searchNextButton, &QPushButton::clicked, this, &LogViewerWidget::onSearchNextClicked);
    searchLayout->addWidget(m_searchNextButton);
    
    m_searchWidget->hide();
    mainLayout->addWidget(m_searchWidget);
    
    // Text edit
    m_textEdit = new QPlainTextEdit(this);
    m_textEdit->setReadOnly(true);
    m_textEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
    m_textEdit->setFont(QFont("Consolas", 9));
    mainLayout->addWidget(m_textEdit);
    
    // Line count label
    m_lineCountLabel = new QLabel("Lines: 0", this);
    mainLayout->addWidget(m_lineCountLabel);
    
    // File watcher
    m_fileWatcher = new QFileSystemWatcher(this);
    m_fileWatcher->addPath(m_filePath);
    connect(m_fileWatcher, &QFileSystemWatcher::fileChanged,
            this, &LogViewerWidget::onFileChanged);
    
    // Refresh timer
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, &LogViewerWidget::onRefreshTimer);
}

void LogViewerWidget::setupSyntaxHighlighting()
{
    // Error format - red
    m_errorFormat.setForeground(QColor(255, 100, 100));
    m_errorFormat.setFontWeight(QFont::Bold);
    
    // Warning format - yellow/orange
    m_warningFormat.setForeground(QColor(255, 200, 100));
    m_warningFormat.setFontWeight(QFont::Bold);
    
    // Info format - cyan/light blue
    m_infoFormat.setForeground(QColor(100, 200, 255));
    
    // Debug format - gray
    m_debugFormat.setForeground(QColor(150, 150, 150));
    
    // Default format - white
    m_defaultFormat.setForeground(Qt::white);
}

void LogViewerWidget::loadFile()
{
    if (!m_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not open file: " + m_filePath);
        return;
    }
    
    // Read file in chunks for large files
    const qint64 chunkSize = 1024 * 1024; // 1MB chunks
    QTextStream stream(&m_file);
    stream.setEncoding(QStringConverter::Utf8);
    
    QString content;
    qint64 totalRead = 0;
    
    while (!stream.atEnd()) {
        QString chunk = stream.read(chunkSize);
        content.append(chunk);
        totalRead += chunk.size();
        
        // Update UI periodically for large files
        if (totalRead % (10 * chunkSize) == 0) {
            QApplication::processEvents();
        }
    }
    
    m_textEdit->setPlainText(content);
    m_lastFileSize = m_file.size();
    m_lastFilePosition = m_file.size();
    
    m_file.close();
    
    updateLineNumbers();
    
    if (m_autoScroll) {
        m_textEdit->moveCursor(QTextCursor::End);
    }
}

void LogViewerWidget::appendNewContent()
{
    if (!m_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    
    // Seek to last known position
    if (!m_file.seek(m_lastFilePosition)) {
        m_file.close();
        return;
    }
    
    QTextStream stream(&m_file);
    stream.setEncoding(QStringConverter::Utf8);
    
    QString newContent = stream.readAll();
    
    if (!newContent.isEmpty()) {
        QTextCursor cursor = m_textEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        cursor.insertText(newContent);
        
        m_lastFilePosition = m_file.pos();
        m_lastFileSize = m_file.size();
        
        updateLineNumbers();
        
        if (m_autoScroll) {
            m_textEdit->moveCursor(QTextCursor::End);
            m_textEdit->verticalScrollBar()->setValue(
                m_textEdit->verticalScrollBar()->maximum()
            );
        }
    }
    
    m_file.close();
}

void LogViewerWidget::refreshFile()
{
    appendNewContent();
    emit refreshStatusChanged("Refreshed");
}

void LogViewerWidget::reloadFile()
{
    m_textEdit->clear();
    m_lastFilePosition = 0;
    loadFile();
    emit refreshStatusChanged("Reloaded");
}

void LogViewerWidget::onFileChanged(const QString &path)
{
    Q_UNUSED(path);
    
    if (m_liveTracking) {
        appendNewContent();
    }
}

void LogViewerWidget::onRefreshTimer()
{
    if (m_liveTracking) {
        appendNewContent();
    }
}

void LogViewerWidget::setAutoScroll(bool enabled)
{
    m_autoScroll = enabled;
}

void LogViewerWidget::setLiveTracking(bool enabled)
{
    m_liveTracking = enabled;
    
    if (m_liveTracking) {
        m_refreshTimer->start(static_cast<int>(m_refreshInterval * 1000));
    } else {
        m_refreshTimer->stop();
    }
}

void LogViewerWidget::setRefreshInterval(double seconds)
{
    m_refreshInterval = seconds;
    
    if (m_liveTracking && m_refreshTimer->isActive()) {
        m_refreshTimer->setInterval(static_cast<int>(m_refreshInterval * 1000));
    }
}

void LogViewerWidget::search(const QString &text, bool caseSensitive)
{
    if (m_searchWidget->isHidden()) {
        m_searchWidget->show();
    }
    
    m_searchEdit->setText(text);
    m_caseSensitiveCheckBox->setChecked(caseSensitive);
    
    if (!text.isEmpty()) {
        onSearchNextClicked();
    }
}

void LogViewerWidget::searchNext()
{
    onSearchNextClicked();
}

void LogViewerWidget::searchPrevious()
{
    onSearchPreviousClicked();
}

void LogViewerWidget::onSearchTextChanged()
{
    QString text = m_searchEdit->text();
    if (!text.isEmpty()) {
        onSearchNextClicked();
    }
}

void LogViewerWidget::onSearchNextClicked()
{
    QString searchText = m_searchEdit->text();
    if (searchText.isEmpty()) {
        return;
    }
    
    bool caseSensitive = m_caseSensitiveCheckBox->isChecked();
    QTextDocument::FindFlags flags;
    
    if (caseSensitive) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    
    QTextCursor cursor = m_textEdit->textCursor();
    
    // If we have a selection, start searching from after it
    if (cursor.hasSelection()) {
        cursor.setPosition(cursor.selectionEnd());
    }
    
    bool found = m_textEdit->find(searchText, flags);
    
    if (!found) {
        // Wrap around to the beginning
        cursor.movePosition(QTextCursor::Start);
        m_textEdit->setTextCursor(cursor);
        found = m_textEdit->find(searchText, flags);
    }
    
    if (found) {
        highlightLine(m_textEdit->textCursor().blockNumber());
    }
}

void LogViewerWidget::onSearchPreviousClicked()
{
    QString searchText = m_searchEdit->text();
    if (searchText.isEmpty()) {
        return;
    }
    
    bool caseSensitive = m_caseSensitiveCheckBox->isChecked();
    QTextDocument::FindFlags flags;
    flags |= QTextDocument::FindBackward;
    
    if (caseSensitive) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    
    QTextCursor cursor = m_textEdit->textCursor();
    
    // If we have a selection, start searching from before it
    if (cursor.hasSelection()) {
        cursor.setPosition(cursor.selectionStart());
    }
    
    bool found = m_textEdit->find(searchText, flags);
    
    if (!found) {
        // Wrap around to the end
        cursor.movePosition(QTextCursor::End);
        m_textEdit->setTextCursor(cursor);
        found = m_textEdit->find(searchText, flags);
    }
    
    if (found) {
        highlightLine(m_textEdit->textCursor().blockNumber());
    }
}

void LogViewerWidget::onCaseSensitiveToggled(bool checked)
{
    Q_UNUSED(checked);
    if (!m_searchEdit->text().isEmpty()) {
        onSearchNextClicked();
    }
}

void LogViewerWidget::highlightLine(int lineNumber)
{
    QTextCursor cursor(m_textEdit->document()->findBlockByNumber(lineNumber));
    m_textEdit->setTextCursor(cursor);
}

QString LogViewerWidget::detectLogLevel(const QString &line)
{
    QString upperLine = line.toUpper();
    
    if (upperLine.contains("ERROR") || upperLine.contains("FATAL") || upperLine.contains("CRITICAL")) {
        return "ERROR";
    } else if (upperLine.contains("WARN") || upperLine.contains("CAUTION")) {
        return "WARNING";
    } else if (upperLine.contains("INFO") || upperLine.contains("SUCCESS")) {
        return "INFO";
    } else if (upperLine.contains("DEBUG") || upperLine.contains("TRACE")) {
        return "DEBUG";
    }
    
    return "DEFAULT";
}

QTextCharFormat LogViewerWidget::getFormatForLevel(const QString &level)
{
    if (level == "ERROR") {
        return m_errorFormat;
    } else if (level == "WARNING") {
        return m_warningFormat;
    } else if (level == "INFO") {
        return m_infoFormat;
    } else if (level == "DEBUG") {
        return m_debugFormat;
    }
    
    return m_defaultFormat;
}

void LogViewerWidget::applySyntaxHighlighting(const QString &line)
{
    QString level = detectLogLevel(line);
    QTextCharFormat format = getFormatForLevel(level);
    
    QTextCursor cursor = m_textEdit->textCursor();
    cursor.select(QTextCursor::LineUnderCursor);
    cursor.setCharFormat(format);
}

void LogViewerWidget::updateLineNumbers()
{
    int lineCount = m_textEdit->document()->blockCount();
    m_lineCountLabel->setText(QString("Lines: %1").arg(lineCount));
}
