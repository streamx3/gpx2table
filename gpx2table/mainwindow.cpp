#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>   // TODO removeme!
#include <QSettings>
#include <QMessageBox>

const QString MainWindow::settingsFileName = "gpx2table.ini"; // TODO rework

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    ui->textBrowser->setVisible(false);
    ui->textBrowser->setMaximumHeight(0);
//    QString sett_filename = QDir(QApplication::applicationDirPath()
//                                 ).filePath(settingsFileName);
//    log("Settings filename: " + sett_filename);
    QSettings settings(settingsFileName, QSettings::IniFormat);
    last_directory = settings.value("last_dir").toString();
//    log("Last dir: " +  last_directory);

    QVector<QSharedPointer<QTableWidgetItem*>> tmpdata;
    cells["id" ] = tmpdata;
    cells["lat"] = tmpdata;
    cells["lon"] = tmpdata;
    cells["ele"] = tmpdata;
    cells["time"] = tmpdata;
    cells["CDATA"] = tmpdata;
    cells["link" ] = tmpdata;
    cells["sat"] = tmpdata;
    cells["speed"] = tmpdata;       // TODO implement in future
    cells["orientation"] = tmpdata; // TODO implement in future
}

MainWindow::~MainWindow()
{

    QSettings settings(settingsFileName, QSettings::IniFormat);
    if(last_directory.length() > 1){
        settings.setValue("last_dir", last_directory);
        settings.sync();
    }
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
    parser_messages.clear();
    tmpGPX.dump_wpts(parser_messages);
    log("Waypoints:\n" + parser_messages + "\n");
    if(!tmpGPX.empty())
        gpx_model = tmpGPX;
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
    log("Using dir: " + last_directory);
    QStringList fnames = QFileDialog::getOpenFileNames(this,
                        tr("Save Address Book"), last_directory,
                        tr("GPX files (*.gpx);; Photos (*.jpg *.jpeg);; All Files (*)"));
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

void MainWindow::log(const QString& s)
{
    str_log.append(s);
    str_log.append("\n");
    ui->textBrowser->setText(str_log);
    qDebug() << s;
}

void MainWindow::log(const char* s)
{
    str_log.append(s);
    str_log.append("\n");
    ui->textBrowser->setText(str_log);
    qDebug() << s;
}

void MainWindow::log(QStringList& sl)
{
    foreach (const QString &s, sl)
        log(s);
}

void MainWindow::on_checkBox_Log_stateChanged(int arg1)
{
    ui->textBrowser->setVisible(arg1 ? true : false);
    ui->textBrowser->setMaximumHeight(arg1 ? 16777215 : 30);
}

void MainWindow::refresh_table(){
    QList<QGPXwpt> wpts = gpx_model.getWaypoints();
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(wpts.count());
    ui->tableWidget->setColumnCount(8);
//    foreach (auto &wpt, wpts) {

//    }
}
