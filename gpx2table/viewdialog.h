#ifndef VIEWDIALOG_H
#define VIEWDIALOG_H

#include <QDialog>
#include <QString>
#include <QGraphicsScene>
#include <QTableWidgetItem>

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

private slots:
    void on_pushButton_update_clicked();

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

    QImage*                 image;
    QGraphicsScene*         scene;
    QGraphicsPixmapItem*    GPitem;
    bool                    isImage;
};

#endif // VIEWDIALOG_H
