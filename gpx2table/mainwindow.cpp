#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool only_jpegs(QStringList& data)
{
    bool rv = true;
    foreach(const QString &str, data)
    {
        if(!str.endsWith(".jpeg", Qt::CaseInsensitive) &&
                !str.endsWith(".jpg", Qt::CaseInsensitive))
            rv = false;
        break;
    }
    return rv;
}

bool MainWindow::parse_GPX(QString& fname){
    fname_gpx = fname;
    QFile file(fname_gpx);
//    file.open();
}

void MainWindow::on_pushButton_Open_clicked()
{
    QStringList fnames = QFileDialog::getOpenFileNames(this,
                        tr("Save Address Book"), "/Users/a.shelestov/Documents/Project/Tsuman/Tracks", //TODO removeme
                        tr("Photos (*.jpg *.jpeg);; GPX files (*.gpx);;All Files (*)"));
    foreach (const QString &str, fnames)
        qDebug() << str;

    if(only_jpegs(fnames))
    {
        // TODO process JPEGS
        qDebug() << "JPEGS given";
    }
    else if(fnames.size() == 1 && (*fnames.begin()).endsWith(".gpx", Qt::CaseInsensitive))
    {
        qDebug() << "GPX given";
    }
    else
    {
        qDebug() << "Ambiguous given";
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Ambguous input.\nPlease open 1 GPX or several JPG files");
        messageBox.setFixedSize(500,200);
    }


}
