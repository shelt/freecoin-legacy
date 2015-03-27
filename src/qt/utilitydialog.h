



#ifndef FREECOIN_QT_UTILITYDIALOG_H
#define FREECOIN_QT_UTILITYDIALOG_H

#include <QDialog>
#include <QObject>

class FreecoinGUI;
class ClientModel;

namespace Ui {
    class HelpMessageDialog;
}

/** "Help message" dialog box */
class HelpMessageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HelpMessageDialog(QWidget *parent, bool about);
    ~HelpMessageDialog();

    void printToConsole();
    void showOrPrint();

private:
    Ui::HelpMessageDialog *ui;
    QString text;

private slots:
    void on_okButton_accepted();
};


/** "Shutdown" window */
class ShutdownWindow : public QWidget
{
    Q_OBJECT

public:
    ShutdownWindow(QWidget *parent=0, Qt::WindowFlags f=0);
    static void showShutdownWindow(FreecoinGUI *window);

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // FREECOIN_QT_UTILITYDIALOG_H
