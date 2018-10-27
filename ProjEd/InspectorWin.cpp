#include "headers/InspectorWin.h"
#include "ui_inspector_win.h"

#include "../World/headers/World.h"
#include <QDoubleValidator>

InspectorWin::InspectorWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::InspectorWin)
{
    ui->setupUi(this);
    x_win_start = 411 + 480;
}

InspectorWin::~InspectorWin()
{
    delete ui;
}

QVBoxLayout* InspectorWin::getContentLayout(){
    return ui->propertySpace;
}

void InspectorWin::clearContentLayout(){
    unsigned int areas_num = static_cast<unsigned int>(this->property_areas.size());
    for(unsigned int area_i = 0; area_i < areas_num; area_i ++){
       this->property_areas[area_i]->clear(this);
       delete property_areas[area_i]; //remove all
    }
    this->property_areas.resize(0); //No areas in list
}

void InspectorWin::addPropertyArea(PropertyEditArea* area){
    area->setup();
    area->addToInspector(this);
    this->property_areas.push_back(area);
}

PropertyEditArea::PropertyEditArea(){
    type = PEA_TYPE_NONE;
    label_widget = new QLabel; //Allocating label
    go_property = nullptr; //Nullptr by default
}

PropertyEditArea::~PropertyEditArea(){
    delete this->label_widget;
}
//Defaults
void PropertyEditArea::setup(){
    switch(this->type){
        case PEA_TYPE_FLOAT3:{
            Float3PropertyArea* ptr = static_cast<Float3PropertyArea*>(this);
            ptr->setup();
            break;
        }
        case PEA_TYPE_STRING:{
            StringPropertyArea* ptr = static_cast<StringPropertyArea*>(this);
            ptr->setup();
            break;
        }
    }
}

void PropertyEditArea::clear(InspectorWin* win){

}
void PropertyEditArea::addToInspector(InspectorWin* win){

}
void PropertyEditArea::updateState(){

    switch(this->type){
        case PEA_TYPE_FLOAT3:{
            Float3PropertyArea* ptr = static_cast<Float3PropertyArea*>(this);
            ptr->updateState();
            break;
        }
        case PEA_TYPE_STRING:{
            StringPropertyArea* ptr = static_cast<StringPropertyArea*>(this);
            ptr->updateState();
            break;
        }
    }
}

void PropertyEditArea::setLabel(QString label){
    this->label_widget->setText(label);
}
//Float3 definations
Float3PropertyArea::Float3PropertyArea(){
    pos_layout = new QHBoxLayout; //Allocation of layout
    vector = nullptr; //set it to null to avoid crash;
    type = PEA_TYPE_FLOAT3;
    //pos_layout->setSpacing(10);

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

    pos_layout->addWidget(label_widget); //Adding label to result layout
    label_widget->setFixedWidth(100);

    pos_layout->addWidget(x_label); //Adding X label
    x_field->setFixedWidth(60);
    x_field->setValidator( new QDoubleValidator(0, 100, 6, nullptr) );
    pos_layout->addWidget(x_field); //Adding X text field

    pos_layout->addWidget(y_label);
    y_field->setFixedWidth(60);
    y_field->setValidator( new QDoubleValidator(0, 100, 6, nullptr) );
    pos_layout->addWidget(y_field);

    pos_layout->addWidget(z_label);
    z_field->setFixedWidth(60);
    z_field->setValidator( new QDoubleValidator(0, 100, 6, nullptr) );
    pos_layout->addWidget(z_field);


}

void Float3PropertyArea::addToInspector(InspectorWin* win){
    win->getContentLayout()->addLayout(this->pos_layout);
}

void Float3PropertyArea::clear(InspectorWin* win){
    pos_layout->removeWidget(label_widget); //Removes label

    pos_layout->removeWidget(x_label);
    pos_layout->removeWidget(y_label);
    pos_layout->removeWidget(z_label);

    pos_layout->removeWidget(x_field);
    pos_layout->removeWidget(y_field);
    pos_layout->removeWidget(z_field);


}

void Float3PropertyArea::updateState(){
    if(this->vector == nullptr) //If vector hasn't been set
        return; //Go out
    //Get current values in textt fields
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
    }
}

void Float3PropertyArea::setup(){
    if(this->vector == nullptr)
        return;
    x_field->setText(QString::number(vector->X));
    y_field->setText(QString::number(vector->Y));
    z_field->setText(QString::number(vector->Y));
}

Float3PropertyArea::~Float3PropertyArea(){
    delete this->x_field;
    delete this->y_field;
    delete this->z_field;

    delete this->x_label;
    delete this->y_label;
    delete this->z_label;

    delete this->pos_layout;
}

void InspectorWin::area_update(){
    unsigned int areas_num = static_cast<unsigned int>(this->property_areas.size());
    for(unsigned int area_i = 0; area_i < areas_num; area_i ++){
        PropertyEditArea* pea_ptr = this->property_areas[area_i]; //Obtain pointer to area
        pea_ptr->updateState(); //Update state on it.
    }
}

StringPropertyArea::StringPropertyArea(){
    type = PEA_TYPE_STRING;
    this->str_layout = new QHBoxLayout; //Allocating layout
    this->value_ptr = nullptr;
    this->edit_field = new QLineEdit; //Allocation of QLineEdit

    str_layout->addWidget(label_widget);
    str_layout->addWidget(edit_field);
}

void StringPropertyArea::setup(){
    this->edit_field->setText(*this->value_ptr);
}

StringPropertyArea::~StringPropertyArea(){
    delete this->str_layout;
}

void StringPropertyArea::clear(InspectorWin* win){
    this->str_layout->removeWidget(this->edit_field);
    this->str_layout->removeWidget(this->label_widget);
}

void StringPropertyArea::addToInspector(InspectorWin* win){
    win->getContentLayout()->addLayout(this->str_layout);
}

void StringPropertyArea::updateState(){
    if(this->value_ptr == nullptr) //If vector hasn't been set
        return; //Go out
    //Get current values in textt fields
    QString current = this->edit_field->text();
    //Compare them
    if(current.compare(value_ptr) != 0){ //If it updated
        *value_ptr = current;

        if(go_property != nullptr){ //If parent property has defined
            GameObjectProperty* property_ptr = static_cast<GameObjectProperty*>(this->go_property);
            property_ptr->onValueChanged(); //Then call changed
        }
    }


}
