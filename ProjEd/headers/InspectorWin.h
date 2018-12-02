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

#include "../../Render/headers/zs-math.h"

#define PEA_TYPE_NONE 0
#define PEA_TYPE_STRING 1
#define PEA_TYPE_FLOAT 2
#define PEA_TYPE_FLOAT2 3
#define PEA_TYPE_FLOAT3 4
#define PEA_TYPE_INT 5
#define PEA_TYPE_RESPICK 6

#define PICK_RES_TYPE_MESH 0
#define PICK_RES_TYPE_TEXTURE 1

class InspectorWin;
class ResourcePickDialog;

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

class PropertyEditArea{
public:
    int type;
    QString label; //Label, describing content
    QLabel* label_widget; //Widget for upper variable
    void* go_property; //Pointer to connected property

    QHBoxLayout* elem_layout; //Layout to contain everything

    PropertyEditArea(); //Default construct

    virtual ~PropertyEditArea(); //Destruct

    virtual void setup(); //Uses to prepare base values
    virtual void updateState(); //Updates value
    virtual void addToInspector(InspectorWin* win); //Add edit area to inspector layout
    void callPropertyUpdate(); //Call property, that created this area to update

    void setLabel(QString label);
};

class StringPropertyArea : public PropertyEditArea{
public:

    QString* value_ptr; //Modifying string
    QLineEdit* edit_field;

    StringPropertyArea();
    ~StringPropertyArea();

    void setup(); //Virtual, prepares base values
    void addToInspector(InspectorWin* win); //Add edit area to inspector layout
    void updateState(); //Updates value

};

class Float3PropertyArea : public PropertyEditArea{
public:

    QLineEdit* x_field; //Text digit field for X coord
    QLineEdit* y_field; //Text digit field for Y coord
    QLineEdit* z_field; //Text digit field for Z coord

    ZSVECTOR3* vector; //Output variable

    QLabel* x_label; //To write X separator
    QLabel* y_label; //To write Y separator
    QLabel* z_label; //To write Z separator

    Float3PropertyArea();
    ~Float3PropertyArea();

    void setup(); //Virtual
    void addToInspector(InspectorWin* win);
    void updateState(); //Virtual, on values changed
};

class PickResourceArea : public PropertyEditArea{
public:
    ResourcePickDialog* dialog;

    unsigned int resource_type;
    QString* rel_path; //Pointer to store result

    QPushButton* respick_btn;
    QLabel* relpath_label;
    PickResourceArea();
    ~PickResourceArea();

    void setup(); //Virtual, to prepare base values
    void addToInspector(InspectorWin* win);
    void updateState();
};

class FloatPropertyArea : public PropertyEditArea{
public:

    QLineEdit* float_field; //Text digit field for X coord

    float* value; //Output variable

    FloatPropertyArea();
    ~FloatPropertyArea();

    void setup(); //Virtual
    void addToInspector(InspectorWin* win);
    void updateState(); //Virtual, on values changed
};

class IntPropertyArea : public PropertyEditArea{
public:

    QLineEdit* int_field; //Text digit field for X coord

    int* value; //Output variable

    IntPropertyArea();
    ~IntPropertyArea();

    void setup(); //Virtual
    void addToInspector(InspectorWin* win);
    void updateState(); //Virtual, on values changed
};


namespace Ui {
class InspectorWin;
}

class InspectorWin : public QMainWindow
{
    Q_OBJECT
private:
    QPushButton* addObjComponentBtn;
public slots:
    void onAddComponentBtnPressed();

public:
    std::vector<PropertyEditArea*> property_areas; //vector for areas
    std::vector<QObject*> additional_objects;
    explicit InspectorWin(QWidget *parent = nullptr);
    ~InspectorWin();
    QVBoxLayout* getContentLayout();
    void clearContentLayout(); //Clears layout
    void addPropertyArea(PropertyEditArea* area); //Adds new property area
    void registerUiObject(QObject* object);
    void area_update(); //To update property areas states
    void makeAddObjComponentBtn(); //Adds "Create Property" Btn to content layout
    void ShowObjectProperties(void* object_ptr);
    void updateObjectProperties();
    void* gameobject_ptr;
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

class ResourcePickDialog : public QDialog{
    Q_OBJECT
public slots:
    void onNeedToShow();
    void onResourceSelected();
private:
    QGridLayout* contentLayout; //Layout to contain everything
public:
    QListWidget* list;
    PickResourceArea* area; //Pointer to area, that invoked this object
    QLabel* resource_text; //Pointer to label in area to write resource label

    ResourcePickDialog(QWidget* parent = nullptr);
    ~ResourcePickDialog();
};

#endif // INSPECTORWIN_H
