



#ifndef FREECOIN_QT_COINCONTROLTREEWIDGET_H
#define FREECOIN_QT_COINCONTROLTREEWIDGET_H

#include <QKeyEvent>
#include <QTreeWidget>

class CoinControlTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit CoinControlTreeWidget(QWidget *parent = 0);

protected:
    virtual void keyPressEvent(QKeyEvent *event);
};

#endif // FREECOIN_QT_COINCONTROLTREEWIDGET_H
