
#include "ShEntityTable.h"
#include "Base\ShLayer.h"
#include "Base\ShLayerTable.h"
#include "Entity\Private\ShSearchEntityStrategy.h"
#include "Entity\Composite\Dim\ShDim.h"
#include "Base\ShDimensionStyle.h"

ShEntityTable::ShEntityTable(ShLayerTable *layerTable)
	:layerTable(layerTable) {

}

ShEntityTable::ShEntityTable(const ShEntityTable &other)
	:ShComposite(other) {

}

ShEntityTable::~ShEntityTable() {

}

ShEntityTable& ShEntityTable::operator=(const ShEntityTable &other) {

	ShComposite::operator=(other);
	
	return *this;
}

ShEntityTable* ShEntityTable::clone() {

	return new ShEntityTable(*this);
}

void ShEntityTable::accept(ShVisitor *visitor) {


}

bool ShEntityTable::add(ShEntity *entity) {

	if (entity == nullptr)
		return false;

	this->justAddedEntityList.clear();

	entity->getLayer()->add(entity);
	
	if (dynamic_cast<ShDim*>(entity)) {
	
		ShDim *dim = dynamic_cast<ShDim*>(entity);
		const_cast<ShDimensionStyle*>(dim->getDimensionStyle())->add(dim);
	}

	if (entity->getLayer()->isTurnOn() == true) {

		this->layerTable->turnOnList.append(entity);
		this->justAddedEntityList.append(entity);
	}

	return ShComposite::add(entity);

}

bool ShEntityTable::add(const QLinkedList<ShEntity*> &list) {
	
	if (list.isEmpty())
		return false;

	this->justAddedEntityList.clear();

	QLinkedList<ShEntity*>::iterator itr;
	for (itr = const_cast<QLinkedList<ShEntity*>&>(list).begin();
		itr != const_cast<QLinkedList<ShEntity*>&>(list).end();
		++itr) {
	
		(*itr)->getLayer()->add((*itr));

		if (dynamic_cast<ShDim*>((*itr))) {

			ShDim *dim = dynamic_cast<ShDim*>((*itr));
			const_cast<ShDimensionStyle*>(dim->getDimensionStyle())->add(dim);
		}

		if ((*itr)->getLayer()->isTurnOn() == true) {

			this->layerTable->turnOnList.append((*itr));
			this->justAddedEntityList.append((*itr));
		}
	}

	return ShComposite::add(list);
}

bool ShEntityTable::remove(ShEntity *entity) {

	entity->getLayer()->remove(entity);

	if (dynamic_cast<ShDim*>(entity)) {

		ShDim *dim = dynamic_cast<ShDim*>(entity);
		const_cast<ShDimensionStyle*>(dim->getDimensionStyle())->remove(dim);
	}

	if (entity->getLayer()->isTurnOn() == true)
		this->layerTable->turnOnList.removeOne(entity);

	return ShComposite::remove(entity);
}

bool ShEntityTable::deleteEntity(ShEntity *entity) {

	entity->getLayer()->remove(entity);

	if (dynamic_cast<ShDim*>(entity)) {

		ShDim *dim = dynamic_cast<ShDim*>(entity);
		const_cast<ShDimensionStyle*>(dim->getDimensionStyle())->remove(dim);
	}

	if (entity->getLayer()->isTurnOn() == true)
		this->layerTable->turnOnList.removeOne(entity);

	return ShComposite::deleteEntity(entity);
}

void ShEntityTable::search(ShSearchEntityStrategy &strategy) {

	strategy.setList(this->layerTable->turnOnList);
	strategy.search();
}

QLinkedList<ShEntity*>::iterator ShEntityTable::justTurnOnLayerBegin() {

	return this->layerTable->getJustTurnOnLayer()->begin();
}

QLinkedList<ShEntity*>::iterator ShEntityTable::justTurnOnLayerEnd() {

	return this->layerTable->getJustTurnOnLayer()->end();
}

QLinkedList<ShEntity*>::iterator ShEntityTable::turnOnLayerBegin() {

	return this->layerTable->turnOnListBegin();
}

QLinkedList<ShEntity*>::iterator ShEntityTable::turnOnLayerEnd() {

	return this->layerTable->turnOnListEnd();
}