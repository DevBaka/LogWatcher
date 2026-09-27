#include "ProjectExplorer.h"

#include <QHeaderView>
#include <QDir>
#include <QFileInfo>

ProjectExplorer::ProjectExplorer(QWidget *parent)
    : QWidget(parent)
    , m_treeView(nullptr)
    , m_fileSystemModel(nullptr)
    , m_toolBar(nullptr)
    , m_goUpAction(nullptr)
    , m_refreshAction(nullptr)
{
    setupUi();
}

ProjectExplorer::~ProjectExplorer()
{
}

void ProjectExplorer::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // Toolbar
    setupToolBar();
    mainLayout->addWidget(m_toolBar);
    
    // Tree view
    m_treeView = new QTreeView(this);
    m_fileSystemModel = new QFileSystemModel(this);
    
    m_fileSystemModel->setRootPath("");
    m_fileSystemModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    m_fileSystemModel->setNameFilterDisables(false);
    
    m_treeView->setModel(m_fileSystemModel);
    m_treeView->setColumnHidden(1, true);  // Hide size column
    m_treeView->setColumnHidden(2, true);  // Hide type column
    m_treeView->setColumnHidden(3, true);  // Hide date column
    m_treeView->setHeaderHidden(true);
    m_treeView->setAnimated(true);
    m_treeView->setIndentation(20);
    
    // Connect double-click signal
    connect(m_treeView, &QTreeView::doubleClicked,
            this, &ProjectExplorer::onFileDoubleClicked);
    
    mainLayout->addWidget(m_treeView);
}

void ProjectExplorer::setupToolBar()
{
    m_toolBar = new QToolBar(this);
    m_toolBar->setOrientation(Qt::Horizontal);
    m_toolBar->setIconSize(QSize(16, 16));
    
    m_goUpAction = m_toolBar->addAction("Go Up");
    connect(m_goUpAction, &QAction::triggered, this, &ProjectExplorer::onGoUp);
    
    m_refreshAction = m_toolBar->addAction("Refresh");
    connect(m_refreshAction, &QAction::triggered, this, &ProjectExplorer::onRefresh);
}

void ProjectExplorer::setRootPath(const QString &path)
{
    QModelIndex index = m_fileSystemModel->setRootPath(path);
    m_treeView->setRootIndex(index);
    
    // Expand the root
    m_treeView->expand(index);
}

QString ProjectExplorer::rootPath() const
{
    return m_fileSystemModel->rootPath();
}

void ProjectExplorer::onFileDoubleClicked(const QModelIndex &index)
{
    QString filePath = m_fileSystemModel->filePath(index);
    QFileInfo fileInfo(filePath);
    
    if (fileInfo.isFile()) {
        // Check if it's a text file
        if (fileInfo.suffix().toLower() == "log" || 
            fileInfo.suffix().toLower() == "txt" ||
            fileInfo.suffix().isEmpty()) {
            emit fileOpened(filePath);
        }
    } else if (fileInfo.isDir()) {
        // Expand/collapse directory
        if (m_treeView->isExpanded(index)) {
            m_treeView->collapse(index);
        } else {
            m_treeView->expand(index);
        }
    }
}

void ProjectExplorer::onGoUp()
{
    QString currentPath = m_fileSystemModel->rootPath();
    if (!currentPath.isEmpty()) {
        QDir dir(currentPath);
        if (dir.cdUp()) {
            setRootPath(dir.absolutePath());
        }
    }
}

void ProjectExplorer::onRefresh()
{
    QString currentPath = m_fileSystemModel->rootPath();
    if (!currentPath.isEmpty()) {
        m_fileSystemModel->setRootPath("");
        QModelIndex index = m_fileSystemModel->setRootPath(currentPath);
        m_treeView->setRootIndex(index);
        m_treeView->expand(index);
    }
}
