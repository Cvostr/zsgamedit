#ifndef INSPECTORWIN_H
#define INSPECTORWIN_H


#include <QMainWindow>
#include <QVBoxLayout>
#include <vector>


namespace Ui {
class InspectorWin;
}

class InspectorWin : public QMainWindow
{
    Q_OBJECT

public slots:


public:

    explicit InspectorWin(QWidget *parent = nullptr);
    ~InspectorWin();
    QVBoxLayout* getContentLayout();
    void clearContentLayout();

private:
    Ui::InspectorWin *ui;
    int x_win_start;
};



#endif // INSPECTORWIN_H
