#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QMenu>
#include <QCompleter>
#include <vector>
#include <string>

#include "my_line_edit.h"
#include "functions.h"


class FunctionItemWidget : public QWidget
{
    Q_OBJECT
public:
    FunctionItemWidget(const FunctionData& func, QWidget* parent = nullptr);
    FunctionData func_data;

signals:
    void parameterChanged();
};

class SideBar : public QWidget
{
    Q_OBJECT
public:
    explicit SideBar(QWidget* parent = nullptr);

    MyLineEdit* search_line;
    QPushButton* delete_button;
    QListWidget* function_list;
    QCompleter* completer;
    QPushButton* run_button;
    bool delete_mode = false;
    std::vector<FunctionState> function_states;

    void updateFunctionStates();
    void printList();

    void addFunction(const FunctionData& func);
    void duplicateItem(QListWidgetItem* item);
    void removeItem(QListWidgetItem* item);

    void onSearchChanged(const QString& text);
    void onItemRightClicked(const QPoint& pos);
    void onItemClicked(QListWidgetItem* item);
    void onDeleteButtonClicked();

    void onParameterChanged();
    void onItemMoved();


    FunctionItemWidget* createFunctionWidget(const FunctionData& func);
};

#endif // SIDEBAR_H
