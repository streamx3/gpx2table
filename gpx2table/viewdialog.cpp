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
    image       = nullptr;
    scene       = nullptr;
    GPitem      = nullptr;
    isImage     = false;
    isPlaying   = false;
    ui->setupUi(this);

    connect(&player, &QMediaPlayer::positionChanged, \
            this, &ViewDialog::on_position_changed);
    connect(&player, &QMediaPlayer::durationChanged, \
            this, &ViewDialog::on_duration_changed);
}

ViewDialog::~ViewDialog()
{
    delete ui;
}

static QString local_timecode(qint64 pos){
    quint64 min, sec;
    pos/= 1000;
    sec = pos % 60;
    min = pos / 60;
    QString rv(QString("%1:%2")\
               .arg(min, 2, 10, QChar('0'))\
               .arg(sec, 2, 10, QChar('0')));
    return rv;
}

void ViewDialog::on_position_changed(qint64 pos){
    ui->horizontalSlider_timeline->setValue(pos);

    ui->label_timecode->setText(local_timecode(pos));
}

void ViewDialog::on_duration_changed(qint64 dur){
    ui->horizontalSlider_timeline->setMaximum(dur);
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
        isPlaying = false;
        this->setWindowTitle(type+ ": " + data);
        ui->pushButton_update->setVisible(false);
        ui->plainTextEdit->setVisible(false);
        ui->graphicsView->setVisible(false);
        this->setMinimumSize (400, 0);
        this->setSizePolicy(QSizePolicy::Preferred,
                            QSizePolicy::Minimum);
        this->adjustSize();

//        QAudioDeviceInfo nfo(QAudioDeviceInfo::defaultOutputDevice());
//        auto codecs = nfo.supportedCodecs();
//        qDebug() << "Codecs sz = " << codecs.size();
//        foreach(auto &c, codecs)
//            qDebug() << c;
        player.setMedia(QUrl::fromLocalFile(
                            QDir::cleanPath(
                                _path + QDir::separator() + _data)));
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
    player.stop();
    isPlaying = false;
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

void ViewDialog::on_pushButton_play_clicked(){
    if(isPlaying == false){
        isPlaying = true;
        player.play();
        ui->pushButton_play->setText("Pause");
    }else{
        isPlaying = false;
        player.pause();
        ui->pushButton_play->setText("Play");
    }
}

void ViewDialog::on_horizontalSlider_timeline_sliderMoved(int position){
    player.setPosition((quint64)position);
}

void ViewDialog::on_horizontalSlider_volume_sliderMoved(int position){
    player.setVolume((quint64)position);
}
