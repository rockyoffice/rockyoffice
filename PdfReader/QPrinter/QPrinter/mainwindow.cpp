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




     renderer.CommandDrawText(ui->TextToPrint->toPlainText().toStdWString(), 10, 10, 0, 0);
     renderer.endRender();
     ui->ActiveLabel->setText("Not Active");
}


void MainWindow::on_LoadFontFileButton_clicked()
{
    QString fileName1 = QFileDialog::getOpenFileName();
    int size, family_number, style_number;
    family_number = ui->FontFamilyLine->text().toInt();
    style_number = ui->FontStyleLine->text().toInt();
    size = ui->FontSizeLine->text().toInt();
    renderer.loadFontFile(fileName1, family_number , style_number, size);
    ui->FontLabel->setText(renderer.getFontName());
}


void MainWindow::on_SelectOutputFileButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName((QWidget* )0, "Export PDF", QString(), "*.pdf");
     if (QFileInfo(fileName).suffix().isEmpty())
     {
         fileName.append(".pdf");
     }
     renderer.selectOutputFile(fileName);
     renderer.beginRender();
     if (renderer.isActive()) {
         ui->ActiveLabel->setText("Active");
     }
}

