#ifndef INSPEDITAREAS_H
#define INSPEDITAREAS_H

#include "../../World/headers/World.h"
#include "InspectorWin.h"

class StringPropertyArea : public PropertyEditArea{
public:

    QString* value_ptr; //Modifying string
    QLineEdit edit_field;

    StringPropertyArea();
    ~StringPropertyArea();

    void destroyContent(); //destroy content, created by this class
    void setup(); //Virtual, prepares base values
    void addToInspector(InspectorWin* win); //Add edit area to inspector layout
    void writeNewValues(); //Updates value
    void updateValues(); //on text fields or other edited from side

};

class Float3PropertyArea : public PropertyEditArea{
public:

    QLineEdit x_field; //Text digit field for X coord
    QLineEdit y_field; //Text digit field for Y coord
    QLineEdit z_field; //Text digit field for Z coord

    ZSVECTOR3* vector; //Output variable

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

    QLineEdit* int_field; //Text digit field for X coord

    int* value; //Output variable

    IntPropertyArea();
    ~IntPropertyArea();

    void setup(); //Virtual
    void addToInspector(InspectorWin* win);
    void writeNewValues(); //Virtual, on values changed
    void updateValues();
};

class BoolCheckboxArea : public PropertyEditArea{
public:
    bool* bool_ptr; //pointer to modifying bool
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
    QString* result_string;

    void setup(); //Virtual
    void addToInspector(InspectorWin* win);
    void writeNewValues(); //Virtual, to check widget state

    ComboBoxArea();
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
    QString* rel_path; //Pointer to store result

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
    ~ColorDialogArea();

    void updText(); //updates text value
    void addToInspector(InspectorWin* win);
};


class PropertyPickArea : public PropertyEditArea{

public:
    PROPERTY_TYPE prop_type;

    GameObjectProperty** property_ptr_ptr; //output value
    QPushButton pick_button; //button to show color pick dialog
    QLabelResourcePickWgt* property_label;
    PropertyPickArea(PROPERTY_TYPE type);
    ~PropertyPickArea();

    void setup();
    void addToInspector(InspectorWin* win);
};

#endif // INSPEDITAREAS_H
