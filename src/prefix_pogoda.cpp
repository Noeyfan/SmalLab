#include "prefix_pogoda.h"
#include "ui_prefix_pogoda.h"

PrefixPogoda::PrefixPogoda(QWidget *parent) :
    ConfigWindowBase(parent),
    ui(new Ui::PrefixPogoda),
    word_list(QStringList(), new QStringListModel(this))
{
    ui->setupUi(this);
    for(int i = 0; i < 3; ++i) {
        QStringList str;
        QStringListModel* model = new QStringListModel(this);
        affix_lists.emplace_back(str, model);
    }
    ui->prefix_list->setModel(affix_lists[0].second);
    ui->root_list->setModel(affix_lists[1].second);
    ui->suffix_list->setModel(affix_lists[2].second);
    ui->word_list->setModel(word_list.second);
}

PrefixPogoda::~PrefixPogoda()
{
    delete ui;
}

std::pair<int, int> PrefixPogoda::get_pos(QString q) {
    for(int i = 0; i < 3; ++i) {
        auto vec = i == 0 ? prefixs : i == 1 ? roots : suffixs;
        for(int j = 0; j < vec.size(); ++j) {
            if(!q.compare(vec[j].first)) return std::make_pair(i, j);
        }
    }
    return std::make_pair(-1, -1);
}

void PrefixPogoda::ReadXmlFileImp(QString filename) {
    XmlFileReader rxml(filename, "templeVocab", this);
    rxml.readNextStartElement();
    while(!rxml.atEnd()) {
        if(rxml.name() == "prefixList") {
            rxml.readNext();
            while(rxml.name() != "prefixList") {
                if(rxml.name() == "prefix") {
                } else if(rxml.name() == "value") {
                    QString value = rxml.readElementText();
                    rxml.readNext();
                    rxml.readNext();
                    QString definition = rxml.readElementText();
                    prefixs.push_back(std::make_pair(value, definition));
                    affix_add_helper(0, value);
                    ui->word_prefix->clear();
                    ui->word_prefix->addItem("");
                    ui->word_prefix->addItems(affix_lists[0].first);
                }
                rxml.readNext();
            }
        }else if(rxml.name() == "rootList") {
            rxml.readNext();
            while(rxml.name() != "rootList") {
                if(rxml.name() == "root") {
                } else if(rxml.name() == "value") {
                    QString value = rxml.readElementText();
                    rxml.readNext();
                    rxml.readNext();
                    QString definition = rxml.readElementText();
                    roots.push_back(std::make_pair(value, definition));
                    affix_add_helper(1, value);
                    ui->word_root->clear();
                    ui->word_root->addItem("");
                    ui->word_root->addItems(affix_lists[1].first);
                }
                rxml.readNext();
            }
        }else if(rxml.name() == "suffixList") {
            rxml.readNext();
            while(rxml.name() != "suffixList") {
                if(rxml.name() == "suffix") {
                    rxml.readNext();
                } else if(rxml.name() == "value") {
                    QString value = rxml.readElementText();
                    rxml.readNext();
                    rxml.readNext();
                    QString definition = rxml.readElementText();
                    suffixs.push_back(std::make_pair(value, definition));
                    affix_add_helper(2, value);
                    ui->word_suffix->clear();
                    ui->word_suffix->addItem("");
                    ui->word_suffix->addItems(affix_lists[2].first);
                    rxml.readNext();
                } else {
                    rxml.readNext();
                }
            }
        }else if(rxml.name() == "wordList") {
            rxml.readNext();
            while(rxml.name() != "wordList") {
                rxml.readNext();
                if(rxml.name() == "word") {
                    // qDebug() << "Entered";
                    rxml.readNextStartElement();
                    QString str = "", defin ="";
                    std::unordered_map<int, int> tmp_map;
                    tmp_map[0] = -1;tmp_map[1] = -1;tmp_map[2] = -1;
                    while(rxml.name() != "word") {
                        if(rxml.name() == "definition") {
                            defin = rxml.readElementText();
                        }else if(rxml.name() == "wordSegment") {
                            str += rxml.readElementText() + ";";
                        }else if(rxml.name() == "properRoot") {
                            QString tmp = rxml.readElementText();
                            int i, j;
                            std::tie(i, j) = get_pos(tmp);
                            if(i == -1 || j == -1) {
                                qDebug() << "Wrong infromation in XML file";
                                return;
                            }
                            tmp_map[i] = j;
                        }
                        rxml.readNext();
                    }
                    // insert a new Word into vector
                    assert(!(str.isEmpty()));
                    str.remove(str.size() - 1, 1);
                    words.emplace_back(tmp_map[0], tmp_map[1], tmp_map[2], str, defin);

                    // update Word List
                    word_list.first << removeSemi(str);
                    word_list.second->setStringList(word_list.first);
                    rxml.readNext();
                }
            }
        }
        rxml.readNext();
    }
}

void PrefixPogoda::WriteXmlFileImp(QString filename) {
    XmlFileWriter wxml(filename);
    wxml.setAutoFormatting(true);

    wxml.writeStartElement("templeVocab");
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

    wxml.writeStartElement("prefixList");
    for(const auto& ele : prefixs) {
        wxml.writeStartElement("prefix");
        WriteCDataString("value", ele.first);
        WriteCDataString("definition", ele.second);
        wxml.writeEndElement();
    }
    wxml.writeEndElement();

    wxml.writeStartElement("rootList");
    for(const auto& ele : roots) {
        wxml.writeStartElement("root");
        WriteCDataString("value", ele.first);
        WriteCDataString("definition", ele.second);
        wxml.writeEndElement();
    }
    wxml.writeEndElement();

    wxml.writeStartElement("suffixList");
    for(const auto& ele : suffixs) {
        wxml.writeStartElement("suffix");
        WriteCDataString("value", ele.first);
        WriteCDataString("definition", ele.second);
        wxml.writeEndElement();
    }
    wxml.writeEndElement();

    wxml.writeStartElement("wordList");
    for(const auto& ele : words) {
        wxml.writeStartElement("word");
        WriteCDataString("value", removeSemi(ele.word));
        WriteCDataString("definition", ele.desc);
        QStringList seg = ele.word.split(';');
        for(const auto& s : seg) {
            WriteCDataString("wordSegment", s);
        }
        if(ele.prefix_id != -1) WriteCDataString("properRoot", prefixs[ele.prefix_id].first);
        if(ele.root_id != -1) WriteCDataString("properRoot", roots[ele.root_id].first);
        if(ele.suffix_id != -1) WriteCDataString("properRoot", suffixs[ele.suffix_id].first);
        wxml.writeEndElement();
    }
    wxml.writeEndElement();

    wxml.writeEndElement();
}

bool PrefixPogoda::CheckEmpty() {

}

void PrefixPogoda::on_affix_add_pressed()
{
    auto ele = std::make_pair(ui->affix_input->text(), ui->affix_des->text());
    if(ele.first == "" || ele.second =="") return;
    switch (ui->affix_type->currentIndex()) {
    case 0:
        prefixs.push_back(ele);
        affix_add_helper(0, ele.first);
        ui->word_prefix->clear();
        ui->word_prefix->addItem("");
        ui->word_prefix->addItems(affix_lists[0].first);
        break;
    case 1:
        roots.push_back(ele);
        affix_add_helper(1, ele.first);
        ui->word_root->clear();
        ui->word_root->addItem("");
        ui->word_root->addItems(affix_lists[1].first);
        break;
    case 2:
        suffixs.push_back(ele);
        affix_add_helper(2, ele.first);
        ui->word_suffix->clear();
        ui->word_suffix->addItem("");
        ui->word_suffix->addItems(affix_lists[2].first);
        break;
    default:
        break;
    }
}

void PrefixPogoda::on_affix_update_clicked()
{
}

void PrefixPogoda::affix_add_helper(int idx, QString s) {
    affix_lists[idx].first << s;
    affix_lists[idx].second->setStringList(affix_lists[idx].first);
}

QString PrefixPogoda::removeSemi(QString qs) {
    QString ret = "";
    for(const auto& ele : qs) {
        if(ele == ';') continue;
        ret += ele;
    }
    return ret;
}

void PrefixPogoda::on_word_add_pressed()
{
    if(ui->word_input->text() == "" || ui->word_des->text() == "") return;
    words.emplace_back(
                ui->word_prefix->currentIndex() - 1,
                ui->word_root->currentIndex() - 1,
                ui->word_suffix->currentIndex() - 1,
                ui->word_input->text(),
                ui->word_des->text()
                );

    word_list.first << ui->word_input->text();
    word_list.second->setStringList(word_list.first);
}

void PrefixPogoda::on_word_update_pressed()
{

}

void PrefixPogoda::on_word_delete_pressed()
{

}

void PrefixPogoda::on_word_list_clicked(const QModelIndex &index)
{
    ui->word_input->setText(words[index.row()].word);
    ui->word_des->setText(words[index.row()].desc);
    ui->word_prefix->setCurrentIndex(words[index.row()].prefix_id + 1);
    ui->word_root->setCurrentIndex(words[index.row()].root_id + 1);
    ui->word_suffix->setCurrentIndex(words[index.row()].suffix_id + 1);
    // add one to fill the blank
}

void PrefixPogoda::on_prefix_list_clicked(const QModelIndex &index)
{
    ui->affix_input->setText(prefixs[index.row()].first);
    ui->affix_des->setText(prefixs[index.row()].second);
    ui->affix_type->setCurrentIndex(0);
}

void PrefixPogoda::on_root_list_clicked(const QModelIndex &index)
{
    ui->affix_input->setText(roots[index.row()].first);
    ui->affix_des->setText(roots[index.row()].second);
    ui->affix_type->setCurrentIndex(1);
}

void PrefixPogoda::on_suffix_list_clicked(const QModelIndex &index)
{
    ui->affix_input->setText(suffixs[index.row()].first);
    ui->affix_des->setText(suffixs[index.row()].second);
    ui->affix_type->setCurrentIndex(2);
}