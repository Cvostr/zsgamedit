#include "headers/InspectorWin.h"
#include "headers/ProjectEdit.h"

#include "../World/headers/World.h"
#include <QDoubleValidator>
#include <QObject>


//Float3 definations
Float3PropertyArea::Float3PropertyArea(){
    vector = nullptr; //set it to null to avoid crash;
    type = PEA_TYPE_FLOAT3;

    x_field = new QLineEdit; //Allocation of text fields
    y_field = new QLineEdit;
    z_field = new QLineEdit;

    //Allocating separator labels
    x_label = new QLabel;
    x_label->setText(QString("X"));
    y_label = new QLabel;
    y_label->setText(QString("Y"));
    z_label = new QLabel;
    z_label->setText(QString("Z"));

    label_widget->setFixedWidth(100);

    QLocale locale(QLocale::English); //Define english locale to set it to double validator later
    QDoubleValidator* validator = new QDoubleValidator(-100, 100, 6, nullptr); //Define double validator
    validator->setLocale(locale); //English locale to accept dost instead of commas

    elem_layout->addWidget(x_label); //Adding X label
    x_field->setFixedWidth(60);
    x_field->setValidator(validator); //Set double validator
    elem_layout->addWidget(x_field); //Adding X text field

    elem_layout->addWidget(y_label);
    y_field->setFixedWidth(60);
    y_field->setValidator(validator);
    elem_layout->addWidget(y_field);

    elem_layout->addWidget(z_label);
    z_field->setFixedWidth(60);
    z_field->setValidator(validator);
    elem_layout->addWidget(z_field);
}

void Float3PropertyArea::addToInspector(InspectorWin* win){
    win->getContentLayout()->addLayout(this->elem_layout);
}

void Float3PropertyArea::updateState(){
    if(this->vector == nullptr) //If vector hasn't been set
        return; //Go out
    //Get current values in text fields
    float vX = this->x_field->text().toFloat();
    float vY = this->y_field->text().toFloat();
    float vZ = this->z_field->text().toFloat();
    //Get current values in out vector ptr
    float vptrX = this->vector->X;
    float vptrY = this->vector->Y;
    float vptrZ = this->vector->Z;
    //Compare them
    if(vX != vptrX || vY != vptrY || vZ != vptrZ){ //If it updated
        this->vector->X = vX;
        this->vector->Y = vY;
        this->vector->Z = vZ;

        PropertyEditArea::callPropertyUpdate();
    }
}

void Float3PropertyArea::setup(){
    if(this->vector == nullptr)
        return;
    x_field->setText(QString::number(static_cast<double>(vector->X)));
    y_field->setText(QString::number(static_cast<double>(vector->Y)));
    z_field->setText(QString::number(static_cast<double>(vector->Z)));
}

Float3PropertyArea::~Float3PropertyArea(){
    delete this->x_field;
    delete this->y_field;
    delete this->z_field;

    delete this->x_label;
    delete this->y_label;
    delete this->z_label;

}
//String property area stuff
StringPropertyArea::StringPropertyArea(){
    type = PEA_TYPE_STRING;
    //this->str_layout = new QHBoxLayout; //Allocating layout
    this->value_ptr = nullptr;
    this->edit_field = new QLineEdit; //Allocation of QLineEdit

    elem_layout->addWidget(edit_field);
}

void StringPropertyArea::setup(){
    this->edit_field->setText(*this->value_ptr);
}

StringPropertyArea::~StringPropertyArea(){
    delete edit_field; //Remove text field
}

void StringPropertyArea::addToInspector(InspectorWin* win){
    win->getContentLayout()->addLayout(this->elem_layout);
}

void StringPropertyArea::updateState(){
    if(this->value_ptr == nullptr) //If vector hasn't been set
        return; //Go out
    //Get current values in textt fields
    QString current = this->edit_field->text();
    //Compare them
    if(current.compare(value_ptr) != 0){ //If it updated
        *value_ptr = current;

        PropertyEditArea::callPropertyUpdate();
    }
}

//Float property area stuff
FloatPropertyArea::FloatPropertyArea(){
    type = PEA_TYPE_FLOAT;
    this->float_field = new QLineEdit; //Allocation of text field

    elem_layout->addWidget(float_field); //Add text filed to layout
}
FloatPropertyArea::~FloatPropertyArea(){
    delete float_field;
}

void FloatPropertyArea::setup(){
    if(this->value == nullptr) //If value pointer didn't set
        return;
    float_field->setText(QString::number(static_cast<double>(*value)));
}
void FloatPropertyArea::addToInspector(InspectorWin* win){
    win->getContentLayout()->addLayout(this->elem_layout);
}

void FloatPropertyArea::updateState(){
    if(this->value == nullptr) //If vector hasn't been set
        return; //Go out
    //Get current values in text fields
    float value = this->float_field->text().toFloat();

    //Get current values in out vector ptr
    float vptrX = *this->value;
    //Compare them
    if(value != vptrX){ //If it updated
        *this->value = value;

        PropertyEditArea::callPropertyUpdate();
    }
}

//Pick resoource area stuff
PickResourceArea::PickResourceArea(){
    type = PEA_TYPE_RESPICK;
    this->rel_path = nullptr;
    respick_btn = new QPushButton; //Allocation of QPushButton
    relpath_label = new QLabel; //Allocation of resource relpath text
    elem_layout->addWidget(relpath_label);
    elem_layout->addWidget(respick_btn);
    respick_btn->setText("Select...");

    this->dialog = new ResourcePickDialog; //Allocation of dialog
    dialog->area = this;

}
PickResourceArea::~PickResourceArea(){
    delete respick_btn;
    delete dialog;
    delete relpath_label;
}

void PickResourceArea::addToInspector(InspectorWin* win){
    QObject::connect(respick_btn,  SIGNAL(clicked()), dialog, SLOT(onNeedToShow())); //On click on this button dialog will be shown
    win->getContentLayout()->addLayout(elem_layout);
}

void PickResourceArea::setup(){
    QString resource_relpath = *this->rel_path;
    relpath_label->setText(resource_relpath);
}

void PickResourceArea::updateState(){

}

IntPropertyArea::IntPropertyArea(){
    type = PEA_TYPE_INT;
    this->value = nullptr;

    this->int_field = new QLineEdit;
    elem_layout->addWidget(int_field); //Add text filed to layout
}

IntPropertyArea::~IntPropertyArea(){
    delete int_field;
}

void IntPropertyArea::addToInspector(InspectorWin* win){
    win->getContentLayout()->addLayout(this->elem_layout);
}
void IntPropertyArea::setup(){

}
void IntPropertyArea::updateState(){

}
