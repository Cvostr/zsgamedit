#include "headers/GO_widget_item.h"

using namespace std;

vector<QTreeWidgetItem*> go_items;
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