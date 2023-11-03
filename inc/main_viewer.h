#ifndef MAIN_VIEWER_H
#define MAIN_VIEWER_H

#include "ingredients_viewer.h"
#include "recipes_viewer.h"

#include <QMainWindow>
#include <QTabWidget>
#include <qtabbar.h>

namespace Ui {
class Main_Viewer;
}

class Main_Viewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit Main_Viewer(QWidget *parent = 0);
    ~Main_Viewer();

private:

    Ui::Main_Viewer*    ui;

    Ingredients_Viewer  m_window_pricing;
    Recipes_Viewer      m_window_recipes;

private slots:


signals:


};

#endif // MAIN_VIEWER_H
