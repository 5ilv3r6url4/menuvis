#ifndef DIALOG_RECIPE_CATEGORY_ADD_H
#define DIALOG_RECIPE_CATEGORY_ADD_H

#include <QWidget>
#include <QShortcut>
#include <QApplication>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

class Dialog_Recipe_Category_Add : public QWidget
{
    Q_OBJECT

public:

    Dialog_Recipe_Category_Add(QWidget *parent = 0);
    ~Dialog_Recipe_Category_Add();

private:

    QString capitalize(const QString& s);

    QPushButton*    m_button_create;
    QLabel*         m_label_category;       QLineEdit*      m_input_category;
    QLabel*         m_label_status;

private slots:

    void slot_create_category();
    void slot_create_category_response(int index);

signals:

    void sig_create_category(QString);

};

#endif // DIALOG_RECIPE_CATEGORY_ADD_H
