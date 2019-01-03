#include "headers/InspectorWin.h"
#include "headers/ProjectEdit.h"
#include "../World/headers/World.h"
#include <QDoubleValidator>
#include <QObject>

AreaButton::AreaButton(){
    this->button = new QPushButton;
    connect(this->button, SIGNAL(clicked()), this, SLOT(onButtonPressed()));
}

void AreaRadioGroup::onRadioClicked(){
    for(unsigned int rbutton_it = 0; rbutton_it < this->rad_buttons.size(); rbutton_it ++){
        if(this->rad_buttons[rbutton_it]->isChecked()){
            *this->value_ptr = rbutton_it + 1;
        }
    }
}

void AreaRadioGroup::addRadioButton(QRadioButton* btn){
    this->rad_buttons.push_back(btn); //add pointer to vector
    this->btn_layout->addWidget(btn); //add pointer to layout
    connect(btn, SIGNAL(clicked()), this, SLOT(onRadioClicked()));

    if(rad_buttons.size() == *value_ptr){
        btn->setChecked(true);
    }
}

AreaRadioGroup::AreaRadioGroup(){
    btn_layout = new QVBoxLayout; //allocate layout object
    this->value_ptr = nullptr;
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
    this->onPressFuncPtr();
    insp_ptr->updateObjectProperties();
}


PropertyEditArea::PropertyEditArea(){
    type = PEA_TYPE_NONE;
    elem_layout = new QHBoxLayout;
    label_widget = new QLabel; //Allocating label
    go_property = nullptr; //Nullptr by default

    elem_layout->addWidget(label_widget); //Adding label to result layout
}

void PropertyEditArea::destroyLayout(){
    delete this->label_widget;
    delete this->elem_layout;
}

void PropertyEditArea::destroyContent(){

}

PropertyEditArea::~PropertyEditArea(){
   //destroyLayout();
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

void PropertyEditArea::callPropertyUpdate(){
    if(go_property != nullptr){ //If parent property has defined
        GameObjectProperty* property_ptr = static_cast<GameObjectProperty*>(this->go_property);
        property_ptr->onValueChanged(); //Then call changed
    }
}

void PropertyEditArea::setLabel(QString label){
    this->label_widget->setText(label);
}

void Float3PropertyArea::destroyContent(){
    delete this->x_field;
    delete this->y_field;
    delete this->z_field;

    delete this->x_label;
    delete this->y_label;
    delete this->z_label;
}

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
        //Store old values
        GameObjectProperty* prop_ptr = static_cast<GameObjectProperty*>(this->go_property);
        getActionManager()->newPropertyAction(prop_ptr->go_link, prop_ptr->type);
        //Write new values
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
    //destroyContent();
}
//String property area stuff
StringPropertyArea::StringPropertyArea(){
    type = PEA_TYPE_STRING;
    this->value_ptr = nullptr;
    this->edit_field = new QLineEdit; //Allocation of QLineEdit

    elem_layout->addWidget(edit_field);
}

void StringPropertyArea::setup(){
    this->edit_field->setText(*this->value_ptr);
}

void StringPropertyArea::destroyContent(){
    delete edit_field; //Remove text field
}

StringPropertyArea::~StringPropertyArea(){
   //destroyContent();
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
        GameObjectProperty* prop_ptr = static_cast<GameObjectProperty*>(this->go_property);
        getActionManager()->newPropertyAction(prop_ptr->go_link, prop_ptr->type);

        *value_ptr = current;

        PropertyEditArea::callPropertyUpdate();
    }
}

//Float property area stuff
FloatPropertyArea::FloatPropertyArea(){
    type = PEA_TYPE_FLOAT;
    this->float_field = new QLineEdit; //Allocation of text field
    value = 0; //Set default value

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
    this->resource_type = RESOURCE_TYPE_TEXTURE; //Default type is texture

    respick_btn = new QPushButton; //Allocation of QPushButton
    relpath_label = new QLabel; //Allocation of resource relpath text
    elem_layout->addWidget(relpath_label);
    elem_layout->addWidget(respick_btn);
    respick_btn->setText("Select...");

    this->dialog = new ResourcePickDialog; //Allocation of dialog
    dialog->area = this;
    dialog->resource_text = this->relpath_label;

}
PickResourceArea::~PickResourceArea(){

}

void PickResourceArea::destroyContent(){
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
    int_field->setText(QString::number(*value));
}
void IntPropertyArea::updateState(){
    if(this->value == nullptr) //If vector hasn't been set
        return; //Go out
    //Get current values in text fields
    int value = this->int_field->text().toInt();

    //Get current values in out vector ptr
    int vptrX = *this->value;
    //Compare them
    if(value != vptrX){ //If it updated
        *this->value = value;

        PropertyEditArea::callPropertyUpdate();
    }
}

void ResourcePickDialog::onResourceSelected(){
    QListWidgetItem* selected = this->list->currentItem();
    QString mesh_path = selected->text();

    GameObjectProperty* prop_ptr = static_cast<GameObjectProperty*>(area->go_property);
    getActionManager()->newPropertyAction(prop_ptr->go_link, prop_ptr->type);

    *area->rel_path = mesh_path;
    area->PropertyEditArea::callPropertyUpdate();
    this->resource_text->setText(mesh_path);
    accept(); //Close dailog with positive answer
}

void ResourcePickDialog::onNeedToShow(){
    this->list->clear();
    //Receiving pointer to project
    Project* project_ptr = static_cast<Project*>(static_cast<GameObjectProperty*>(this->area->go_property)->world_ptr->proj_ptr);
    unsigned int resources_num = static_cast<unsigned int>(project_ptr->resources.size());
    if(area->resource_type == RESOURCE_TYPE_MESH)
        QListWidgetItem* item = new QListWidgetItem("@plane", this->list);

    //Iterate over all resources
    for(unsigned int res_i = 0; res_i < resources_num; res_i ++){
        Resource* resource_ptr = &project_ptr->resources[res_i];
        if(resource_ptr->type == area->resource_type){
            QListWidgetItem* item = new QListWidgetItem(resource_ptr->rel_path, this->list);
        }
    }
    this->exec();
}
ResourcePickDialog::ResourcePickDialog(QWidget* parent) : QDialog (parent){
    contentLayout = new QGridLayout(); // Alocation of layout
    list = new QListWidget;
    this->setWindowTitle("Select Resource");

    contentLayout->addWidget(list);
    connect(this->list, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(onResourceSelected())); //Connect to slot
    setLayout(contentLayout);
}
ResourcePickDialog::~ResourcePickDialog(){
    delete list;
}
