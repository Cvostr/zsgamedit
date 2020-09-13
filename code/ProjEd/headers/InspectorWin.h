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
#include <QComboBox>
#include <QScrollArea>

#include <render/zs-math.h>
#include "../../Misc/headers/zs_types.h"

enum EDITAREA_TYPE {PEA_TYPE_NONE, PEA_TYPE_STRING, PEA_TYPE_FLOAT, PEA_TYPE_FLOAT2, PEA_TYPE_FLOAT3, PEA_TYPE_INT,
                    PEA_TYPE_INT2,
                   PEA_TYPE_RESPICK, PEA_TYPE_COLOR, PEA_TYPE_BOOL, PEA_TYPE_COMBOBOX,
                    PEA_TYPE_PROPPICK, PEA_TYPE_GOBJECT_PICK};


class InspectorWin;

class AreaPropertyTitle : public QObject{
    Q_OBJECT
public slots:
    void onActiveCheckboxPressed();
    void onDeleteButtonPressed();
public:
    Engine::GameObjectProperty* pProp;

    QVBoxLayout layout;
    QHBoxLayout widg_layout;

    QFrame line;

    QCheckBox checkbox;
    QPushButton delete_btn;
    QLabel prop_title;

    AreaPropertyTitle(Engine::GameObjectProperty* prop);
};

class AreaButton : public QObject{
    Q_OBJECT
public slots:
    void onButtonPressed();
public:
    QPushButton* button;
    void (*onPressFuncPtr)(); //Function pointer

    AreaButton();
    ~AreaButton(); //Destroy button widget
};

class AreaText : public QObject{
    Q_OBJECT
public:
    QLabel* label;

    AreaText();
    ~AreaText(); //Destroy button widget
};

class AreaRadioGroup : public QObject{
    Q_OBJECT
public slots:
    void onRadioClicked();
public:
    bool updateInspectorOnChange;
    Engine::GameObjectProperty* go_property; //Ponter to property, that created this group
    QVBoxLayout* btn_layout; //layout to contain everything
    std::vector<QRadioButton*> rad_buttons;
    uint8_t* value_ptr; //pointer to changing value

    void addRadioButton(QRadioButton* btn);
    AreaRadioGroup();
    ~AreaRadioGroup();
};

class PropertyEditArea {
public:
    EDITAREA_TYPE type; //Type of EditArea
    QString label; //Label, describing content
    QLabel* label_widget; //Widget for upper variable
    Engine::GameObjectProperty* go_property; //Pointer to connected property

    QHBoxLayout* elem_layout; //Layout to contain everything

    PropertyEditArea(); //Default construct
    virtual ~PropertyEditArea(); //Destruct

    void destroyLayout(); //Destoroy base layout
    virtual void destroyContent(); //Destroy content, placed by inherited class
    virtual void setup(); //Uses to prepare base values
    virtual void writeNewValues(); //if some text edited
    virtual void updateValues(); //if value changed by other method
    virtual void addToInspector(InspectorWin* win); //Add edit area to inspector layout
    void callPropertyUpdate(); //Call property, that created this area to update

    void setLabel(QString label);
};


namespace Ui {
class InspectorWin;
}

class InspectorWin : public QMainWindow
{
    Q_OBJECT
private:
    QPushButton* addObjComponentBtn;
    QPushButton* paintTileButton;

    QFrame* line; //line to divide props and control buttons
public slots:
    void onAddComponentBtnPressed();
    void onManagePropButtonPressed();
    void onPropertyChange();
    void onPaintTileClicked();
public:
    bool updateRequired; //Some properties may set that to TRUE to update inspector content
    bool updatePropertyStateLock;
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
    void ShowObjectProperties(Engine::GameObject* object_ptr);
    void updateObjectProperties();
    void addPropertyInterfaceToInspector(Engine::GameObjectProperty* property_ptr);

    void resizeEvent(QResizeEvent* event);

    Engine::GameObject* gameobject_ptr;
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
    QListWidget property_list; //list to store wgt list of props
    QLineEdit* comp_type;
    Engine::GameObject* g_object_ptr; //Pointer to object, when we'll add components

    AddGoComponentDialog(Engine::GameObject* game_object_ptr, QWidget* parent = nullptr);
    ~AddGoComponentDialog();
};


#endif // INSPECTORWIN_H
