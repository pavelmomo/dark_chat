#ifndef QTEXT_EDIT_H
#define QTEXT_EDIT_H

#include <QTextEdit>
#include <QWidget>
#include <QKeyEvent>
#include <QDebug>


class qtext_edit : public QTextEdit
{
    Q_OBJECT
public:
    qtext_edit(QWidget* parent = nullptr);
    void keyPressEvent(QKeyEvent *e) override;
signals:
    void returnPressed();
};

#endif // QTEXT_EDIT_H
