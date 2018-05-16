#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QFileDialog>
#include <GPX.h>

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

    void on_groupBox_toggled(bool arg1);

private:
    Ui::MainWindow *ui;
    GPX gpx_model;
    QString fname_gpx;
    QString str_log;
    QStringList fnames_photos;

    bool parse_GPX(QString& fname);
    void log(const QString& s);
    void log(const char* s);
    void log(QStringList& sl);
};

#endif // MAINWINDOW_H
