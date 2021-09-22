#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qrenderer.h"
#include<QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_PrintButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName((QWidget* )0, "Export PDF", QString(), "*.pdf");
     if (QFileInfo(fileName).suffix().isEmpty())
     {
         fileName.append(".pdf");
     }

     QRenderer renderer(fileName);
     renderer.CommandDrawText(ui->TextToPrint->toPlainText().toStdWString(), 10, 10, 0, 0);
     renderer.endRender();
}

