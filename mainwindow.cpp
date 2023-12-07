#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //ui->playstop_Song->setIcon(QIcon(":/source/play_1.png"));
    ui->playstop_Song->setIcon(QIcon(":/icons/stop.png"));
    ui->playstop_Song->setIconSize(QSize(76, 76));
    ui->previous_Song->setIcon(QIcon(":/icons/pre.png"));
    ui->previous_Song->setIconSize(QSize(38, 38));
    ui->next_Song->setIcon(QIcon(":/icons/next.png"));
    ui->next_Song->setIconSize(QSize(38, 38));
    ui->mute_button->setIcon(QIcon(":/icons/volume.png"));
    ui->mute_button->setIconSize(QSize(38, 38));

    initTreeView();

    //菜单栏
    connect(ui->open_action, SIGNAL(triggered(bool)), this, SLOT(opensong()));
    connect(ui->openfile_action, SIGNAL(triggered(bool)), this, SLOT(openfile()));


    //初始化output对象
    audOutput = new QAudioOutput(this);
    //初始化媒体播放对象
    player = new QMediaPlayer(this);
    player->setAudioOutput(audOutput);
    audOutput->setVolume(0.1);

    //获取当前音乐播放时长
    connect(player, &QMediaPlayer::positionChanged, this, [=](qint64 position)
    {
        if(ui->Song_progress->isSliderDown()) return;
        ui->playback_Duration->setText(QString("%1: %2").arg(position/1000/60,2,10,QChar('0')).arg(position/1000%60,2,10,QChar('0')));
        ui->Song_progress->setValue(position);
    });
    //获取歌曲总时长
    connect(player, &QMediaPlayer::durationChanged, this, [=](qint64 duration)
    {
        ui->total_Duration->setText(QString("%1: %2").arg(duration/1000/60,2,10,QChar('0')).arg(duration/1000%60,2,10,QChar('0')));
        ui->Song_progress->setRange(0, duration);
    });

    //拖动进度条跳转播放
    connect(ui->Song_progress, &reSlider::sliderMoved, player, &QMediaPlayer::setPosition);

    //点击进度条跳转播放
    connect(ui->Song_progress, &reSlider::sliderPressAt, player, &QMediaPlayer::setPosition);

    // 显示嵌入封面
    connect(player, &QMediaPlayer::metaDataChanged, this, [=]()
    {
        QMediaMetaData metaData = player->metaData();
        QVariant metaImg = metaData.value(QMediaMetaData::ThumbnailImage);
        if(metaImg.isValid())
        {
            QImage img= metaImg.value<QImage>(); //QVariant 转换为 QImage

            QPixmap musicPixmp= QPixmap::fromImage(img);

            MainWindow::setScalePix(musicPixmp);
        }
        else
        {
            QImage img(":/cover/720.jpg");
            QPixmap musicPixmp = QPixmap::fromImage(img);
            MainWindow::setScalePix(musicPixmp);
        }

    });

    //显示歌曲详细信息
    connect(player, &QMediaPlayer::metaDataChanged, this, [=]()
    {
        //获取音频文件元数据，打印
        QMediaMetaData metaData = player->metaData();
        qInfo() << "Title: " << metaData.value(QMediaMetaData::Title).toString();
        qInfo() << "Artist: " << metaData.value(QMediaMetaData::Author).toString();
        qInfo() << "Album: " << metaData.value(QMediaMetaData::AlbumTitle).toString();

        //将音频文件数据放上界面
        ui->author_label->setText(metaData.value(QMediaMetaData::Author).toString());
        ui->album_label->setText(metaData.value(QMediaMetaData::AlbumTitle).toString());
        ui->title_label->setText(metaData.value(QMediaMetaData::Title).toString());
    });



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initTreeView()
{

    songList_model->setHorizontalHeaderLabels(QStringList() << QStringLiteral("#") << QStringLiteral("标题")
                                     << QStringLiteral("艺术家") << QStringLiteral("专辑") << QStringLiteral("时间"));



    ui->song_tree->setModel(songList_model);
}

void MainWindow::setScalePix(QPixmap& musicPixmp)
{
    int scalHeight = ui->cover_label->height();
    int scalWidth = musicPixmp.scaledToHeight(scalHeight).width();// 根据label高度计算得到控件目标宽度

    if(ui->cover_label->width() < scalWidth)
    {// 再次调整，确保图片能够在控件中完整显示
        scalWidth = ui->cover_label->width();
        scalHeight = musicPixmp.scaledToWidth(scalWidth).height();
    }

    ui->cover_label->setPixmap(musicPixmp.scaled(scalWidth,scalHeight,Qt::KeepAspectRatio,Qt::SmoothTransformation));
}

//封装播放音乐的函数
void MainWindow::play_song(int curPlayIndex)
{
    player->setSource(playList[curPlayIndex]);
    player->play();
    ui->Song_name->setText(playList[curPlayIndex].fileName());
    //ui->Song_progress = 0;
    ui->playstop_Song->setIcon(QIcon(":/icons/play.png"));
}

//打开
void MainWindow::opensong()
{
    //QString song_path = QFileDialog::getExistingDirectory(this, "打开", "E:\\Music");
    QString songname = QFileDialog::getOpenFileName(this, tr("打开"), "E:\\Music", tr("(*.mp3 *.flac *.wav)"));
    QUrl songpath = QUrl::fromLocalFile(songname);

    //放入音乐列表
    if(songpath.fileName() == nullptr)
        return;
    ui->song_List->addItem(songpath.fileName());//展示音乐列表
    ui->song_List->setCurrentRow(0);

    songList.append(songpath.fileName());
    playList.append(songpath);
    qInfo() << "-------------------------点击打开----------------------------";
    qInfo() << "将要展示在song_List的音乐列表:" << songList;
    qInfo() << "playList实际的播放列表:" << playList;
}

//打开文件
void MainWindow::openfile()
{
    //打开文件选择框，选择音乐
    QString song_path = QFileDialog::getExistingDirectory(this, "打开", "E:\\Music");
    qInfo() << "获取音乐路径：" << song_path;//打印路径，检查

    //根据路径获取所有音乐的格式
    QDir song_dir(song_path);
    songList = (song_dir.entryList(QStringList()<<"*.mp3"<<"*.flac"<<"*.wav"));
    qInfo() << "获取到的音乐：" << songList;//打印获取到的所有歌曲，检查

    ui->song_List->addItems(songList);//展示音乐列表
    ui->song_List->setCurrentRow(0);//默认选中第一项

    //保存音乐完整路径
    for(auto temp : songList)
    {
        playList.append(QUrl::fromLocalFile(song_path + "/" + temp));
    }
    qInfo() << "------------------------点击打开文件---------------------------";
    qInfo() << "将要展示在song_List的音乐列表:" << songList;
    qInfo() << "playList实际的播放列表:" << playList;
}

//-------------------------------播放模块-----------------------------------
//播放和暂停
void MainWindow::on_playstop_Song_clicked()
{
    if(playList.empty()) return;//列表为空不做反应
    //播放 & 暂停音乐
    switch(player->playbackState())
    {
        case QMediaPlayer::PlaybackState::StoppedState:
        {
            //没有播放，则播放选中音乐
            curPlayIndex = ui->song_List->currentRow();
            play_song(curPlayIndex);
            break;
        }

        case QMediaPlayer::PlaybackState::PlayingState:
        {
            //正在播放，则暂停
            player->pause();
            ui->playstop_Song->setIcon(QIcon(":/icons/stop.png"));
            break;
        }

        case QMediaPlayer::PlaybackState::PausedState:
        {
            //正在暂停，则继续播放
            player->play();
            ui->playstop_Song->setIcon(QIcon(":/icons/play.png"));
            break;
        }
    }

}
void MainWindow::on_playstop_Song_pressed()
{
    ui->playstop_Song->setStyleSheet("QPushButton:hover{"
                                     "background-color:rgb(150, 150, 150)}"
                                     "QPushButton{"
                                     "border-radius:25px}");
}
void MainWindow::on_playstop_Song_released()
{
    ui->playstop_Song->setStyleSheet("QPushButton:hover{"
                                     "background-color:rgb(200, 200, 200)}"
                                     "QPushButton{"
                                     "border-radius:25px}");
}


//双击列表播放音乐
void MainWindow::on_song_List_doubleClicked(const QModelIndex &index)
{
    curPlayIndex = index.row();
    play_song(curPlayIndex);
}

//上一首
void MainWindow::on_previous_Song_clicked()
{
    if(playList.empty()) return;//列表为空不做反应
    curPlayIndex--;
    if(curPlayIndex < 0)
    {
        curPlayIndex = playList.size()-1;
    }
    ui->song_List->setCurrentRow(curPlayIndex);
    play_song(curPlayIndex);
}
void MainWindow::on_previous_Song_pressed()
{
    ui->previous_Song->setStyleSheet("QPushButton:hover{"
                                     "background-color:rgb(150, 150, 150)}"
                                     "QPushButton{"
                                     "border-radius:16px}");
}
void MainWindow::on_previous_Song_released()
{
    ui->previous_Song->setStyleSheet("QPushButton:hover{"
                                     "background-color:rgb(200, 200, 200)}"
                                     "QPushButton{"
                                     "border-radius:16px}");
}

//下一首
void MainWindow::on_next_Song_clicked()
{
    if(playList.empty()) return;//列表为空不做反应
    curPlayIndex = (curPlayIndex + 1) % playList.size();//防止越界
    ui->song_List->setCurrentRow(curPlayIndex);
    play_song(curPlayIndex);
}
void MainWindow::on_next_Song_pressed()
{
    ui->next_Song->setStyleSheet("QPushButton:hover{"
                                     "background-color:rgb(150, 150, 150)}"
                                     "QPushButton{"
                                     "border-radius:16px}");
}
void MainWindow::on_next_Song_released()
{
    ui->next_Song->setStyleSheet("QPushButton:hover{"
                                     "background-color:rgb(200, 200, 200)}"
                                     "QPushButton{"
                                     "border-radius:16px}");
}


//-------------------------进度条---------------------------
//拖动进度条跳转播放
void MainWindow::on_Song_progress_sliderMoved(int position)
{
    //player->setPosition(position);
    qInfo() << "'拖动'进度条到当前位置：" << position;
}

void MainWindow::on_Song_progress_rangeChanged(int min, int max)
{
    qInfo() << "进度条变更小值为：" << min;
    qInfo() << "进度条变更大值为：" << max;
}



//---------------------音量模块--------------------------
//拖动调节音量
void MainWindow::on_volumeBar_valueChanged(int value)
{
    qreal volume = value * 0.01;
    audOutput->setVolume(volume);
    //qInfo() << "检查：传入setVolume()中的参数值为：" << volume;
    //qInfo() << "滑动调节音量为：" << value;
}
void MainWindow::on_volumeBar_sliderReleased()
{
    //松开调节音量按钮，打印数值检查
    int value = ui->volumeBar->value();
    qreal volume = value * 0.01;
    qInfo() << "检查：传入setVolume()中的参数值为：" << volume;
    qInfo() << "滑动调节音量为：" << value;
}

//有声音则静音，已静音则恢复
void MainWindow::on_mute_button_clicked()
{
    if(is_mute == 0)//非静音
    {
        now_volume = ui->volumeBar->value();//保存当前音量
        audOutput->setVolume(0);
        ui->volumeBar->setValue(0);
        qInfo() << "点击静音按钮，音量为0";
        is_mute = 1;//已静音
    }
    else//静音时，则恢复到原来状态
    {
        audOutput->setVolume(now_volume);
        ui->volumeBar->setValue(now_volume);
        qInfo() << "已静音，恢复至原本音量为：" << now_volume;
        is_mute = 0;//非静音
    }
};
void MainWindow::on_mute_button_pressed()
{
    ui->mute_button->setStyleSheet("QPushButton:hover{"
                                     "background-color:rgb(150, 150, 150)}"
                                     "QPushButton{"
                                     "border-radius:16px}");
}
void MainWindow::on_mute_button_released()
{
    ui->mute_button->setStyleSheet("QPushButton:hover{"
                                     "background-color:rgb(200, 200, 200)}"
                                     "QPushButton{"
                                     "border-radius:16px}");
}


