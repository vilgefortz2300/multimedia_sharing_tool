#ifndef FRMRUNNING_H
#define FRMRUNNING_H

#include "dictionary.h"
#include "connectivity.h"
#include "configurations.h"
#include "tcpserverthread.h"
#include "camerathread.h"
#include <QWidget>
#include <QDir>
#include <qdesktopwidget.h>
#include <QAction>
#include <QListView>
#include <QStandardItemModel>

namespace Ui {
class FrmRunning;
}

class FrmRunning : public QWidget
{
    Q_OBJECT

public:
    explicit FrmRunning(QWidget *parent = nullptr);
    ~FrmRunning();
    void setDict(Dictionary* dict);
    void setSelector(int* selector);
private slots:
    void on_btnStop_clicked();
    void writeTextOnTxtBox(QString str);
    void on_btnSend_clicked();
    void clientConnected();
    void otherGuyDisconnected();
    void on_btnToggleConfig_clicked();
    void startServerStream();
    void on_btnStartStopStream_clicked();
    void streamingEnded();
    void stopStream();
    void takeAScreenPicture();
    void saveCameraFrame(cv::Mat frame);
    void imageScaleBlur(std::string frame_path);

private:
    Ui::FrmRunning *ui;
    Dictionary* dict;
    int* selector;
    TcpServerThread *tcp_server_thread;
    Connectivity c;
    bool client_connected = false;
    bool listconfig_active = true;
    bool is_stream_active = false;
    CameraThread *camera_thread;

    void startServer();
    void stopThreads();
    void enableListConfiguration();
    void disableListConfiguration();
    void updateListConfiguration();

signals:
    void setStreamingEnded();
    void startServerStreamThread();
    void stopServerStreamThread();
    void pictureReady();
    void cameraFrameSaved();
};

#endif // FRMRUNNING_H
