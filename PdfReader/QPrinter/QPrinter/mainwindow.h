#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qrenderer.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
     QRenderer renderer;


private slots:
    void on_PrintButton_clicked();

    void on_LoadFontFileButton_clicked();

    void on_SelectOutputFileButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
