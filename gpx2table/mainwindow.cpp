#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>   // TODO removeme!
#include <QSettings>
#include <QMessageBox>

#include "version.h"

const QString MainWindow::settingsFileName = "gpx2table.ini"; // TODO rework

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    this->setWindowTitle(QString("gpx2table v") + APP_VERION_STR);

    ui->textBrowser->setVisible(false);
    ui->textBrowser->setMaximumHeight(0);
//    QString sett_filename = QDir(QApplication::applicationDirPath()
//                                 ).filePath(settingsFileName);
//    log("Settings filename: " + sett_filename);
    QSettings settings(settingsFileName, QSettings::IniFormat);
    last_directory = settings.value("last_dir").toString();

    QString window_width = settings.value("window_hsize").toString();
    if(1 == settings.value("use_localtime").toInt()){
        ui->checkBox_LocalTime->setChecked(true);
    }
    if(window_width != "0"){
        qDebug() << "Attempt to set size: " << window_width;
        QSize tmpsz = this->size();
        tmpsz.setWidth(window_width.toInt());
        this->resize(tmpsz);
    }

    columns["lat"] = QTabWidItemVec();
    columns["lon"] = QTabWidItemVec();
    columns["ele"] = QTabWidItemVec();
    columns["time"] = QTabWidItemVec();
    columns["CDATA"] = QTabWidItemVec();
    columns["link" ] = QTabWidItemVec();
    columns["sat"] = QTabWidItemVec();
    columns["speed"] = QTabWidItemVec();       // TODO implement in future
    columns["orientation"] = QTabWidItemVec(); // TODO implement in future

    ui->tableWidget->setColumnCount(8);
    column_names.push_back("Latitude");
    column_names.push_back("Longitude");
    column_names.push_back("Elev.");
    column_names.push_back("Time");
    column_names.push_back("CDATA");
    column_names.push_back("Link");
    column_names.push_back("Sat");
    column_names.push_back("Speed");

    ui->tableWidget->setColumnWidth(  0,  70 );
    ui->tableWidget->setColumnWidth(  1,  70 );
    ui->tableWidget->setColumnWidth(  2,  30 );
    ui->tableWidget->setColumnWidth(  3,  170 );
    ui->tableWidget->setColumnWidth(  4,  90 );
    ui->tableWidget->setColumnWidth(  5,  160 );
    ui->tableWidget->setColumnWidth(  6,  30 );
    ui->tableWidget->setColumnWidth(  7,  40 );

    ui->tableWidget->setHorizontalHeaderLabels(column_names);
}

MainWindow::~MainWindow()
{

    QSettings settings(settingsFileName, QSettings::IniFormat);
    if(last_directory.length() > 1){
        settings.setValue("last_dir", last_directory);
    }
    settings.setValue("window_hsize", ui->centralWidget->geometry().width());
    settings.setValue("use_localtime",
                      ui->checkBox_LocalTime->isChecked() ? 1 : 0);
    settings.sync();
    delete ui;
}

bool only_jpegs(QStringList& data){
    foreach(const QString &str, data)
    {
        if(!str.endsWith(".jpeg", Qt::CaseInsensitive) &&
                !str.endsWith(".jpg", Qt::CaseInsensitive))
            return false;
    }
    return true;
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
    refresh_table();
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
//    qDebug() << s;
}

void MainWindow::log(const char* s)
{
    str_log.append(s);
    str_log.append("\n");
    ui->textBrowser->setText(str_log);
//    qDebug() << s;
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
    QVector<QGPXwpt> wpts = gpx_model.getWaypoints();
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(wpts.count());
    ui->tableWidget->setColumnCount(8);

    for(auto it = columns.begin(); it != columns.end(); ++it){
        it.value().resize(wpts.size());
    }
    for(int i = 0; i < wpts.size(); ++i) {
        columns["lat"][i].setText(QString::number(wpts[i].coordinate.latitude(), 'G', 10));
        columns["lon"][i].setText(QString::number(wpts[i].coordinate.longitude(), 'G', 10));
        columns["ele"][i].setText(QString::number(wpts[i].coordinate.altitude(), 'G', 5));
        if(ui->checkBox_LocalTime->isChecked()){
            auto cur_ts = QDateTime::currentDateTime().timeSpec();
            wpts[i].time = wpts[i].time.toTimeSpec(cur_ts);
        }
        columns["time"][i].setText(wpts[i].time.toString());
        columns["CDATA"][i].setText(wpts[i].CDATA);
        columns["link"][i].setText(wpts[i].link);
        columns["sat"][i].setText(wpts[i].sat);

        ui->tableWidget->setItem(i,0,&(columns["lat"][i]));
        ui->tableWidget->setItem(i,1,&(columns["lon"][i]));
        ui->tableWidget->setItem(i,2,&(columns["ele"][i]));
        ui->tableWidget->setItem(i,3,&(columns["time"][i]));
        ui->tableWidget->setItem(i,4,&(columns["CDATA"][i]));
        ui->tableWidget->setItem(i,5,&(columns["link"][i]));
        ui->tableWidget->setItem(i,6,&(columns["sat"][i]));
    }

}

void MainWindow::on_checkBox_LocalTime_toggled(bool)
{
//    refresh_table(); // TODO Fix before commit
}
