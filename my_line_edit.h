#ifndef MY_LINE_EDIT_H
#define MY_LINE_EDIT_H

#include <QLineEdit>
#include <QKeyEvent>
#include <QCompleter>
#include <qabstractitemview.h>

class MyLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit MyLineEdit(QWidget* parent = nullptr)
        : QLineEdit(parent) {}

protected:
    void keyPressEvent(QKeyEvent* event) override
    {
        if (event->key() == Qt::Key_Tab && completer())
        {
            if (completer()->completionCount() > 0)
            {
                QString first = completer()->completionModel()->index(0, 0).data().toString();
                setText(first);
                selectAll(); // optional
                event->accept();
                return;
            }
        }

        QLineEdit::keyPressEvent(event);
    }


    bool focusNextPrevChild(bool) override
    {
        // Disable Qt’s default Tab focus behavior
        return false;
    }
};


#endif // MY_LINE_EDIT_H
