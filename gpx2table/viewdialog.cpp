#include "viewdialog.h"
#include "ui_viewdialog.h"

#include <QDir>
#include <QDebug>
#include <QAudioOutput>
#include <QGraphicsPixmapItem>

ViewDialog::ViewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewDialog)
{
    image   = nullptr;
    scene   = nullptr;
    GPitem  = nullptr;
    isImage = false;
    ui->setupUi(this);
}

ViewDialog::~ViewDialog()
{
    delete ui;
}

void ViewDialog::setData(QString _type, QString _data,
                         QString _path, QTableWidgetItem* _item){
    this->type = _type;
    this->data = _data;
    this->item = _item;

    if("Photo" == type){
        isImage = true;
        this->setWindowTitle(type+ ": " + data);
        ui->pushButton_play->setVisible(false);
        ui->pushButton_update->setVisible(false);
        ui->label_timecode->setVisible(false);
        ui->label_volume_icon->setVisible(false);
        ui->horizontalSlider_timeline->setVisible(false);
        ui->horizontalSlider_volume->setVisible(false);
        ui->plainTextEdit->setVisible(false);
        ui->horizontalLayout_4->setEnabled(false);

        scene = new QGraphicsScene();
        if(nullptr == scene)
            qDebug() << "Failed to allocate QGraphicsScene";
        ui->graphicsView->setScene(scene);
        image = new QImage(QDir::cleanPath(_path + QDir::separator() + _data));
        if(nullptr == image)
            qDebug() << "Failed to allocate QImage";
        if(image->isNull())
            qDebug() << "Image reading failed[" << data<< "]";
        auto sz = ui->graphicsView->size();
        GPitem = new QGraphicsPixmapItem(QPixmap::fromImage(*image).scaled(
                                             sz, Qt::KeepAspectRatio,
                                             Qt::SmoothTransformation));
        if(nullptr == GPitem)
            qDebug() << "Failed to allocate QGraphicsPixmapItem";
        scene->addItem(GPitem);

        ui->graphicsView->show();

    }else if ("Voice recording" == type) {
        this->setWindowTitle(type+ ": " + data);
        ui->pushButton_update->setVisible(false);
        ui->plainTextEdit->setVisible(false);
        ui->graphicsView->setVisible(false);
        this->setMinimumSize (400, 0);
        this->setSizePolicy(QSizePolicy::Preferred,
                            QSizePolicy::Minimum);
        this->adjustSize();

//        this
        QAudioDeviceInfo nfo(QAudioDeviceInfo::defaultOutputDevice());
        auto codecs = nfo.supportedCodecs();
        qDebug() << "Codecs sz = " << codecs.size();
        foreach(auto &c, codecs)
            qDebug() << c;
    }else if ("Text" == type){ // TODO check me
        this->setWindowTitle(type+ ": " + data);
        ui->pushButton_play->setVisible(false);
        ui->label_timecode->setVisible(false);
        ui->label_volume_icon->setVisible(false);
        ui->horizontalSlider_timeline->setVisible(false);
        ui->horizontalSlider_volume->setVisible(false);
        ui->graphicsView->setVisible(false);
        ui->plainTextEdit->setPlainText(data);
    }
}

void ViewDialog::on_pushButton_update_clicked(){
    if(nullptr != item){
        item->setText(ui->plainTextEdit->toPlainText());
    }
    this->close();
}

void ViewDialog::closeEvent(QCloseEvent *event){
    qDebug() << "View Close event";
    // TODO uncomment ASAP
//    if(nullptr != image)
//        delete image;
//    if(nullptr != scene)
//        delete scene;
//    if(nullptr != GPitem)
//        delete GPitem;
}

void ViewDialog::resizeEvent(QResizeEvent *e){
    qDebug() << "Resize event";
    updateImageSize();
}

void ViewDialog::updateImageSize(){
    // TODO Refactor or remove
    if(!isImage)
        return;
    if(nullptr == GPitem)
        delete GPitem;
    scene->clear();
    auto sz = ui->graphicsView->size();
    GPitem = new QGraphicsPixmapItem(QPixmap::fromImage(*image).scaled(
                                         sz, Qt::KeepAspectRatio,
                                         Qt::SmoothTransformation));
    if(nullptr == GPitem)
        qDebug() << "Failed to allocate QGraphicsPixmapItem";
    scene->addItem(GPitem);
    ui->graphicsView->show();
}

void ViewDialog::showEvent(QShowEvent * event){
    updateImageSize();
}
