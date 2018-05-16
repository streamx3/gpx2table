#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QFileDialog>

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

private:
    Ui::MainWindow *ui;
    QString fname_gpx;
    QStringList fnames_photos;

    bool parse_GPX(QString& fname);
};

#endif // MAINWINDOW_H
