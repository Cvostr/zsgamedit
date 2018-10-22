#ifndef INSPECTORWIN_H
#define INSPECTORWIN_H


#include <QMainWindow>
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


private:
    Ui::InspectorWin *ui;
};



#endif // INSPECTORWIN_H
