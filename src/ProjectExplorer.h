#ifndef PROJECTEXPLORER_H
#define PROJECTEXPLORER_H

#include <QWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QToolBar>
#include <QAction>

class ProjectExplorer : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectExplorer(QWidget *parent = nullptr);
    ~ProjectExplorer();

    void setRootPath(const QString &path);
    QString rootPath() const;

signals:
    void fileOpened(const QString &filePath);

private slots:
    void onFileDoubleClicked(const QModelIndex &index);
    void onGoUp();
    void onRefresh();

private:
    void setupUi();
    void setupToolBar();
    
    QTreeView *m_treeView;
    QFileSystemModel *m_fileSystemModel;
    QToolBar *m_toolBar;
    QAction *m_goUpAction;
    QAction *m_refreshAction;
};

#endif // PROJECTEXPLORER_H
