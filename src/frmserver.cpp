//_____________________________________________________________________________
//_____________________________________________________________________________
//                             FRAME SERVER
//_____________________________________________________________________________
//_____________________________________________________________________________

#include "frmserver.h"
#include "ui_frmserver.h"

FrmServer::FrmServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FrmServer)
{
    ui->setupUi(this);
    std::string public_ip = Connectivity::getPublicIp();
    ui->lblIpServer2->setText(QString::fromStdString(public_ip));
    ui->txtIpClient->setText("127.0.0.1");
    ui->txtPort->setText("49152");
    setFixedSize(size());

    QRect desktopRect = QApplication::desktop()->availableGeometry(this);
    QPoint center = desktopRect.center();
    move(center.x()- static_cast<int>(width()*0.5),
         center.y()- static_cast<int>(height()*0.5));

    // Set icons
    setWindowIcon(QIcon(":/resources/media/mst_logo.png"));

    QPixmap back_img(":/resources/media/back.png");
    QIcon BackIcon(back_img);
    ui->btnBack->setIcon(BackIcon);
    ui->btnBack->setIconSize(back_img.rect().size());

    QPixmap load_img(":/resources/media/load.png");
    QIcon LoadIcon(load_img);

    QPixmap save_img(":/resources/media/save.png");
    QIcon SaveIcon(save_img);

    QPixmap start_img(":/resources/media/start.png");
    QIcon StartIcon(start_img);
    ui->btnStartServer->setIcon(StartIcon);
    ui->btnStartServer->setIconSize(start_img.rect().size());
}

FrmServer::~FrmServer()
{
    if(*selector == 1)
        *selector = -1;
    delete ui;
}

/**
 * Set text of gui controls according to the current selected language.
 * @param   : dict; set current dictionary based on current language.
 * @return  : void.
*/
void FrmServer::setDict(Dictionary* dict)
{
    this->dict = dict;
    ui->lblInfoServer->setText(QString::fromStdString
                               ((*dict).getTextOflblInfoServer()));
    (*dict).setTextOflblIpServerS(ui->lblIpServer);
    (*dict).setTextOflblIpClientS(ui->lblIpClient);
    (*dict).setTextOflblPort(ui->lblPort);
    (*dict).setTextOflblPasswordS(ui->lblPassword);
    (*dict).setTextOflblSource(ui->lblSource);
    (*dict).setTextOflblVideoChunk(ui->lblVideoChunk);
    (*dict).setTextOflblBlur(ui->lblBlur);
    ui->lblConfig->setText(QString::fromStdString
                           ((*dict).getTextOflblConfig()));
    ui->lblLeaveConfig->setText(QString::fromStdString
                                ((*dict).getTextOflblLeaveConfig()));
    (*dict).setTextOflblResolution(ui->lblResolution);
    (*dict).setTextOflblFps(ui->lblFps);
    (*dict).setTextOflblColorScale(ui->lblColorScale);
    (*dict).setTextOflblNetwork(ui->lblNetwork);
    ui->btnBack->setText(QString::fromStdString
                         ((*dict).getTextOfbtnBack()));
    ui->btnStartServer->setText(QString::fromStdString
                                ((*dict).getTextOfbtnStartServer()));
    (*dict).fillcmbSource(ui->cmbSource);
    (*dict).fillcmbResolution(ui->cmbResolution);
    (*dict).fillcmbColorScale(ui->cmbColorScale);
    (*dict).fillcmbFps(ui->cmbFps);
    (*dict).fillcmbVideoChunk(ui->cmbVideoChunk);
    (*dict).fillcmbNetwork(ui->cmbNetwork);
}

/**
 * Set the application selector.
 * @param   : selector; variable passed from the main.
 * @return  : void.
*/
void FrmServer::setSelector(int *selector)
{
    this->selector = selector;
}

/**
 * Return to Frame Main.
 * @param   : void.
 * @return  : void.
*/
void FrmServer::on_btnBack_clicked()
{
    *selector = 0;
    this->close();
}

/**
 * Set configurations variables.
 * @param   : void.
 * @return  : void.
*/
void FrmServer::setConfigurations()
{
    Configurations::system = SERVER;
    Configurations::source = static_cast<uint8_t>
            (ui->cmbSource->currentIndex());
    Configurations::server_ip = ui->lblIpServer2->text().toStdString();
    Configurations::client_ip = ui->txtIpClient->text().toStdString();
    Configurations::port = static_cast<uint16_t>
            (stoi(ui->txtPort->text().toStdString()));
    Configurations::password = ui->txtPassword->text().toStdString();
    Configurations::leave_client_config = ui->chkLeaveConfig->isChecked();
    Configurations::resolution = static_cast<uint8_t>
            (ui->cmbResolution->currentIndex());
    Configurations::fps = static_cast<uint8_t>(ui->cmbFps->currentIndex());
    Configurations::color_scale = static_cast<uint8_t>
            (ui->cmbColorScale->currentIndex());
    Configurations::file_name = "";
    if(ui->chkIntrusionDetection->checkState() == Qt::Unchecked)
        Configurations::intrusion_detection_enabled = false;
    else
        Configurations::intrusion_detection_enabled = true;

    Configurations::network = ui->cmbNetwork->currentText().toStdString();

    Configurations::blur_effect = ui->sldBlur->value();
}

/**
 * Allows the user to select the screen region to record.
 * @param   : void.
 * @return  : void.
*/
void FrmServer::selectRegion()
{
    Display* disp;
    Screen*  scrn;
    cv::Mat img;
    std::string title;

    disp = XOpenDisplay(nullptr);
    scrn = DefaultScreenOfDisplay(disp);
    title = "Select area (SPACE or ENTER to confirm)";

    ScreenShot screen(0, 0, scrn->width, scrn->height);
    screen(img);

    Configurations::rect = selectROI(title, img);
    cv::destroyWindow(title);
}

/**
 * Set configuration variables and start the server.
 * @param   : void.
 * @return  : void.
*/
void FrmServer::on_btnStartServer_clicked()
{
    bool go_on = true;
    ui->btnStartServer->setEnabled(false);
    ui->btnStartServer->repaint();
    setConfigurations();
    if(Configurations::source_choices[Configurations::source] == "Video file")
    {
        Configurations::file_name = QFileDialog::getOpenFileName(this,
                           QString::fromStdString((*dict).getTextOpenafile()),
                           QDir::homePath(), "*.mkv").toStdString();
        if(Configurations::file_name == "")
        {
            go_on = false;
            ui->btnStartServer->setEnabled(true);
        }
        Configurations::video_chunk_seconds =
                (ui->cmbVideoChunk->currentIndex() * 5) + 10;
    }
    else
    {
        if(Configurations::source_choices[Configurations::source] == "Screen")
        {
            selectRegion();
            Configurations::frame_width = Configurations::rect.width;
            Configurations::frame_height = Configurations::rect.height;
        }
    }

    if(go_on)
    {
        *selector = 3;
        this->close();
    }
}

/**
 * Change gui according to source selection.
 * @param   : void.
 * @return  : void.
*/
void FrmServer::on_cmbSource_currentIndexChanged(int index)
{
    if(index == 0)
    {
        // Video file activated
        ui->lblVideoChunk->setEnabled(true);
        ui->cmbVideoChunk->setEnabled(true);
    }
    else
    {
        ui->lblVideoChunk->setEnabled(false);
        ui->cmbVideoChunk->setEnabled(false);
    }
}

/**
 * Change gui on Inttrusion detection checked/unchecked action.
 * @param   : void.
 * @return  : void.
*/
void FrmServer::on_chkIntrusionDetection_stateChanged(int arg1)
{
    if(arg1 == 0)
    {
        // Deactivated
        ui->lblNetwork->setEnabled(false);
        ui->cmbNetwork->setEnabled(false);
    }
    else
    {
        // Activated
        ui->lblNetwork->setEnabled(true);
        ui->cmbNetwork->setEnabled(true);
    }
}
