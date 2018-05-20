#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QMap>
#include <QVector>
#include <QFileDialog>
#include <QSharedPointer>
#include <QTableWidgetItem>

typedef QVector<QTableWidgetItem> QTabWidItemVec;

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

    void on_checkBox_LocalTime_toggled(bool checked);

private:
    Ui::MainWindow* ui;
    QGPX            gpx_model;
    QString         fname_gpx;
    QString         str_log;
    QStringList     fnames_photos;
    QStringList     column_names;
    QString         last_directory;

    QMap<QString, QTabWidItemVec> columns;

    static const QString settingsFileName;

    bool parse_GPX(QString& fname);
    void log(const QString& s);
    void log(const char* s);
    void log(QStringList& sl);

    void refresh_table();
};

#endif // MAINWINDOW_H
