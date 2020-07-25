#ifndef LOC_STRING_EDIT_WIN
#define LOC_STRING_EDIT_WIN

#include <string>
#include <QMainWindow>
#include <QDialog>
#include <QGridLayout>
#include <QCheckbox>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTreeWidget>
#include <engine/LocaleStringsFile.h>

namespace Ui {
    class LocStringEditWindow;
}

class LocStringEditWindow : public QMainWindow
{
    Q_OBJECT
public slots:
    void onSave();
    void onNewString();

    void onStringsListItemClicked();
    void onStringsListItemDoubleClicked();
public:
    void addStringToUiList(ZSPIRE::LocString* str);
    void showWindowWithFile(std::string file);
    unsigned int getStringID(QTreeWidgetItem* id);

    void resizeEvent(QResizeEvent* event);
    void keyPressEvent(QKeyEvent*);
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

class StringEditDialogLine {
public:
    QLabel* label;
    QLineEdit* edit;

    StringEditDialogLine() {
        label = new QLabel;
        edit = new QLineEdit;
    }
    ~StringEditDialogLine() {
        delete label;
        delete edit;
    }
};

class StringEditDialog : public QDialog {
    Q_OBJECT
private:
    ZSPIRE::LocString* str;
    ZSPIRE::LocaleStringsFile* lsf;

    QGridLayout* contentLayout;
    QPushButton* apply_btn;

    StringEditDialogLine* str_id_line;
    StringEditDialogLine* lines;

    QFrame* line; //line to divide String ID field and other fields
public slots:
    void onApply();

public:
    StringEditDialog(ZSPIRE::LocString* str, ZSPIRE::LocaleStringsFile* lsf, QWidget* parent = nullptr);
    ~StringEditDialog();
};


#endif