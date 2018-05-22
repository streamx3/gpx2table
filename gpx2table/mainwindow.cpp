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

    QString window_width = settings.value("width").toString();
    QString window_height = settings.value("height").toString();
    if(1 == settings.value("use_localtime").toInt()){
        ui->checkBox_LocalTime->setChecked(true);
    }
    ui->checkBox_Log->setChecked(settings.value("log").toInt() > 0);

    this->debug = settings.value("debug").toInt();
    ui->pushButton_dbg->setVisible(debug > 0 ? true : false);
    ui->spinBox->setVisible(debug > 0 ? true : false);
    ui->spinBox->setValue(debug);

    if(window_width.toInt() > 100){
        if(debug)
            qDebug() << "Attempt to set size: " << window_width << " " << window_height;
        QSize tmpsz = this->size();
        tmpsz.setWidth(window_width.toInt());
        if(window_height.toInt() > 100){
            tmpsz.setHeight(window_height.toInt());
        }
        this->resize(tmpsz);
    }

    ui->tableWidget->setColumnCount(8);
    column_names.push_back("Latitude");
    column_names.push_back("Longitude");
    column_names.push_back("Elev.");
    column_names.push_back("Time");
    column_names.push_back("CDATA");
    column_names.push_back("Link");
    column_names.push_back("Sat");
    column_names.push_back("Speed");

    // TODO apply OS-dependent preset for column widths or adjust to content
#define MY_TABLE_COL_SZ 8
    int col_widths[MY_TABLE_COL_SZ] = {70,70,30,170,90,160,30,40};
    settings.beginReadArray("cols");
    for(int i = 0; i < MY_TABLE_COL_SZ; ++i){
        settings.setArrayIndex(i);
        auto tmp = settings.value("c").toInt();
        if(tmp > 10)
            col_widths[i] = tmp;
    }
    settings.endArray();

    for (auto i = 0; i < 8; ++i){
        ui->tableWidget->setColumnWidth(i, col_widths[i]);
    }

    ui->tableWidget->setHorizontalHeaderLabels(column_names);
}

MainWindow::~MainWindow()
{

    QSettings settings(settingsFileName, QSettings::IniFormat);
    if(last_directory.length() > 1){
        settings.setValue("last_dir", last_directory);
    }
    settings.setValue("width", ui->centralWidget->geometry().width());
    settings.setValue("height", ui->centralWidget->geometry().height());
    settings.setValue("use_localtime",
                      ui->checkBox_LocalTime->isChecked() ? 1 : 0);
    settings.setValue("debug", debug);
    settings.setValue("log", ui->checkBox_Log->isChecked() ? 1 : 0);

    // Column Array
    settings.beginWriteArray("cols");
    for(int i = 0; i < MY_TABLE_COL_SZ; ++i){
        settings.setArrayIndex(i);
        settings.setValue("c", ui->tableWidget->columnWidth(i));
    }
    settings.endArray();

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
    QTableWidgetItem *p_tmpItem;
    QVector<QGPXwpt> wpts = gpx_model.getWaypoints();
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(wpts.count());
    ui->tableWidget->setColumnCount(8);

    ui->tableWidget->setRowCount(wpts.size());

    for(int i = 0; i < wpts.size(); ++i) {
        p_tmpItem = new QTableWidgetItem(
                    QString::number(wpts[i].coordinate.latitude(), 'G', 10));
        ui->tableWidget->setItem(i,0,p_tmpItem);

        p_tmpItem = new QTableWidgetItem(
                    QString::number(wpts[i].coordinate.longitude(), 'G', 10));
        ui->tableWidget->setItem(i,1,p_tmpItem);

        p_tmpItem = new QTableWidgetItem(
                    QString::number(wpts[i].coordinate.altitude(), 'G', 5));
        ui->tableWidget->setItem(i,2,p_tmpItem);

        if(ui->checkBox_LocalTime->isChecked()){
            auto cur_ts = QDateTime::currentDateTime().timeSpec();
            wpts[i].time = wpts[i].time.toTimeSpec(cur_ts);
        }
        p_tmpItem = new QTableWidgetItem(wpts[i].time.toString());
        ui->tableWidget->setItem(i,3,p_tmpItem);

        p_tmpItem = new QTableWidgetItem(wpts[i].CDATA);
        ui->tableWidget->setItem(i,4,p_tmpItem);

        QWidget* pWidget = new QWidget();
        QPushButton* btn_edit = new QPushButton();
        btn_edit->setText(wpts[i].link);
        QHBoxLayout* pLayout = new QHBoxLayout(pWidget);
        pLayout->addWidget(btn_edit);
        pLayout->setAlignment(Qt::AlignCenter);
        pLayout->setContentsMargins(0, 0, 0, 0);
        pWidget->setLayout(pLayout);


//        p_tmpItem = new QTableWidgetItem(wpts[i].link);
        ui->tableWidget->setCellWidget(i,5,pWidget);
//        ui->tableWidget->setItem(i,5,p_tmpItem);

        p_tmpItem = new QTableWidgetItem(wpts[i].sat);
        ui->tableWidget->setItem(i,6,p_tmpItem);
    }

}

void MainWindow::on_checkBox_LocalTime_toggled(bool)
{
    refresh_table(); // TODO Fix before commit
}

void MainWindow::on_pushButton_dbg_clicked()
{
    ui->tableWidget->setRowCount(ui->tableWidget->rowCount()-1);
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    this->debug = arg1 > 5 ? 5 : arg1;
}
