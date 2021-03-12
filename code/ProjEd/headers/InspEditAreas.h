#ifndef INSPEDITAREAS_H
#define INSPEDITAREAS_H

#include "../../World/headers/World.h"
#include "InspectorWin.h"

class StringPropertyArea : public PropertyEditArea{
public:

    std::string* value_ptr; //Modifying string
    QLineEdit edit_field;

    StringPropertyArea();
    ~StringPropertyArea();

    void destroyContent(); //destroy content, created by this class
    void setup(); //Virtual, prepares base values
    void addToInspector(InspectorWin* win); //Add edit area to inspector layout
    void writeNewValues(); //Updates value
    void updateValues(); //on text fields or other edited from side

};

class Float4PropertyArea : public PropertyEditArea {
public:

    QLineEdit x_field; //Text digit field for X coord
    QLineEdit y_field; //Text digit field for Y coord
    QLineEdit z_field; //Text digit field for Z coord
    QLineEdit w_field; //Text digit field for Z coord

    Vec4* vector; //Output variable

    QLabel x_label; //To write X separator
    QLabel y_label; //To write Y separator
    QLabel z_label; //To write Z separator
    QLabel w_label; //To write W separator

    Float4PropertyArea();
    ~Float4PropertyArea();

    void destroyContent();
    void setup(); //Virtual
    void updateValues(); //on text fields or other edited
    void addToInspector(InspectorWin* win);
    void writeNewValues(); //Virtual, on values changed
};


class Float3PropertyArea : public PropertyEditArea{
public:

    QLineEdit x_field; //Text digit field for X coord
    QLineEdit y_field; //Text digit field for Y coord
    QLineEdit z_field; //Text digit field for Z coord

    Vec3* vector; //Output variable

    QLabel x_label; //To write X separator
    QLabel y_label; //To write Y separator
    QLabel z_label; //To write Z separator

    Float3PropertyArea();
    ~Float3PropertyArea();

    void destroyContent();
    void setup(); //Virtual
    void updateValues(); //on text fields or other edited
    void addToInspector(InspectorWin* win);
    void writeNewValues(); //Virtual, on values changed
};

class Float2PropertyArea : public PropertyEditArea{
public:

    QLineEdit x_field; //Text digit field for X coord
    QLineEdit y_field; //Text digit field for Y coord

    Vec2* vector; //Output variable

    QLabel x_label; //To write X separator
    QLabel y_label; //To write Y separator

    Float2PropertyArea();
    ~Float2PropertyArea();

    void destroyContent();
    void setup(); //Virtual
    void updateValues(); //on text fields or other edited
    void addToInspector(InspectorWin* win);
    void writeNewValues(); //Virtual, on values changed
};

class FloatPropertyArea : public PropertyEditArea{
public:

    QLineEdit float_field; //Text digit field for X coord

    float* value; //Output variable

    FloatPropertyArea();
    ~FloatPropertyArea();

    void setup(); //Virtual
    void addToInspector(InspectorWin* win);
    void writeNewValues(); //Virtual, on values changed
    void updateValues(); //on text fields or other edited
};

class IntPropertyArea : public PropertyEditArea{
public:
    //Text digit field for X coord
    QLineEdit* int_field;
    //Output variable
    int* value;

    IntPropertyArea();
    ~IntPropertyArea();

    void setup(); //Virtual
    void addToInspector(InspectorWin* win);
    void writeNewValues(); //Virtual, on values changed
    void updateValues();
};

class Int2PropertyArea : public PropertyEditArea{
public:

    QLineEdit x_field; //Text digit field for X coord
    QLineEdit y_field; //Text digit field for Y coord

    Vec2i* vector; //Output variable

    QLabel x_label; //To write X separator
    QLabel y_label; //To write Y separator

    Int2PropertyArea();
    ~Int2PropertyArea();

    void destroyContent();
    void setup(); //Virtual
    void updateValues(); //on text fields or other edited
    void addToInspector(InspectorWin* win);
    void writeNewValues(); //Virtual, on values changed
};

class BoolCheckboxArea : public PropertyEditArea{
public:
    bool updateInspectorOnChange;
    bool* pResultBool; //pointer to modifying bool
    QCheckBox checkbox; //pressing checkbox

    BoolCheckboxArea();
    void setup(); //Virtual
    void addToInspector(InspectorWin* win);
    void writeNewValues(); //Virtual, to check widget state
    void updateValues(); //bool value changed from side
};

class ComboBoxArea : public PropertyEditArea{
public:
    QComboBox widget;
    std::string* pResultString;

    void setup(); //Virtual
    void addToInspector(InspectorWin* win);
    void writeNewValues(); //Virtual, to check widget state

    ComboBoxArea();
};

class SpinBoxArea : public PropertyEditArea {
public:
    QSpinBox widget;
    int* pResultInt;

    void setup(); //Virtual
    void addToInspector(InspectorWin* win);
    void writeNewValues(); //Virtual, to check widget state

    SpinBoxArea();
};

class ResourcePickDialog;
class PickResourceArea;
class QLabelResourcePickWgt : public QLabel{
    Q_OBJECT
private:
    PropertyEditArea* area_ptr;
protected:
    void dragEnterEvent( QDragEnterEvent* event );
    void dropEvent( QDropEvent* event );
public:
    QLabelResourcePickWgt(PropertyEditArea* area_ptr, QWidget* parent = nullptr);
};

class PickResourceArea : public PropertyEditArea{
public:
    ResourcePickDialog* dialog; //dialog shown after button press

    bool isShowNoneItem; //If enabled, @none will be added to list
    QString extension_mask; //Extension mask to show needed files
    RESOURCE_TYPE resource_type; //flag of resource type

    std::string* pResultString; //Pointer to store result

    QPushButton* respick_btn; //button to press
    QLabelResourcePickWgt* relpath_label;
    PickResourceArea(RESOURCE_TYPE resource_type);
    ~PickResourceArea();

    void destroyContent(); //destroy content, created by this class
    void setup(); //Virtual, to prepare base values
    void addToInspector(InspectorWin* win);
    void updateValues();
    void updateLabel();
};

class ResourcePickDialog : public QDialog{
    Q_OBJECT
public slots:
    void onNeedToShow();
    void onResourceSelected();
    void onDialogClose();
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
    RGBAColor* color_ptr; //Pointer to write color
    ColorDialogArea* area_ptr;
    ZSColorPickDialog(QWidget* parent = nullptr);
};

class ColorDialogArea : public PropertyEditArea{

public:
    QLabel digit_str;
    RGBAColor* color; //output value
    ZSColorPickDialog dialog; //Dialog pointer
    QPushButton pick_button; //button to show color pick dialog
    ColorDialogArea();
    ~ColorDialogArea();

    void updText(); //updates text value
    void addToInspector(InspectorWin* win);
};


class PropertyPickArea : public PropertyEditArea{

public:
    PROPERTY_TYPE prop_type;

    Engine::IGameObjectComponent** property_ptr_ptr; //output value
    Engine::GameObjectLink link;

    QPushButton pick_button; //button to show color pick dialog
    QLabelResourcePickWgt* property_label;

    void setPropertyLink(ObjectPropertyLink* link);

    PropertyPickArea(PROPERTY_TYPE type);
    ~PropertyPickArea();

    void setup();
    void addToInspector(InspectorWin* win);
};

class GameobjectPickArea : public PropertyEditArea{

public:
    PROPERTY_TYPE prop_type;

    Engine::GameObject** gameobject_ptr_ptr; //output value
    QPushButton pick_button; //button to show color pick dialog
    QLabelResourcePickWgt* property_label;
    GameobjectPickArea();
    ~GameobjectPickArea();

    void setup();
    void addToInspector(InspectorWin* win);
};

#endif // INSPEDITAREAS_H
