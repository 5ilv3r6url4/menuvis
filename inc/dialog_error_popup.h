#ifndef DIALOG_ERROR_POPUP_H
#define DIALOG_ERROR_POPUP_H

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>

class Dialog_Error_Popup : public QWidget
{
    Q_OBJECT

public:

    explicit Dialog_Error_Popup(QString message, QWidget *parent = 0);
    ~Dialog_Error_Popup();

public slots:

    void slot_confirm();

signals:

};

#endif // DIALOG_ERROR_POPUP_H

