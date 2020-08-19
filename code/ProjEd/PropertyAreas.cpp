#include "headers/InspectorWin.h"
#include "headers/InspEditAreas.h"
#include "headers/ProjectEdit.h"
#include "../World/headers/World.h"
#include <QDoubleValidator>
#include <QDragEnterEvent>
#include <QObject>
#include <QDir>
#include <iostream>

#define INSP_DIMENSION_WIDGET_SIZE 11

extern EditWindow* _editor_win;
extern Project* project_ptr;
//Hack to support resources
extern ZSGAME_DATA* game_data;

AreaPropertyTitle::AreaPropertyTitle(){
    this->layout.addWidget(&this->line);
    this->layout.addWidget(&this->prop_title);
    prop_title.setFixedHeight(25);
    prop_title.setMargin(0);

    QFont font = prop_title.font();
    font.setPointSize(13);
    font.setItalic(true);
    prop_title.setFont(font);

    line.setFrameShape(QFrame::HLine);
    line.setFrameShadow(QFrame::Sunken);
}

AreaButton::AreaButton(){
    this->button = new QPushButton;
    onPressFuncPtr = nullptr;
    connect(this->button, SIGNAL(clicked()), this, SLOT(onButtonPressed()));
}

void AreaRadioGroup::onRadioClicked(){
    Engine::GameObjectProperty* property_ptr = static_cast<Engine::GameObjectProperty*>(this->go_property);
    //make EdAction
    getActionManager()->newPropertyAction(property_ptr->go_link, property_ptr->type);

    for(unsigned int rbutton_it = 0; rbutton_it < this->rad_buttons.size(); rbutton_it ++){
        if(this->rad_buttons[rbutton_it]->isChecked()){
            *this->value_ptr = rbutton_it + 1;
        }
    }

    property_ptr->onValueChanged();
    if(updateInspectorOnChange)
        _editor_win->getInspector()->updateRequired = true;
}

void AreaRadioGroup::addRadioButton(QRadioButton* btn){
    this->rad_buttons.push_back(btn); //add pointer to vector
    this->btn_layout->addWidget(btn); //add pointer to layout
    connect(btn, SIGNAL(clicked()), this, SLOT(onRadioClicked()));
    //if button ID equals value, then this button is checked
    if(rad_buttons.size() == *value_ptr){
        btn->setChecked(true);
    }
}

AreaRadioGroup::AreaRadioGroup(){
    btn_layout = new QVBoxLayout; //allocate layout object
    this->value_ptr = nullptr;
    this->go_property = nullptr;
    updateInspectorOnChange = false;
}

AreaRadioGroup::~AreaRadioGroup(){
    delete this->btn_layout; //release layout object

    for(unsigned int rbutton_it = 0; rbutton_it < this->rad_buttons.size(); rbutton_it ++){
        delete this->rad_buttons[rbutton_it];
    }

    this->rad_buttons.clear();
}

AreaButton::~AreaButton(){
    delete this->button;
}

void AreaButton::onButtonPressed(){
    if(onPressFuncPtr != nullptr)
        this->onPressFuncPtr();
    insp_ptr->updateObjectProperties();
}

PropertyEditArea::PropertyEditArea(){
    type = PEA_TYPE_NONE;
    elem_layout = new QHBoxLayout;
    label_widget = new QLabel; //Allocating label
    go_property = nullptr; //Nullptr by default

    elem_layout->addWidget(label_widget); //Adding label to result layout
    elem_layout->setContentsMargins(8,0,0,0); //move layout to right
    elem_layout->setSpacing(0);
}

AreaText::AreaText(){
    this->label = new QLabel;
    label->setContentsMargins(0,0,0,0);
    label->setMaximumHeight(100);
    label->setMargin(0);
}
AreaText::~AreaText(){
    delete this->label;
}

void PropertyEditArea::destroyLayout(){
    delete this->label_widget;
    delete this->elem_layout;
}

void PropertyEditArea::destroyContent(){

}

void PropertyEditArea::updateValues(){

}

PropertyEditArea::~PropertyEditArea(){

}
//Defaults
void PropertyEditArea::setup(){

}

void PropertyEditArea::addToInspector(InspectorWin* win){
    assert(win);
}
void PropertyEditArea::writeNewValues(){

}

void PropertyEditArea::callPropertyUpdate(){
    if(go_property != nullptr){ //If parent property has defined
        Engine::GameObjectProperty* property_ptr = static_cast<Engine::GameObjectProperty*>(this->go_property);
        property_ptr->onValueChanged(); //Then call changed
    }
}

void PropertyEditArea::setLabel(QString label){
    this->label_widget->setText(label);
    this->label = label;
}

void Float3PropertyArea::destroyContent(){

}

//Float3 definations
Float3PropertyArea::Float3PropertyArea(){
    vector = nullptr; //set it to null to avoid crash;
    type = PEA_TYPE_FLOAT3;

    //Allocating separator labels
    x_label.setText(QString("X"));
    x_label.setFixedWidth(INSP_DIMENSION_WIDGET_SIZE);
    y_label.setText(QString("Y"));
    y_label.setFixedWidth(INSP_DIMENSION_WIDGET_SIZE);
    z_label.setText(QString("Z"));
    z_label.setFixedWidth(INSP_DIMENSION_WIDGET_SIZE);
    //Set specific styles to labels
    x_label.setStyleSheet("QLabel { color : white; background-color: red }");
    y_label.setStyleSheet("QLabel { color : white; background-color: green }");
    z_label.setStyleSheet("QLabel { color : white; background-color: blue }");

    label_widget->setFixedWidth(100);

    QLocale locale(QLocale::English); //Define english locale to set it to double validator later
    QDoubleValidator* validator = new QDoubleValidator(-100, 100, 6, nullptr); //Define double validator
    validator->setLocale(locale); //English locale to accept dost instead of commas

    elem_layout->addWidget(&x_label); //Adding X label
    x_field.setMinimumWidth(60);
    x_field.setValidator(validator); //Set double validator
    elem_layout->addWidget(&x_field); //Adding X text field

    elem_layout->addWidget(&y_label);
    y_field.setMinimumWidth(60);
    y_field.setValidator(validator);
    elem_layout->addWidget(&y_field);

    elem_layout->addWidget(&z_label);
    z_field.setMinimumWidth(60);
    z_field.setValidator(validator);
    elem_layout->addWidget(&z_field);
}

void Float3PropertyArea::addToInspector(InspectorWin* win){
    win->connect(&this->y_field, SIGNAL(textEdited(QString)), win, SLOT(onPropertyChange()));
    win->connect(&this->x_field, SIGNAL(textEdited(QString)), win, SLOT(onPropertyChange()));
    win->connect(&this->z_field, SIGNAL(textEdited(QString)), win, SLOT(onPropertyChange()));

    win->getContentLayout()->addLayout(this->elem_layout);

}

void Float3PropertyArea::writeNewValues(){
    if(this->vector == nullptr) //If vector hasn't been set
        return; //Go out
    //Get current values in text fields
    float vX = this->x_field.text().toFloat();
    float vY = this->y_field.text().toFloat();
    float vZ = this->z_field.text().toFloat();

    if(!REAL_NUM_EQ(vector->X, vX) || !REAL_NUM_EQ(vector->Y, vY) || !REAL_NUM_EQ(vector->Z, vZ)){
        //Store old values
        Engine::GameObjectProperty* prop_ptr = static_cast<Engine::GameObjectProperty*>(this->go_property);
        getActionManager()->newPropertyAction(prop_ptr->go_link, prop_ptr->type);
    }
    //Write new values
    this->vector->X = vX;
    this->vector->Y = vY;
    this->vector->Z = vZ;

    PropertyEditArea::callPropertyUpdate();
}

void Float3PropertyArea::updateValues(){
    if(this->vector == nullptr) //If vector hasn't been set
        return; //Go out
    //Get current values in textt fields
    float vX = this->x_field.text().toFloat();
    float vY = this->y_field.text().toFloat();
    float vZ = this->z_field.text().toFloat();
    //if variables content changed
    if(!REAL_NUM_EQ(vector->X, vX) || !REAL_NUM_EQ(vector->Y, vY) || !REAL_NUM_EQ(vector->Z, vZ)){
        this->x_field.setText(QString::number(static_cast<double>(vector->X)));
        this->y_field.setText(QString::number(static_cast<double>(vector->Y)));
        this->z_field.setText(QString::number(static_cast<double>(vector->Z)));
    }
}

void Float3PropertyArea::setup(){
    if(this->vector == nullptr)
        return;
    x_field.setText(QString::number(static_cast<double>(vector->X)));
    y_field.setText(QString::number(static_cast<double>(vector->Y)));
    z_field.setText(QString::number(static_cast<double>(vector->Z)));
}

Float3PropertyArea::~Float3PropertyArea(){

}

//-------------
void Float2PropertyArea::destroyContent(){

}

//Float3 definations
Float2PropertyArea::Float2PropertyArea(){
    vector = nullptr; //set it to null to avoid crash;
    type = PEA_TYPE_FLOAT2;

    //Allocating separator labels
    x_label.setText(QString("X"));
    x_label.setFixedWidth(INSP_DIMENSION_WIDGET_SIZE);
    y_label.setText(QString("Y"));
    y_label.setFixedWidth(INSP_DIMENSION_WIDGET_SIZE);

    //Set specific styles to labels
    x_label.setStyleSheet("QLabel { color : white; background-color: red }");
    y_label.setStyleSheet("QLabel { color : white; background-color: green }");

    label_widget->setFixedWidth(100);

    QLocale locale(QLocale::English); //Define english locale to set it to double validator later
    QDoubleValidator* validator = new QDoubleValidator(-100, 100, 6, nullptr); //Define double validator
    validator->setLocale(locale); //English locale to accept dost instead of commas

    elem_layout->addWidget(&x_label); //Adding X label
    x_field.setMinimumWidth(60);
    x_field.setValidator(validator); //Set double validator
    elem_layout->addWidget(&x_field); //Adding X text field

    elem_layout->addWidget(&y_label);
    y_field.setMinimumWidth(60);
    y_field.setValidator(validator);
    elem_layout->addWidget(&y_field);

}

void Float2PropertyArea::addToInspector(InspectorWin* win){
    win->connect(&this->y_field, SIGNAL(textEdited(QString)), win, SLOT(onPropertyChange()));
    win->connect(&this->x_field, SIGNAL(textEdited(QString)), win, SLOT(onPropertyChange()));

    win->getContentLayout()->addLayout(this->elem_layout);

}

void Float2PropertyArea::writeNewValues(){
    if(this->vector == nullptr) //If vector hasn't been set
        return; //Go out
    //Get current values in text fields
    float vX = this->x_field.text().toFloat();
    float vY = this->y_field.text().toFloat();

    if(!REAL_NUM_EQ(vector->X, vX) || !REAL_NUM_EQ(vector->Y, vY)){
        //Store old values
        Engine::GameObjectProperty* prop_ptr = static_cast<Engine::GameObjectProperty*>(this->go_property);
        getActionManager()->newPropertyAction(prop_ptr->go_link, prop_ptr->type);
    }
    //Write new values
    this->vector->X = vX;
    this->vector->Y = vY;

    PropertyEditArea::callPropertyUpdate();
}

void Float2PropertyArea::updateValues(){
    if(this->vector == nullptr) //If vector hasn't been set
        return; //Go out
    //Get current values in textt fields
    float vX = this->x_field.text().toFloat();
    float vY = this->y_field.text().toFloat();
    //if variables content changed
    if(!REAL_NUM_EQ(vector->X, vX) || !REAL_NUM_EQ(vector->Y, vY)){
        this->x_field.setText(QString::number(static_cast<double>(vector->X)));
        this->y_field.setText(QString::number(static_cast<double>(vector->Y)));
    }
}

void Float2PropertyArea::setup(){
    if(this->vector == nullptr)
        return;
    x_field.setText(QString::number(static_cast<double>(vector->X)));
    y_field.setText(QString::number(static_cast<double>(vector->Y)));
}

Float2PropertyArea::~Float2PropertyArea(){

}


//-------------
void Int2PropertyArea::destroyContent(){

}

//Float3 definations
Int2PropertyArea::Int2PropertyArea(){
    vector = nullptr; //set it to null to avoid crash;
    type = PEA_TYPE_INT2;

    //Allocating separator labels
    x_label.setText(QString("X"));
    x_label.setFixedWidth(INSP_DIMENSION_WIDGET_SIZE);
    y_label.setText(QString("Y"));
    y_label.setFixedWidth(INSP_DIMENSION_WIDGET_SIZE);

    //Set specific styles to labels
    x_label.setStyleSheet("QLabel { color : white; background-color: red }");
    y_label.setStyleSheet("QLabel { color : white; background-color: green }");

    label_widget->setFixedWidth(100);

    QLocale locale(QLocale::English); //Define english locale to set it to double validator later
    QDoubleValidator* validator = new QDoubleValidator(-100, 100, 6, nullptr); //Define double validator
    validator->setLocale(locale); //English locale to accept dost instead of commas

    elem_layout->addWidget(&x_label); //Adding X label
    x_field.setMinimumWidth(60);
    x_field.setValidator(validator); //Set double validator
    elem_layout->addWidget(&x_field); //Adding X text field

    elem_layout->addWidget(&y_label);
    y_field.setMinimumWidth(60);
    y_field.setValidator(validator);
    elem_layout->addWidget(&y_field);

}

void Int2PropertyArea::addToInspector(InspectorWin* win){
    win->connect(&this->y_field, SIGNAL(textEdited(QString)), win, SLOT(onPropertyChange()));
    win->connect(&this->x_field, SIGNAL(textEdited(QString)), win, SLOT(onPropertyChange()));

    win->getContentLayout()->addLayout(this->elem_layout);

}

void Int2PropertyArea::writeNewValues(){
    if(this->vector == nullptr) //If vector hasn't been set
        return; //Go out
    //Get current values in text fields
    int vX = this->x_field.text().toInt();
    int vY = this->y_field.text().toInt();

    if(vector[0] != vX || vector[1] != vY){
        //Store old values
        Engine::GameObjectProperty* prop_ptr = static_cast<Engine::GameObjectProperty*>(this->go_property);
        getActionManager()->newPropertyAction(prop_ptr->go_link, prop_ptr->type);
    }
    //Write new values
    this->vector[0] = vX;
    this->vector[1] = vY;

    PropertyEditArea::callPropertyUpdate();
}

void Int2PropertyArea::updateValues(){
    if(this->vector == nullptr) //If vector hasn't been set
        return; //Go out
    //Get current values in textt fields
    int vX = this->x_field.text().toInt();
    int vY = this->y_field.text().toInt();
    //if variables content changed
    if(vector[0] != vX || vector[1] != vY){
        this->x_field.setText(QString::number(vector[0]));
        this->y_field.setText(QString::number(vector[1]));
    }
}

void Int2PropertyArea::setup(){
    if(this->vector == nullptr)
        return;
    this->x_field.setText(QString::number(vector[0]));
    this->y_field.setText(QString::number(vector[1]));
}

Int2PropertyArea::~Int2PropertyArea(){

}


//String property area stuff
StringPropertyArea::StringPropertyArea(){
    type = PEA_TYPE_STRING;
    this->value_ptr = nullptr;

    elem_layout->addWidget(&edit_field);
}

void StringPropertyArea::setup(){
    this->edit_field.setText(QString::fromStdString(*this->value_ptr));
}

void StringPropertyArea::destroyContent(){

}

StringPropertyArea::~StringPropertyArea(){

}

void StringPropertyArea::addToInspector(InspectorWin* win){
    win->connect(&this->edit_field, SIGNAL(textEdited(QString)), win, SLOT(onPropertyChange()));

    win->getContentLayout()->addLayout(this->elem_layout);
}

void StringPropertyArea::writeNewValues(){
    if(this->value_ptr == nullptr) //If vector hasn't been set
        return; //Go out
    //Get current values in textt fields
    std::string current = this->edit_field.text().toStdString();
    if(*value_ptr != current){ //if value changed, then make Action
        Engine::GameObjectProperty* prop_ptr = static_cast<Engine::GameObjectProperty*>(this->go_property);
        getActionManager()->newPropertyAction(prop_ptr->go_link, prop_ptr->type);
    }
    *value_ptr = current;

    PropertyEditArea::callPropertyUpdate();
}

void StringPropertyArea::updateValues(){
    if(this->value_ptr == nullptr) //If vector hasn't been set
        return; //Go out
    //Get current values in textt fields
    std::string str = this->edit_field.text().toStdString();

    if(value_ptr->compare(str)){
        this->edit_field.setText(QString::fromStdString(*value_ptr));
    }
}

//Float property area stuff
FloatPropertyArea::FloatPropertyArea(){
    type = PEA_TYPE_FLOAT;
    value = nullptr; //Set default value

    QLocale locale(QLocale::English); //Define english locale to set it to double validator later
    QDoubleValidator* validator = new QDoubleValidator(-100, 100, 6, nullptr); //Define double validator
    validator->setLocale(locale); //English locale to accept dost instead of commas

    float_field.setValidator(validator);
    elem_layout->addWidget(&float_field); //Add text filed to layout
}
FloatPropertyArea::~FloatPropertyArea(){

}

void FloatPropertyArea::setup(){
    if(this->value == nullptr) //If value pointer didn't set
        return;
    float_field.setText(QString::number(static_cast<double>(*value)));
}
void FloatPropertyArea::addToInspector(InspectorWin* win){
    win->connect(&this->float_field, SIGNAL(textEdited(QString)), win, SLOT(onPropertyChange()));

    win->getContentLayout()->addLayout(this->elem_layout);
}

void FloatPropertyArea::writeNewValues(){
    if(this->value == nullptr) //If vector hasn't been set
        return; //Go out
    //Get current values in text fields
    float value = this->float_field.text().toFloat();

    if(!REAL_NUM_EQ(*this->value, value)){ //if value changed, then make Action
        //Store old values
        Engine::GameObjectProperty* prop_ptr = static_cast<Engine::GameObjectProperty*>(this->go_property);
        getActionManager()->newPropertyAction(prop_ptr->go_link, prop_ptr->type);
    }
    *this->value = value;

    PropertyEditArea::callPropertyUpdate();
}

void FloatPropertyArea::updateValues(){
    if(this->value == nullptr) //If vector hasn't been set
        return; //Go out
    //Get current values in textt fields
    float vX = this->float_field.text().toFloat();

    if(!REAL_NUM_EQ(*value, vX)){
        this->float_field.setText(QString::number(static_cast<double>(*value)));
    }
}



IntPropertyArea::IntPropertyArea(){
    type = PEA_TYPE_INT;
    this->value = nullptr;

    QLocale locale(QLocale::English); //Define english locale to set it to double validator later
    QDoubleValidator* validator = new QDoubleValidator(-100, 100, 6, nullptr); //Define double validator
    validator->setLocale(locale); //English locale to accept dost instead of commas

    this->int_field = new QLineEdit;
    int_field->setValidator(validator);
    elem_layout->addWidget(int_field); //Add text filed to layout
}

IntPropertyArea::~IntPropertyArea(){
    delete int_field;
}

void IntPropertyArea::addToInspector(InspectorWin* win){
    win->connect(this->int_field, SIGNAL(textEdited(QString)), win, SLOT(onPropertyChange()));
    win->getContentLayout()->addLayout(this->elem_layout);
}
void IntPropertyArea::setup(){
    int_field->setText(QString::number(*value));
}
void IntPropertyArea::writeNewValues(){
    if(this->value == nullptr) //If vector hasn't been set
        return; //Go out
    //Get current values in text fields
    int value = this->int_field->text().toInt();
    if(*this->value != value){
        Engine::GameObjectProperty* prop_ptr = static_cast<Engine::GameObjectProperty*>(this->go_property);
        getActionManager()->newPropertyAction(prop_ptr->go_link, prop_ptr->type);
    }
    *this->value = value;

    PropertyEditArea::callPropertyUpdate();
}

void IntPropertyArea::updateValues(){
    if(this->value == nullptr) //If vector hasn't been set
        return; //Go out
    //Get current values in textt fields
    int vX = this->int_field->text().toInt();

    if(*value != vX){
        this->int_field->setText(QString::number(*value));
    }
}

ColorDialogArea::ColorDialogArea(){
    type = PEA_TYPE_COLOR;
    pick_button.setText("Pick color");
    elem_layout->addWidget(&digit_str);
    elem_layout->addWidget(&pick_button);
    dialog.area_ptr = this; //setting area pointer
    this->color = nullptr;
}

ColorDialogArea::~ColorDialogArea(){

}

void ColorDialogArea::addToInspector(InspectorWin* win){
    win->getContentLayout()->addLayout(this->elem_layout);
    QObject::connect(&this->pick_button, SIGNAL(clicked()), &dialog, SLOT(onNeedToShow()));
    dialog.color_ptr = this->color;
    updText();
}

void ColorDialogArea::updText(){
    int red = color->r;
    int green = color->g;
    int blue = color->b;

    QString _text = QString::number(red) + ", " + QString::number(green) + ", " + QString::number(blue);
    //set digits values
    digit_str.setText(_text);
}

ZSColorPickDialog::ZSColorPickDialog(QWidget* parent) : QColorDialog(parent){
    area_ptr = nullptr;
    color_ptr = nullptr;
}

void ZSColorPickDialog::onNeedToShow(){
    QColor color = this->getColor(QColor(color_ptr->r, color_ptr->g, color_ptr->b)); //invoke dialog

    if(!color.isValid()) return; //user canceled dialog
    //transform QColor to ZSRGBCOLOR
    ZSRGBCOLOR _color = ZSRGBCOLOR(color.red(), color.green(), color.blue());
    _color.updateGL();

    //Store old values
    Engine::GameObjectProperty* prop_ptr = static_cast<Engine::GameObjectProperty*>(this->area_ptr->go_property);
    if(prop_ptr != nullptr){
        getActionManager()->newPropertyAction(prop_ptr->go_link, prop_ptr->type);
    }
    //Store new value and call property update
    *color_ptr = _color;
    area_ptr->updText();
    area_ptr->PropertyEditArea::callPropertyUpdate();

}

BoolCheckboxArea::BoolCheckboxArea(){
    type = PEA_TYPE_BOOL;
    bool_ptr = nullptr;

    elem_layout->addWidget(&this->checkbox);
    updateInspectorOnChange = false;
}
void BoolCheckboxArea::addToInspector(InspectorWin* win){
    win->connect(&this->checkbox, SIGNAL(stateChanged(int)), win, SLOT(onPropertyChange()));

    win->getContentLayout()->addLayout(this->elem_layout);
}
void BoolCheckboxArea::writeNewValues(){
    if(bool_ptr == nullptr) return; //pointer not set, exiting

    if(go_property != nullptr && (*this->bool_ptr != checkbox.isChecked())){
        Engine::GameObjectProperty* prop_ptr = static_cast<Engine::GameObjectProperty*>(this->go_property);
        getActionManager()->newPropertyAction(prop_ptr->go_link, prop_ptr->type);
    }

    if((this->checkbox.isChecked() != *bool_ptr) && updateInspectorOnChange)
        _editor_win->getInspector()->updateRequired = true;

    *bool_ptr = this->checkbox.isChecked();

    PropertyEditArea::callPropertyUpdate();
}
void BoolCheckboxArea::setup(){
    checkbox.setChecked(*bool_ptr);
}
void BoolCheckboxArea::updateValues(){
    if(this->bool_ptr == nullptr) //If bool pointer hasn't been set
        return; //Go out
    //Get current value in text field
    bool cur = this->checkbox.isChecked();

    if(*bool_ptr != cur){
        checkbox.setChecked(*bool_ptr);
    }
}

ComboBoxArea::ComboBoxArea(){
    type = PEA_TYPE_COMBOBOX;

    elem_layout->addWidget(&this->widget);

    result_string_std = nullptr;
}
void ComboBoxArea::setup(){ //Virtual
    if(this->result_string_std != nullptr)
        widget.setCurrentText(QString::fromStdString(*this->result_string_std));
}
void ComboBoxArea::addToInspector(InspectorWin* win){
    win->getContentLayout()->addLayout(this->elem_layout);
    win->connect(&this->widget, SIGNAL(currentIndexChanged(int)), win, SLOT(onPropertyChange()));
}
void ComboBoxArea::writeNewValues(){ //Virtual, to check widget state
    if(result_string_std == nullptr) return; //pointer not set, exiting

    QString selected = widget.currentText();
    *this->result_string_std = selected.toStdString();

    PropertyEditArea::callPropertyUpdate();
}

GameobjectPickArea::GameobjectPickArea(){
    this->type = PEA_TYPE_GOBJECT_PICK;

    this->property_label = new QLabelResourcePickWgt(this); //Allocation of resource relpath text
    gameobject_ptr_ptr = nullptr;
    elem_layout->addWidget(property_label);
}
GameobjectPickArea::~GameobjectPickArea(){
    delete property_label;
}

void GameobjectPickArea::setup(){
    Engine::GameObject* obj_ptr = *this->gameobject_ptr_ptr;

    if(obj_ptr != nullptr){
        property_label->setText(QString::fromStdString(*obj_ptr->label_ptr));
    }else {
        property_label->setText("<none>");
    }
}
void GameobjectPickArea::addToInspector(InspectorWin* win){
    win->getContentLayout()->addLayout(this->elem_layout);
}


PropertyPickArea::PropertyPickArea(PROPERTY_TYPE type){
    this->prop_type = type;

    this->type = PEA_TYPE_PROPPICK;

    this->property_label = new QLabelResourcePickWgt(this); //Allocation of resource relpath text
    elem_layout->addWidget(property_label);

    //this->obj_label_ptr = nullptr;
    this->property_ptr_ptr = nullptr;

}
PropertyPickArea::~PropertyPickArea(){
    delete property_label;
}

void PropertyPickArea::setPropertyLink(ObjectPropertyLink* link){

}

void PropertyPickArea::setup(){
    if(this->property_ptr_ptr == nullptr) return;

    Engine::GameObjectProperty* property = *property_ptr_ptr;
    if(property != nullptr){
        Engine::GameObject* obj = property->go_link.updLinkPtr();
        link = obj->getLinkToThisObject();
        property_label->setText(QString::fromStdString(*this->link.updLinkPtr()->label_ptr) + "<" +getPropertyString(this->prop_type) + ">");
    }else {
        property_label->setText("<none>");
    }
}

void PropertyPickArea::addToInspector(InspectorWin* win){
    win->getContentLayout()->addLayout(this->elem_layout);
}
