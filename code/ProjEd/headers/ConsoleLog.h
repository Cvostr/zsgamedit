#ifndef CONSOLE_LOG_H
#define CONSOLE_LOG_H

#include <QMainWindow>
#include <QResizeEvent>
#include <QListWidget>
#include <game.h>

namespace Ui {
    class ConsoleLog;
}

class ConsoleLog : public QMainWindow
{
    Q_OBJECT

public slots:
    void currentTabChanged(int index);
    void clearLogsOnCurrentType();
public:

    void clearLogsOnType(LogEntryType type);
    void resizeEvent(QResizeEvent* event);
    void keyPressEvent(QKeyEvent*);

    void updateLogsList();

    explicit ConsoleLog(QWidget* parent = nullptr);
    ~ConsoleLog();

private:
    QTabWidget* tabs;
    QListWidget* logs_list;
    LogEntryType leType;

    Ui::ConsoleLog* ui;
};


#endif