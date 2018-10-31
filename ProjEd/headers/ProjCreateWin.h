#ifndef PROJCREATEWIN_H
#define PROJCREATEWIN_H
#include <QMainWindow>

namespace Ui {
class CreateProjectWindow;
}

class CreateProjectWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void onCreateButtonPressed();
    void onBrowsePathButtonPressed();
public:
    void* mainwin_ptr;
    explicit CreateProjectWindow(QWidget *parent = nullptr);
    ~CreateProjectWindow();


private:
    QString root_path; //Need to store picked project root path
    Ui::CreateProjectWindow *ui;
};

#endif // PROJCREATEWIN_H
