#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QAbstractItemModel>
#include <QXmlStreamReader>
#include <QFile>
#include <QStandardItemModel>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Выберите файл"), "", tr("XML файлы (*.xml)"));
    if (!fileName.isEmpty()) {
        QDomDocument doc;
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            doc.setContent(&file);
            file.close();
            ui->textEdit->setPlainText(doc.toString());
            this->doc = doc;
            this->partNumbers.clear();
            QDomNodeList items = doc.elementsByTagName("Item");
            for (int i = 0; i < items.size(); ++i) {
                QDomElement item = items.at(i).toElement();
                partNumbers << item.attribute("PartNumber");
            }
            QStandardItemModel *model = new QStandardItemModel(this);
            foreach (const QString &partNumber, partNumbers) {
                QStandardItem *item = new QStandardItem(partNumber);
                model->appendRow(item);
            }
            ui->comboBox->setModel(model);
        }
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    if (!doc.isNull()) {
        QString partNumber = ui->comboBox->currentText();
        if (!partNumber.isEmpty()) {
            QDomNodeList items = doc.elementsByTagName("Item");
            for (int i = 0; i < items.size(); ++i) {
                QDomElement item = items.at(i).toElement();
                if (item.attribute("PartNumber") == partNumber) {
                    item.parentNode().removeChild(item);
                    partNumbers.removeOne(partNumber);
                    QStandardItemModel *model = new QStandardItemModel(this);
                    foreach (QString partNumber, partNumbers) {
                        QStandardItem *item = new QStandardItem(partNumber);
                        model->appendRow(item);
                    }
                    ui->comboBox->setModel(model);

                    break;
                }
            }
            ui->textEdit->setPlainText(doc.toString());
        }
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    if (!doc.isNull()) {
        QString partNumber = ui->lineEdit->text();
        if (!partNumber.isEmpty()) {
            QFile *file = new QFile("C:/Users/digod/Documents/qt/build-lab6-Desktop_Qt_6_6_3_MinGW_64_bit-Debug/ItemLibrary.xml");
            file->open(QIODevice::ReadOnly | QIODevice::Text);
            QXmlStreamReader stream(file);
            while (!stream.atEnd()) {
                stream.readNext();
                if (stream.isStartElement() && stream.name().toString() == "Item") {
                    if (stream.attributes().value("PartNumber") == partNumber) {

                        QDomElement newItem = doc.createElement("Item");
                        newItem.setAttribute("PartNumber", partNumber);
                        QDomElement productName = doc.createElement("ProductName");
                        QDomElement quantity = doc.createElement("Quantity");
                        QDomElement usprice = doc.createElement("USPrice");
                        QXmlStreamAttributes attributes = stream.attributes();

                        do{
                            stream.readNext();
                        if (stream.isStartElement() && stream.name().toString() == "ProductName")
                            productName.appendChild(doc.createTextNode(stream.readElementText()));
                        if (stream.isStartElement() && stream.name().toString() == "Quantity")
                            quantity.appendChild(doc.createTextNode(stream.readElementText()));
                        if (stream.isStartElement() && stream.name().toString() == "USPrice")
                            usprice.appendChild(doc.createTextNode(stream.readElementText()));
                        }while(!(stream.isEndElement() && stream.name().toString() == "USPrice"));

                        newItem.appendChild(productName);
                        newItem.appendChild(quantity);
                        newItem.appendChild(usprice);
                        QDomElement items = doc.documentElement().firstChildElement("Items");
                        items.appendChild(newItem);
                        partNumbers << partNumber;
                        QStandardItemModel *model = new QStandardItemModel(this);
                        foreach (const QString &partNumber, partNumbers) {
                            QStandardItem *item = new QStandardItem(partNumber);
                            model->appendRow(item);
                        }
                        ui->comboBox->setModel(model);
                        stream.skipCurrentElement();
                        break;
                    }
                }
            }
            ui->textEdit->setPlainText(doc.toString());
        }
    }
}


void MainWindow::on_pushButton_4_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Выберите файл"), "", tr("XML файлы (*.xml)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if(file.open(QIODevice::WriteOnly)){
            file.write(doc.toByteArray());
        }
        file.close();
    }
}


