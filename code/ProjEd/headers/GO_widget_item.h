#ifndef GO_W_I_H
#define GO_W_I_H

#include <QTreeWidget>
#include <world/World.hpp>
#include <vector>

namespace GO_W_I {

	void reserve(int max);
	QTreeWidgetItem* getItem(int index);
	QTreeWidgetItem* getItem(Engine::GameObject* obj);
	void recreate(int index);
	void recreateAll(int max);
	void updateObjsNames(Engine::World* world);
	void makeParentings(Engine::World* world, QTreeWidget* obj_widget_ptr);
	void updateGameObjectItem(Engine::GameObject* obj);
	void clearChildrenList(QTreeWidgetItem* item);
	void clearChildrenListOnAll(Engine::World* world);
}
#endif