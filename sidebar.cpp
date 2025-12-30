#include "sidebar.h"
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStringListModel>
#include <QCompleter>
#include <QMenu>
#include <iostream>

extern std::vector<FunctionData> g_function_data;

FunctionItemWidget::FunctionItemWidget(const FunctionData& func, QWidget* parent)
    : QWidget(parent), func_data(func)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* name_label = new QLabel(QString::fromStdString(func_data.name));
    layout->addWidget(name_label);

    for (size_t i = 0; i < func_data.parameters.size(); ++i)
    {
        Parameter& p = func_data.parameters[i];
        QHBoxLayout* row = new QHBoxLayout;
        QLabel* label = new QLabel(QString::fromStdString(p.label));
        QDoubleSpinBox* spin = new QDoubleSpinBox;
        spin->setRange(p.min_value, p.max_value);
        spin->setValue(p.value);
        spin->setDecimals(p.decimals);
        spin->setSingleStep(p.step);

        connect(spin, &QDoubleSpinBox::valueChanged, [&p, this](double val){
            p.value = val;
            emit parameterChanged();
        });

        row->addWidget(label);
        row->addWidget(spin);
        layout->addLayout(row);
    }


    layout->addStretch();
}

SideBar::SideBar(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* main_layout = new QVBoxLayout(this);

    QHBoxLayout* top_layout = new QHBoxLayout;
    search_line = new MyLineEdit;
    search_line->setPlaceholderText("Search and add function...");
    top_layout->addWidget(search_line);

    delete_button = new QPushButton("Mass Delete");
    top_layout->addWidget(delete_button);
    main_layout->addLayout(top_layout);


    run_button = new QPushButton("Run");
    top_layout->addWidget(run_button);

    function_list = new QListWidget;
    function_list->setDragDropMode(QAbstractItemView::InternalMove);
    function_list->setSelectionMode(QAbstractItemView::SingleSelection);
    function_list->setContextMenuPolicy(Qt::CustomContextMenu);
    main_layout->addWidget(function_list);

    completer = new QCompleter(this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);

    search_line->setCompleter(completer);

    connect(search_line, &QLineEdit::textChanged, [this](const QString &){
        QStringList names;
        for (const auto& func : g_function_data)
            names << QString::fromStdString(func.name);
        completer->setModel(new QStringListModel(names, completer));
    });

    // add function on Enter
    connect(completer, QOverload<const QString &>::of(&QCompleter::activated),
            [this](const QString &text){
                for (const auto& func : g_function_data)
                {
                    if (QString::fromStdString(func.name).compare(text, Qt::CaseInsensitive) == 0)
                    {
                        addFunction(func);
                        search_line->clear();
                        break;
                    }
                }
            });

    connect(search_line, &QLineEdit::returnPressed, [this]() {
        QString text = search_line->text();
        if (text.isEmpty()) return;

        for (const auto& func : g_function_data)
        {
            if (QString::fromStdString(func.name).compare(text, Qt::CaseInsensitive) == 0)
            {
                addFunction(func);
                search_line->clear();
                break;
            }
        }
    });


    connect(function_list, &QListWidget::customContextMenuRequested,
            this, &SideBar::onItemRightClicked);

    connect(function_list, &QListWidget::itemClicked,
            this, &SideBar::onItemClicked);

    connect(delete_button, &QPushButton::clicked,
            this, &SideBar::onDeleteButtonClicked);

    connect(function_list->model(), &QAbstractItemModel::rowsMoved,
            this, &SideBar::updateFunctionStates);

}

void SideBar::printList()
{
    for (const auto& f : function_states)
    {
        std::cout << f.id;
        for (float v : f.params)
            std::cout << " - " << v;
        std::cout << std::endl;
    }
}

void SideBar::onDeleteButtonClicked()
{
    delete_mode = !delete_mode;
    delete_button->setText(delete_mode ? "Exit Delete Mode" : "Mass Delete");
}

void SideBar::onItemClicked(QListWidgetItem* item)
{
    if (!delete_mode) return;
    removeItem(item);
}

FunctionItemWidget* SideBar::createFunctionWidget(const FunctionData& func)
{
    return new FunctionItemWidget(func, this);
}

void SideBar::addFunction(const FunctionData& func)
{
    QListWidgetItem* item = new QListWidgetItem;
    FunctionItemWidget* widget = createFunctionWidget(func);

    connect(widget, &FunctionItemWidget::parameterChanged,
            this, &SideBar::updateFunctionStates);

    item->setSizeHint(widget->sizeHint());
    function_list->addItem(item);
    function_list->setItemWidget(item, widget);

    updateFunctionStates();
}

void SideBar::onItemRightClicked(const QPoint& pos)
{
    QListWidgetItem* item = function_list->itemAt(pos);
    if (!item) return;

    QMenu menu;
    QAction* duplicate_action = menu.addAction("Duplicate");
    QAction* remove_action = menu.addAction("Remove");

    QAction* selected = menu.exec(function_list->mapToGlobal(pos));
    if (selected == duplicate_action)
        duplicateItem(item);
    else if (selected == remove_action)
        removeItem(item);
}

void SideBar::duplicateItem(QListWidgetItem* item)
{
    QWidget* widget = function_list->itemWidget(item);
    if (!widget) return;

    FunctionItemWidget* orig = qobject_cast<FunctionItemWidget*>(widget);
    if (!orig) return;

    FunctionData copy_data = orig->func_data;

    QListWidgetItem* new_item = new QListWidgetItem;
    FunctionItemWidget* new_widget = createFunctionWidget(copy_data);

    connect(new_widget, &FunctionItemWidget::parameterChanged,
            this, &SideBar::updateFunctionStates);

    new_item->setSizeHint(new_widget->sizeHint());
    int row = function_list->row(item);
    function_list->insertItem(row + 1, new_item);
    function_list->setItemWidget(new_item, new_widget);

    updateFunctionStates();
}

void SideBar::removeItem(QListWidgetItem* item)
{
    int row = function_list->row(item);
    QWidget* widget = function_list->itemWidget(item);
    if (widget) delete widget;
    delete function_list->takeItem(row);

    updateFunctionStates();
}

void SideBar::updateFunctionStates()
{
    function_states.clear();
    for (int i = 0; i < function_list->count(); ++i)
    {
        QListWidgetItem* item = function_list->item(i);
        FunctionItemWidget* widget = qobject_cast<FunctionItemWidget*>(function_list->itemWidget(item));
        if (!widget) continue;

        FunctionState state;
        state.id = widget->func_data.id;
        state.params.clear();
        for (auto& param : widget->func_data.parameters)
            state.params.push_back(param.value);

        function_states.push_back(state);
    }
}



