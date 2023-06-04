#include "qtext_edit.h"

qtext_edit::qtext_edit(QWidget* parent) : QTextEdit(parent)
{

}
void qtext_edit::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Return)
    {
        emit returnPressed();
    }
    else
    {
        QTextEdit::keyPressEvent(e);
    }

}
