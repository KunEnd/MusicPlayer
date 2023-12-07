#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QFileDialog>
#include <QDebug>
#include <QString>
#include <QDir>
#include <QUrl>
#include <QTimer>
#include <QMediaMetaData>
#include <QPixmap>
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initTreeView();

    void play_song(int);


private slots:

    void opensong();

    void openfile();

    void on_playstop_Song_clicked();
    void on_playstop_Song_pressed();
    void on_playstop_Song_released();

    void on_song_List_doubleClicked(const QModelIndex &index);

    void on_previous_Song_clicked();
    void on_previous_Song_pressed();
    void on_previous_Song_released();

    void on_next_Song_clicked();
    void on_next_Song_pressed();
    void on_next_Song_released();

    void on_Song_progress_sliderMoved(int position);

    void on_Song_progress_rangeChanged(int min, int max);

    void on_volumeBar_valueChanged(int value);

    void on_volumeBar_sliderReleased();

    void on_mute_button_clicked();
    void on_mute_button_pressed();
    void on_mute_button_released();

private:
    Ui::MainWindow *ui;
    QStringList songList;
    QList<QUrl> playList;
    QMediaPlayer *player;
    QAudioOutput *audOutput;
    int curPlayIndex = 0;//当前播放音乐的下标
    bool is_mute = 0;//1表示静音，0表示非静音
    int now_volume = 100;//表示当前音量
    void setScalePix(QPixmap &musicPixmp);
    QStandardItemModel *songList_model = new QStandardItemModel();
};
#endif // MAINWINDOW_H
