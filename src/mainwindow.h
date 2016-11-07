#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <mesh.h>

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void sig_SendLoadOBJTriggered();
    void sig_SendExportOBJTriggered();
private slots:
    void on_actionQuit_triggered();

    void on_actionCamera_Controls_triggered();

    void on_actionLoadOBJ_triggered();

    void on_actionExportOBJ_triggered();

private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
