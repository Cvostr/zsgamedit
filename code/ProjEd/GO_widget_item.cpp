#include "headers/GO_widget_item.h"

using namespace std;

vector<QTreeWidgetItem*> go_items;
QTreeWidget* rootWidget;
bool go_w_i_reserved = false;

void GO_W_I::reserve(int max) {
	if (go_w_i_reserved) return;
	go_items.reserve(max);

	for (unsigned int i = 0; i < max; i++) {
		go_items.push_back( new QTreeWidgetItem);
	}

	go_w_i_reserved = true;
}

QTreeWidgetItem* GO_W_I::getItem(int index) {
	return go_items.at(index);
}

QTreeWidgetItem* GO_W_I::getItem(Engine::GameObject* obj) {
	return go_items.at(obj->array_index);
}

void GO_W_I::recreate(int index) {
	delete go_items.at(index);
	go_items[index] = new QTreeWidgetItem;
}

void GO_W_I::recreateAll(int max) {
	//counter of deleted items
	unsigned int total_deleted = 0;
	//While all objects aren't deleted
	while (total_deleted < max) {
		//iterate over all items
		for (unsigned int i = 0; i < max; i++) {
			QTreeWidgetItem* item = go_items.at(i);
			//if item has no children
			if(item->childCount() == 0)
				//delete item
				delete item;
			//increase counter of deleted items
			total_deleted++;
		}
	}
	for (unsigned int i = 0; i < max; i++) {
		go_items[i] = new QTreeWidgetItem;
	}
}

void GO_W_I::updateObjsNames(Engine::World* world) {
	for (unsigned int i = 0; i < world->objects.size(); i++) {
		Engine::GameObject* obj = world->objects[i];
		//check, if object was deleted
		if (!obj->alive)
			continue;
		Engine::LabelProperty* label = obj->getLabelProperty();
		if (label == nullptr)
			continue;
		//update text in widget item
		go_items[obj->array_index]->setText(0, QString::fromStdString(label->label));
	}
}

void GO_W_I::makeParentings(Engine::World* world, QTreeWidget* obj_widget_ptr) {
	rootWidget = obj_widget_ptr;
	obj_widget_ptr->clear();
	//Now add all objects to inspector tree
	for (unsigned int obj_i = 0; obj_i < world->objects.size(); obj_i++) {
		Engine::GameObject* obj_ptr = world->objects[obj_i];
		if (obj_ptr->parent.isEmpty()) { //If object has no parent
			obj_widget_ptr->addTopLevelItem(GO_W_I::getItem(obj_ptr));
		}
		else { //It has a parent
			Engine::GameObject* parent_ptr = (Engine::GameObject*)obj_ptr->parent.ptr; //Get parent pointer
			GO_W_I::getItem(parent_ptr)->addChild(GO_W_I::getItem(obj_ptr)); //Connect Qt Tree Items
		}
	}
}

void GO_W_I::updateGameObjectItem(Engine::GameObject* obj) {
	Engine::LabelProperty* label = obj->getLabelProperty();
	if (label == nullptr)
		return;
	//update text in widget item
	go_items[obj->array_index]->setText(0, QString::fromStdString(label->label));

	Engine::GameObject* parent = obj->parent.updLinkPtr();
	if (parent != nullptr) {
		go_items[parent->array_index]->addChild(go_items[obj->array_index]);
	}
	else {
		rootWidget->addTopLevelItem(go_items[obj->array_index]);
	}
	unsigned int children_num = obj->children.size();

	for (unsigned int ch_i = 0; ch_i < children_num; ch_i++) {
		Engine::GameObject* ch = obj->children[ch_i].updLinkPtr();
		updateGameObjectItem(ch);
	}
}