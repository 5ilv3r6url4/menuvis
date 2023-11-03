#include "main_viewer.h"
#include "ui_main_viewer.h"

#include <iostream>

Main_Viewer::Main_Viewer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Main_Viewer)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->ui_tabbed);

//    ui->ui_tabbed->addTab(new QWidget(), "Menu");
    ui->ui_tabbed->addTab(&m_window_recipes, "Recipes");
    ui->ui_tabbed->addTab(&m_window_pricing, "Ingredients");

//    QPushButton* btn_save = new QPushButton("Save");
//    QPushButton* btn_load = new QPushButton("Load");
}

Main_Viewer::~Main_Viewer()
{
    delete ui;
}
