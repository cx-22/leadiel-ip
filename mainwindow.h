#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "topbar.h"
#include "sidebar.h"
#include "viewer.h"
#include "pipeline.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QWidget* main_widget;
    QVBoxLayout* layout;
    QHBoxLayout* bottom_layout;

    TopBar* topbar;
    SideBar* sidebar;
    Viewer* viewer;
    Pipeline* pipeline;
};

#endif // MAINWINDOW_H
