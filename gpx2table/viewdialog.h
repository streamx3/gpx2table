#ifndef VIEWDIALOG_H
#define VIEWDIALOG_H

#include <QDialog>
#include <QString>
#include <QGraphicsScene>
#include <QTableWidgetItem>
#include <QMediaPlayer>

namespace Ui {
class ViewDialog;
}

class ViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ViewDialog(QWidget *parent = 0);
    ~ViewDialog();

    // Weak poin is using raw pointer here
    // Research and replace if possible
    void setData(QString type, QString data,
                 QString _path, QTableWidgetItem* item);

public slots:
    void on_position_changed(qint64 pos);
    void on_duration_changed(qint64 dur);

private slots:
    void on_pushButton_update_clicked();

    void on_pushButton_play_clicked();

    void on_horizontalSlider_timeline_sliderMoved(int position);

    void on_horizontalSlider_volume_sliderMoved(int position);

private:
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *e);
    void showEvent(QShowEvent * event);

    void updateImageSize();

private:
    Ui::ViewDialog*         ui;
    QTableWidgetItem*       item;
    QString                 type;
    QString                 data;
    QMediaPlayer            player;

    QImage*                 image;
    QGraphicsScene*         scene;
    QGraphicsPixmapItem*    GPitem;
    bool                    isImage;
    bool                    isPlaying;
};

#endif // VIEWDIALOG_H
