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

class PropertyEditArea{
public:
    int type;
    QString label; //Label, describing content
    QLabel* label_widget; //Widget for upper variable
    void* go_property; //Pointer to connected property

    QHBoxLayout* elem_layout; //Layout to contain everything

    PropertyEditArea(); //Default construct

    virtual ~PropertyEditArea();


    virtual void setup(); //Uses to prepare base values
    virtual void updateState(); //Updates value
    virtual void clear(InspectorWin* win); //Clears all widget in layout
    virtual void addToInspector(InspectorWin* win); //Add edit area to inspector layout

    void setLabel(QString label);
};

class StringPropertyArea : public PropertyEditArea{
public:

    QString* value_ptr; //Modifying string
    QLineEdit* edit_field;

    StringPropertyArea();
    ~StringPropertyArea();

    void setup(); //Virtual, prepares base values
    void clear(InspectorWin* win); //Virtual, clears all qt classes
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
    void clear(InspectorWin* win); //Virtual, clears all qt classes
    void updateState(); //Virtual, on values changed
};

class PickResourceArea : public PropertyEditArea{
public:
    ResourcePickDialog* dialog;

    unsigned int resource_type;
    QString* mesh_rel_path; //Pointer to store result

    QPushButton* respick_btn;

    PickResourceArea();
    ~PickResourceArea();

    //void setup(); //Virtual, to prepare base values
    void addToInspector(InspectorWin* win);
};

class FloatPropertyArea : public PropertyEditArea{
public:

    QLineEdit* x_field; //Text digit field for X coord

    float* value; //Output variable

    FloatPropertyArea();
    ~FloatPropertyArea();

    void setup(); //Virtual
    void addToInspector(InspectorWin* win);
    void clear(InspectorWin* win); //Virtual, clears all qt classes
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
    explicit InspectorWin(QWidget *parent = nullptr);
    ~InspectorWin();
    QVBoxLayout* getContentLayout();
    void clearContentLayout(); //Clears layout
    void addPropertyArea(PropertyEditArea* area); //Adds new property area
    void area_update(); //To update property areas states
    void makeAddObjComponentBtn(); //Adds "Create Property" Btn to content layout
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
private:
    QGridLayout* contentLayout;
public:
    QListWidget* list;
    PickResourceArea* area;

    ResourcePickDialog(QWidget* parent = nullptr);
    ~ResourcePickDialog();
};

#endif // INSPECTORWIN_H
