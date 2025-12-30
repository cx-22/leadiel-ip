#ifndef TOPBAR_H
#define TOPBAR_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QComboBox>
#include <QFileDialog>

#include "pipeline.h"

class TopBar : public QWidget
{
    Q_OBJECT
public:
    explicit TopBar(QWidget *parent = nullptr);
    ~TopBar();
    TopBar();

    Pipeline* pipeline;

    QHBoxLayout* layout;

    QComboBox* file_combo;
    QComboBox* preset_combo;

    void loadImage(QString file_name = "");
    void saveImage();
};

#endif // TOPBAR_H
