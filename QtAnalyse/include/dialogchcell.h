#ifndef DIALOGCHCELL_H
#define DIALOGCHCELL_H

#include <QDialog>

namespace Ui {
class DialogChCell;
}

class DialogChCell : public QDialog
{
    Q_OBJECT

public:
    explicit DialogChCell(QWidget *parent = 0,const bool Display=true);
    ~DialogChCell();
    void setFile(QStringList list);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::DialogChCell *ui;

signals:
    void dataAvailable(int16_t,int16_t,int16_t,int64_t,int64_t);

};

#endif // DIALOGCHCELL_H
