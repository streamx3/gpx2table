#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>   // TODO removeme!
#include <QSettings>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->groupBox->setCheckable(true);
    ui->groupBox->setChecked(false);
    QString sett_filename = QDir(QApplication::applicationDirPath()
                                 ).filePath("gpx2table.ini");
    qDebug() << sett_filename;
    QSettings settings(sett_filename, QSettings::IniFormat);
    last_directory = settings.value("last_direcotry", "").toString();
    qDebug() << last_directory;
}

MainWindow::~MainWindow()
{
    QSettings settings("gpx2table.ini", QSettings::IniFormat);
    settings.setValue("last_direcotry", last_directory);
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
    bool rv = true;
    fname_gpx = fname;
    QFile file(fname_gpx); // TODO consider removal
    QString parser_messages;
    QGPX tmpGPX(fname, &parser_messages);
//    GPX::parseGPXFile(fname,&(this->gpx_model),&parser_messages);
    log("Parsing GPX file:\n" + parser_messages + "\nParsing done.");
    return rv;
}

QString __getDirNameFromFile(QString filename){
    QFileInfo nfo(filename);
    if(!nfo.exists()){
        return QString("");
    }
    auto dir = nfo.dir();
    return dir.path();
}

void MainWindow::on_pushButton_Open_clicked()
{
    QStringList fnames = QFileDialog::getOpenFileNames(this,
                        tr("Save Address Book"), "/Users/a.shelestov/Documents/Project/Tsuman/Tracks", //TODO removeme
                        tr("Photos (*.jpg *.jpeg);; GPX files (*.gpx);;All Files (*)"));
    foreach (const QString &str, fnames)
    {
        log("File " + str);
    }
//    log(fnames);
    if(only_jpegs(fnames))
    {
        // TODO process JPEGS
        log("JPEGS given");
    }
    else if(fnames.size() == 1 && (*fnames.begin()).endsWith(".gpx", Qt::CaseInsensitive))
    {
        log("GPX given");
        last_directory = __getDirNameFromFile(*fnames.begin());
        log("Dir: \"" + last_directory + "\"");
        parse_GPX(*fnames.begin());
    }
    else
    {
        log("Ambiguous given");
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Ambguous input.\nPlease open 1 GPX or several JPG files");
        messageBox.setFixedSize(500,200);
    }



}

void MainWindow::on_groupBox_toggled(bool arg1)
{
    ui->groupBox->setMaximumHeight(arg1 ? 16777215 : 30);
    ui->textBrowser->setVisible(arg1 ? true : false);
}


void MainWindow::log(const QString& s)
{
    str_log.append(s);
    str_log.append("\n");
    ui->textBrowser->setText(str_log);
}

void MainWindow::log(const char* s)
{
    str_log.append(s);
    str_log.append("\n");
    ui->textBrowser->setText(str_log);
}

void MainWindow::log(QStringList& sl)
{
    foreach (const QString &s, sl)
        log(s);
}
