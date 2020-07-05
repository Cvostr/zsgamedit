#ifndef LOC_STRING_EDIT_WIN
#define LOC_STRING_EDIT_WIN

#include <string>
#include <QMainWindow>
#include <QDialog>
#include <QGridLayout>
#include <QCheckbox>
#include <QPushButton>
#include <engine/LocaleStringsFile.h>

namespace Ui {
    class LocStringEditWindow;
}

class LocStringEditWindow : public QMainWindow
{
    Q_OBJECT
public slots:
    void onSave();
public:

    void showWindowWithFile(std::string file);

    explicit LocStringEditWindow(QWidget* parent = nullptr);
    ~LocStringEditWindow();

private:
    ZSPIRE::LocaleStringsFile* lsf;
    std::string file_path;
    Ui::LocStringEditWindow* ui;
};


class LanguagesDialog : public QDialog {
    Q_OBJECT
private:
    ZSPIRE::LocaleStringsFile* lsf;
    QCheckBox** lang_checkboxes;
    QPushButton* apply_btn;
    QGridLayout* contentLayout;
public slots:
    void onApply();

public:

    LanguagesDialog(ZSPIRE::LocaleStringsFile* lsf, QWidget* parent = nullptr);
    ~LanguagesDialog();
};

#endif