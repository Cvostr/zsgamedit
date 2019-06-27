#ifndef INSPEDITAREAS_H
#define INSPEDITAREAS_H

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

#endif // INSPEDITAREAS_H
