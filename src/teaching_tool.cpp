#include "teaching_tool.h"
#include "ui_teaching_tool.h"

TeachingTool::TeachingTool(QWidget *parent) :
    ConfigWindowBase(parent),
    ui(new Ui::TeachingTool),
    level_list(QStringList(), new QStringListModel(this))
{
    ui->setupUi(this);
    ui->levels->setModel(level_list.second);
}

TeachingTool::~TeachingTool()
{
    delete ui;
}

void TeachingTool::ReadXmlFileImp(QString) {

}

void TeachingTool::WriteXmlFileImp(QString filename) {
    XmlFileWriter wxml(filename);
    wxml.setAutoFormatting(true);

    wxml.writeStartElement("ToolCustomization");
    wxml.writeStartElement("LevelList");

    // lambda for write CData
    auto WriteCDataNumb = [&wxml](QString tag, double numb) {
        wxml.writeStartElement(tag);
        wxml.writeCDATA(QString::number(numb));
        wxml.writeEndElement();
    };
    auto WriteCDataString = [&wxml](QString tag, QString s) {
        wxml.writeStartElement(tag);
        wxml.writeCDATA(s);
        wxml.writeEndElement();
    };

    for(const auto& ele : levels) {
        wxml.writeStartElement("level");
        WriteCDataString("imagepath", ele.imagename);
        WriteCDataString("imagename", ele.imagepath);
        WriteCDataNumb("scenenumberofimage", ele.soi);
        WriteCDataString("textcontent", ele.textcontent);
        WriteCDataNumb("scenenumberoftext", ele.sot);
        WriteCDataString("nameofvideo", ele.videoname);
        WriteCDataString("videourl", ele.videourl);
        WriteCDataNumb("scenenumberofvideo", ele.sov);
        wxml.writeEndElement();
    }
    wxml.writeEndElement();
    wxml.writeEndDocument();
}

bool TeachingTool::CheckEmpty() {

}

QString TeachingTool::HelpImp() {
}

void TeachingTool::on_load_button_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this,tr("Open image File"),
                                                     QStandardPaths::locate(
                                                         QStandardPaths::DesktopLocation,
                                                         QString(),
                                                         QStandardPaths::LocateDirectory),
                                                     tr("image Files (*.jpg *.png *.gif *.tiff *.jpeg)"));
    ui->img_path_input->setText(file_name);
    auto sep = file_name.split('/');
    ui->img_name_input->setText(sep[sep.size() - 1]);
}

void TeachingTool::on_pushButton_2_clicked()
{
    // add level
    if(ui->img_name_input->text() == "") return;
    levels.emplace_back(ui->img_path_input->text(), ui->img_name_input->text(),
                        ui->text_content_input->text(), ui->video_name_input->text(), ui->video_url_input->text(),
                        ui->soi->text().toInt(), ui->sot->text().toInt(), ui->sov->text().toInt());
    level_list.first << ui->img_name_input->text();
    level_list.second->setStringList(level_list.first);
    ui->img_path_input->clear(); ui->img_name_input->clear();
    ui->text_content_input->clear(); ui->video_url_input->clear();
    ui->video_name_input->clear();
    ui->soi->clear();
    ui->sot->clear();
    ui->sov->clear();
}

void TeachingTool::on_levels_clicked(const QModelIndex &index)
{
    auto ele = levels[index.row()];
    ui->img_path_input->setText(ele.imagepath); ui->img_name_input->setText(ele.imagename);
    ui->text_content_input->setText(ele.textcontent);
    ui->video_name_input->setText(ele.videoname);
    ui->video_url_input->setText(ele.videourl);
    ui->soi->setText(QString::number(ele.soi));
    ui->sot->setText(QString::number(ele.sot));
    ui->sov->setText(QString::number(ele.sov));
}

void TeachingTool::on_pushButton_clicked()
{
    // update
    int idx = ui->levels->currentIndex().row();
    if(idx < 0) return;
    levels[idx] = TT_Ele(ui->img_path_input->text(), ui->img_name_input->text(),
                         ui->text_content_input->text(), ui->video_name_input->text(), ui->video_url_input->text(),
                         ui->soi->text().toInt(), ui->sot->text().toInt(), ui->sov->text().toInt());
    ui->img_path_input->clear(); ui->img_name_input->clear();
    ui->text_content_input->clear(); ui->video_url_input->clear();
    ui->video_name_input->clear();
    ui->soi->clear();
    ui->sot->clear();
    ui->sov->clear();
}

void TeachingTool::on_pushButton_3_clicked()
{
    int idx = ui->levels->currentIndex().row();
    if(idx < 0) return;
    levels.erase(levels.begin() + idx);
    level_list.first.erase(level_list.first.begin() + idx);
    level_list.second->setStringList(level_list.first);
    ui->img_path_input->clear(); ui->img_name_input->clear();
    ui->text_content_input->clear(); ui->video_url_input->clear();
    ui->video_name_input->clear();
    ui->soi->clear();
    ui->sot->clear();
    ui->sov->clear();
}
