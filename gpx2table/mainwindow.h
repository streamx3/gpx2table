#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QFileDialog>

#include <qgpx.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_Open_clicked();

    void on_checkBox_Log_stateChanged(int arg1);

private:
    Ui::MainWindow* ui;
    QGPX            gpx_model;
    QString         fname_gpx;
    QString         str_log;
    QStringList     fnames_photos;
    QString         last_directory;

    static const QString settingsFileName;

    bool parse_GPX(QString& fname);
    void log(const QString& s);
    void log(const char* s);
    void log(QStringList& sl);
};

#endif // MAINWINDOW_H
