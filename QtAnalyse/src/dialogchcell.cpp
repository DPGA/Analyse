#include "dialogchcell.h"
#include "ui_dialogchcell.h"

DialogChCell::DialogChCell(QWidget *parent,const bool Display) :
    QDialog(),
    ui(new Ui::DialogChCell)
{
    ui->setupUi(this);

    ui->SelectChannel->setVisible(Display);
    ui->SelectCell->setVisible(Display);
    ui->SelectNbEventsBegin->setVisible(Display);
    ui->SelectNbEventsEnd->setVisible(Display);

    for (int Index=0;Index<24;Index++)
        ui->SelectChannel->addItem(QString::number(Index,10));

        
}

DialogChCell::~DialogChCell()
{
    delete ui;
}

void DialogChCell::on_buttonBox_accepted()
{

    emit dataAvailable(ui->ListFile->currentIndex(),ui->SelectChannel->currentIndex(),ui->SelectCell->value(),ui->SelectNbEventsBegin->value(),ui->SelectNbEventsEnd->value());
}

void DialogChCell::on_buttonBox_rejected()
{
    emit dataAvailable(-1,-1,-1,-1,-1);
}

void DialogChCell::setFile(QStringList list)
{
    for (int i=0;i< list.size();i++) ui->ListFile->addItem(list.at(i));
}
