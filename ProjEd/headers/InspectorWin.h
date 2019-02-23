#ifndef INSPECTORWIN_H
#define INSPECTORWIN_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QDialog>
#include <vector>
#include <QListWidget>
#include <QRadioButton>
#include <QColorDialog>
#include <QDialog>
#include <QCheckBox>
#include <QScrollArea>

#include "../../Render/headers/zs-math.h"

#define PEA_TYPE_NONE 0
#define PEA_TYPE_STRING 1
#define PEA_TYPE_FLOAT 2
#define PEA_TYPE_FLOAT2 3
#define PEA_TYPE_FLOAT3 4
#define PEA_TYPE_INT 5
#define PEA_TYPE_RESPICK 6
#define PEA_TYPE_COLOR 7
#define PEA_TYPE_BOOL 8

#define PICK_RES_TYPE_MESH 0
#define PICK_RES_TYPE_TEXTURE 1
#define PICK_RES_TYPE_AUDIO 2
#define PICK_RES_TYPE_FILE 1000

class InspectorWin;
class ResourcePickDialog;

class AreaPropertyTitle : public QObject{
    Q_OBJECT
public:
    QVBoxLayout layout;

    QFrame line;
    QLabel prop_title;

    AreaPropertyTitle();
};

class AreaButton : public QObject{
    Q_OBJECT
public slots:
    void onButtonPressed();
public:
    InspectorWin* insp_ptr;
    QPushButton* button;
    void (*onPressFuncPtr)(); //Function pointer

    AreaButton();
    ~AreaButton(); //Destroy button widget
};

class AreaRadioGroup : public QObject{
    Q_OBJECT
public slots:
    void onRadioClicked();
public:
    void* go_property; //Ponter to property, that created this group
    QVBoxLayout* btn_layout; //layout to contain everything
    std::vector<QRadioButton*> rad_buttons;
    uint8_t* value_ptr; //pointer to changing value

    void addRadioButton(QRadioButton* btn);
    AreaRadioGroup();
    ~AreaRadioGroup();
};

class PropertyEditArea {
public:
    int type;
    QString label; //Label, describing content
    QLabel* label_widget; //Widget for upper variable
    void* go_property; //Pointer to connected property

    QHBoxLayout* elem_layout; //Layout to contain everything

    PropertyEditArea(); //Default construct
    virtual ~PropertyEditArea(); //Destruct

    void destroyLayout(); //Destoroy base layout
    virtual void destroyContent(); //Destroy content, placed by inherited class
    virtual void setup(); //Uses to prepare base values
    virtual void writeNewValues(); //Updates value
    virtual void updateValues(); //if some text edited
    virtual void addToInspector(InspectorWin* win); //Add edit area to inspector layout
    void callPropertyUpdate(); //Call property, that created this area to update

    void setLabel(QString label);
};

class PickResourceArea : public PropertyEditArea{
public:
    ResourcePickDialog* dialog; //dialog shown after button press

    QString extension_mask; //Extension mask to show needed files
    unsigned int resource_type; //flag of resource type
    QString* rel_path; //Pointer to store result

    QPushButton* respick_btn; //button to press
    QLabel* relpath_label;
    PickResourceArea();
    ~PickResourceArea();

    void destroyContent(); //destroy content, created by this class
    void setup(); //Virtual, to prepare base values
    void addToInspector(InspectorWin* win);
    void updateState();
};


namespace Ui {
class InspectorWin;
}

class InspectorWin : public QMainWindow
{
    Q_OBJECT
private:
    QPushButton addObjComponentBtn;
    QPushButton managePropButton;

    QFrame line; //line to divide props and control buttons
public slots:
    void onAddComponentBtnPressed();
    void onManagePropButtonPressed();
    void onPropertyChange();

public:
    bool updateAreas; //if TRUE, all areas will update
    std::vector<PropertyEditArea*> property_areas; //vector for areas
    std::vector<QObject*> additional_objects;
    explicit InspectorWin(QWidget *parent = nullptr);
    ~InspectorWin();
    QVBoxLayout* getContentLayout();
    void clearContentLayout(); //Clears layout
    void addPropertyArea(PropertyEditArea* area); //Adds new property area
    void registerUiObject(QObject* object);
    void onPropertyEdited(); //To update property areas states on area edited
    void addPropButtons(); //Adds "Create Property" Btn to content layout
    void updateAreasChanges();
    void ShowObjectProperties(void* object_ptr);
    void updateObjectProperties();
    void* gameobject_ptr;
    void* editwindow_ptr;
private:
    Ui::InspectorWin *ui;

};

class AddGoComponentDialog : public QDialog{
    Q_OBJECT
private:

    QPushButton* add_btn;
    QPushButton* close_btn;

    QGridLayout* contentLayout;
public slots:
    void onAddButtonPressed();

public:
    QLineEdit* comp_type;
    void* g_object_ptr; //Pointer to object, when we'll add components

    AddGoComponentDialog(QWidget* parent = nullptr);
    ~AddGoComponentDialog();
};

class ManageComponentDialog;

class PropertyCtxMenu : public QObject{
    Q_OBJECT
public slots:
    void onDeleteClicked();
    void onPainClicked();

private:
    QMenu* menu; //Menu object to contain everything

    QAction* action_delete;
    QAction* action_paint_prop;

    InspectorWin* win;
    ManageComponentDialog* dialog;
public:

    PropertyCtxMenu(InspectorWin* win, ManageComponentDialog* dialog, QWidget* parent = nullptr);
    void show(QPoint point);
    void close();
};

class ManageComponentDialog : public QDialog{
    Q_OBJECT
private:
    QGridLayout contentLayout; //Layout to contain everything
    QPushButton close_btn;
    PropertyCtxMenu* ctx_menu; //Ctx menu, that shown on list item press
public slots:
    void onPropertyDoubleClick();
    void deleteProperty();

public:
    QListWidget property_list; //list to store wgt list of props

    InspectorWin* win;
    void* g_object_ptr; //Pointer to object, when we'll add components

    ManageComponentDialog(InspectorWin* win, void* g_object_ptr, QWidget* parent = nullptr);
    ~ManageComponentDialog();
};

class ResourcePickDialog : public QDialog{
    Q_OBJECT
public slots:
    void onNeedToShow();
    void onResourceSelected();
private:
    QGridLayout* contentLayout; //Layout to contain everything
    void findFiles(QString directory);
public:
    QString extension_mask; //Extension mask to show needed files
    QListWidget* list;
    PickResourceArea* area; //Pointer to area, that invoked this object
    QLabel* resource_text; //Pointer to label in area to write resource label

    ResourcePickDialog(QWidget* parent = nullptr);
    ~ResourcePickDialog();
};

class ColorDialogArea;

class ZSColorPickDialog : public QColorDialog{
    Q_OBJECT
public slots:
    void onNeedToShow();

public:
    ZSRGBCOLOR* color_ptr; //Pointer to write color
    ColorDialogArea* area_ptr;
    ZSColorPickDialog(QWidget* parent = nullptr);
};

class ColorDialogArea : public PropertyEditArea{

public:
    QLabel digit_str;
    ZSRGBCOLOR* color; //output value
    ZSColorPickDialog dialog; //Dialog pointer
    QPushButton pick_button; //button to show color pick dialog
    ColorDialogArea();

    void updText(); //updates text value
    void addToInspector(InspectorWin* win);
};


#endif // INSPECTORWIN_H
