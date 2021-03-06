

#include "ShDefaultAction.h"
#include <QMouseEvent>
#include "ActionHandler\Private\ShChangeActionStrategy.h"
#include "ActionHandler\TemporaryAction\ShDragSelectAction.h"
#include "KeyHandler\ShKeyHandler.h"
#include "KeyHandler\ShCustomKey.h"
#include "Entity\Private\ShSearchEntityStrategy.h"
#include "Entity\Composite\ShSelectedEntities.h"
#include "Entity\Private\Algorithm\ShVertexFinder.h"
#include <qpainter.h>
#include "Base\ShMath.h"
#include "Entity\Private\Algorithm\ShStretchVisitor.h"
#include "TemporaryAction\ShTemporaryStretchAction.h"
#include "TemporaryAction\ShTemporaryMoveAxisAction.h"


ShDefaultAction::ShDefaultAction(ShCADWidget *widget)
	:ShActionHandler(widget) {

	this->subDefaultAction = new ShSubDefaultAction_Default(this, this->widget);

	this->keyHandler = ShKeyHandler::ShBuilder(this->widget, this).
		allowInput().
		allowKey(KeyType::Control_Z).
		allowKey(KeyType::Control_Y).
		allowKey(KeyType::Control_A).
		allowCustom(new ShCustomKey<ShDefaultAction>(Qt::Key::Key_Escape, Qt::KeyboardModifier::NoModifier, this, &ShDefaultAction::escPressed)).
		build();
}

ShDefaultAction::~ShDefaultAction() {

	if (this->subDefaultAction != nullptr)
		delete this->subDefaultAction;
}

void ShDefaultAction::mouseLeftPressEvent(ShActionData &data) {

	this->subDefaultAction->mouseLeftPressEvent(data);
}

void ShDefaultAction::mouseMoveEvent(ShActionData &data) {

	this->subDefaultAction->mouseMoveEvent(data);
}


ActionType ShDefaultAction::getType() {

	return ActionType::ActionDefault;
}

QString ShDefaultAction::getHeadTitle() {

	return shGetLanValue_command("Command/Command");
}

void ShDefaultAction::changeSubAction(ShSubDefaultAction *subDefaultAction) {

	if (this->subDefaultAction != nullptr)
		delete this->subDefaultAction;

	this->subDefaultAction = subDefaultAction;
}

void ShDefaultAction::escPressed() {

	if (this->widget->getSelectedEntities()->getSize() > 0) {
		this->widget->getSelectedEntities()->unSelectAll();
		this->widget->update(DrawType::DrawAll);
		this->widget->captureImage();
	
	}

	this->changeSubAction(new ShSubDefaultAction_Default(this, this->widget));
	
	shCommandLogManager->appendListEditTextWith((shGetLanValue_command("Command/<Cancel>")));

}

//////////////////////////////////////////////////////////////////////////////////

ShSubDefaultAction::ShSubDefaultAction(ShDefaultAction *defaultAction, ShCADWidget *widget)
	:defaultAction(defaultAction), widget(widget) {

}

ShSubDefaultAction::~ShSubDefaultAction() {

}


////////////////////////////////////////////////////////////////////////////////////



ShSubDefaultAction_Default::ShSubDefaultAction_Default(ShDefaultAction *defaultAction, ShCADWidget *widget)
	:ShSubDefaultAction(defaultAction, widget) {

}

ShSubDefaultAction_Default::~ShSubDefaultAction_Default() {

}

void ShSubDefaultAction_Default::mouseLeftPressEvent(ShActionData &data) {

	ShEntity *entity = nullptr;
	ShPoint3d mouse = this->widget->getMousePoint();
	ShSearchEntityUniqueStrategy strategy(&entity, mouse.x, mouse.y, this->widget->getZoomRate());
	this->widget->getEntityTable().search(strategy);
	
	if (entity == nullptr) {
		if (data.mouseEvent->modifiers() == Qt::ShiftModifier) {

			ShChangeTemporaryStrategy strategy(new ShDragSelectAction(this->widget, data.point.x, data.point.y,
				ShDragSelectAction::Mode::UnSelectMode), this->defaultAction);
			this->widget->changeAction(strategy);
		}

		else {
			ShChangeTemporaryStrategy strategy(new ShDragSelectAction(this->widget, data.point.x, data.point.y), this->defaultAction);
			this->widget->changeAction(strategy);
		}
	}
	else {
	
		if (data.mouseEvent->modifiers() == Qt::ShiftModifier) {
			
			if (this->widget->getSelectedEntities()->remove(entity) == true) {
			
				this->widget->update(DrawType::DrawAll);
				this->widget->captureImage();
			}
		
		}
		else {
		
			if (this->widget->getSelectedEntities()->add(entity) == true) {
			
				this->widget->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawSelectedEntities));
				this->widget->captureImage();
			}
		}
	}
}

void ShSubDefaultAction_Default::mouseMoveEvent(ShActionData &data) {
	
	ShPoint3d mouse = this->widget->getMousePoint();

	if (this->widget->getSelectedEntities()->getSize() > 0) {

		VertexType vertexType = VertexType::VertexNothing;
		ShPoint3d vertexPoint;

		ShNearestVertexFinder visitor(mouse.x, mouse.y, this->widget->getZoomRate(), vertexType, vertexPoint);

		auto itr = this->widget->getSelectedEntities()->begin();

		while (itr != this->widget->getSelectedEntities()->end() &&
			(vertexType == VertexType::VertexNothing || vertexType == VertexType::VertexOther)) {

			(*itr)->accept(&visitor);
			++itr;
		}

		if (vertexType != VertexType::VertexNothing && vertexType != VertexType::VertexOther) {

			int dx, dy;
			this->widget->convertEntityToDevice(vertexPoint.x, vertexPoint.y, dx, dy);
			QCursor::setPos(this->widget->mapToGlobal(QPoint(dx, dy)));

			ShSubDefaultAction_MouseIsInShapeVertex *subAction =
				new ShSubDefaultAction_MouseIsInShapeVertex(this->defaultAction, this->widget, vertexPoint);

			this->defaultAction->changeSubAction(subAction);
		}
	}
	else {
	
		int originX, originY;
		ShPoint3d xAxisTopLeft, xAxisBottomRight, yAxisTopLeft, yAxisBottomRight;
		this->widget->convertEntityToDevice(0, 0, originX, originY);

		this->widget->convertDeviceToEntity(originX - 3, originY - 103, yAxisTopLeft.x, yAxisTopLeft.y);
		this->widget->convertDeviceToEntity(originX + 3, originY + 3, yAxisBottomRight.x, yAxisBottomRight.y);
		this->widget->convertDeviceToEntity(originX - 3, originY - 3, xAxisTopLeft.x, xAxisTopLeft.y);
		this->widget->convertDeviceToEntity(originX + 103, originY + 3, xAxisBottomRight.x, xAxisBottomRight.y);
		

		if (math::checkPointLiesInsideRect(mouse, xAxisTopLeft, xAxisBottomRight, 1) == true ||
			math::checkPointLiesInsideRect(mouse, yAxisTopLeft, yAxisBottomRight, 1) == true) {

			ShSubDefaultAction_MouseIsInAxisOrigin *subAction =
				new ShSubDefaultAction_MouseIsInAxisOrigin(this->defaultAction, this->widget);

			this->defaultAction->changeSubAction(subAction);
		}
	}
}



ShSubDefaultAction_MouseIsInShapeVertex::ShSubDefaultAction_MouseIsInShapeVertex(ShDefaultAction *defaultAction, ShCADWidget *widget,
	const ShPoint3d &vertexPoint)
	:ShSubDefaultAction(defaultAction, widget), vertexPoint(vertexPoint) {

	this->drawVertex();
}

ShSubDefaultAction_MouseIsInShapeVertex::~ShSubDefaultAction_MouseIsInShapeVertex() {

}

void ShSubDefaultAction_MouseIsInShapeVertex::mouseLeftPressEvent(ShActionData &data) {

	QLinkedList<ShEntity*> possibleStretchEntities;
	QLinkedList<ShStretchData*> stretchDatas;
	bool possible = false;
	ShStretchData *stretchData = nullptr;

	ShPossibleEntityToStretchFinder visitor(this->vertexPoint, possible, &stretchData);

	auto itr = this->widget->getSelectedEntities()->begin();
	
	for (itr; itr != this->widget->getSelectedEntities()->end(); ++itr) {
	
		possible = false;
		(*itr)->accept(&visitor);
		if (possible == true) {
			possibleStretchEntities.append(*itr);
			stretchDatas.append(stretchData);
		}
	}

	this->setLastBasePoint(this->vertexPoint);

	ShChangeTemporaryStrategy strategy(new ShTemporaryStretchAction(this->widget, this->vertexPoint,
		possibleStretchEntities, stretchDatas), this->defaultAction);
	
	this->widget->changeAction(strategy);
	this->defaultAction->changeSubAction(new ShSubDefaultAction_Default(this->defaultAction, this->widget));



}

void ShSubDefaultAction_MouseIsInShapeVertex::mouseMoveEvent(ShActionData &data) {

	ShPoint3d topLeft, bottomRight;
	int dx, dy;
	this->widget->convertEntityToDevice(this->vertexPoint.x, this->vertexPoint.y, dx, dy);
	this->widget->convertDeviceToEntity(dx - 3, dy - 3, topLeft.x, topLeft.y);
	this->widget->convertDeviceToEntity(dx + 3, dy + 3, bottomRight.x, bottomRight.y);

	if (math::checkPointLiesInsideRect(data.point, topLeft, bottomRight, 1) == false) {
		this->widget->update(DrawType::DrawCaptureImage);
		this->defaultAction->changeSubAction(new ShSubDefaultAction_Default(this->defaultAction, this->widget));
	}
}

void ShSubDefaultAction_MouseIsInShapeVertex::drawVertex() {

	int dx, dy;
	this->widget->convertEntityToDevice(this->vertexPoint.x, this->vertexPoint.y, dx, dy);

	QPainter painter(this->widget);
	painter.fillRect(dx - 3, dy - 3, 6, 6, QColor(255, 000, 000));
}


///////////////////////////////////////////////////////////////


ShSubDefaultAction_MouseIsInAxisOrigin::ShSubDefaultAction_MouseIsInAxisOrigin(ShDefaultAction *defaultAction, ShCADWidget *widget)
	:ShSubDefaultAction(defaultAction, widget) {

	this->drawAxisOrigin();
}

ShSubDefaultAction_MouseIsInAxisOrigin::~ShSubDefaultAction_MouseIsInAxisOrigin() {

}

void ShSubDefaultAction_MouseIsInAxisOrigin::mouseLeftPressEvent(ShActionData &data) {

	ShChangeTemporaryStrategy strategy(new ShTemporaryMoveAxisAction(this->widget), this->defaultAction);

	this->widget->changeAction(strategy);
	this->defaultAction->changeSubAction(new ShSubDefaultAction_Default(this->defaultAction, this->widget));
}

void ShSubDefaultAction_MouseIsInAxisOrigin::mouseMoveEvent(ShActionData &data) {

	ShPoint3d mouse = this->widget->getMousePoint();

	int originX, originY;
	ShPoint3d xAxisTopLeft, xAxisBottomRight, yAxisTopLeft, yAxisBottomRight;
	this->widget->convertEntityToDevice(0, 0, originX, originY);

	this->widget->convertDeviceToEntity(originX - 3, originY - 103, yAxisTopLeft.x, yAxisTopLeft.y);
	this->widget->convertDeviceToEntity(originX + 3, originY + 3, yAxisBottomRight.x, yAxisBottomRight.y);
	this->widget->convertDeviceToEntity(originX - 3, originY - 3, xAxisTopLeft.x, xAxisTopLeft.y);
	this->widget->convertDeviceToEntity(originX + 103, originY + 3, xAxisBottomRight.x, xAxisBottomRight.y);

	if (math::checkPointLiesInsideRect(data.point, xAxisTopLeft, xAxisBottomRight, 1) == false &&
		math::checkPointLiesInsideRect(data.point, yAxisTopLeft, yAxisBottomRight, 1) == false) {
		this->widget->update(DrawType::DrawCaptureImage);
		this->defaultAction->changeSubAction(new ShSubDefaultAction_Default(this->defaultAction, this->widget));
	}
}

void ShSubDefaultAction_MouseIsInAxisOrigin::drawAxisOrigin() {

	int dx, dy;
	this->widget->convertEntityToDevice(0, 0, dx, dy);

	QPainter painter(this->widget);

	this->widget->convertEntityToDevice(this->widget->getRubberBand().getEnd().x, this->widget->getRubberBand().getEnd().y, dx, dy);

	painter.setPen(QColor(255, 255, 000));
	painter.drawLine(dx, dy, dx + 100, dy);
	painter.drawLine(dx, dy, dx, dy - 100);

	painter.drawText(dx + 90, dy + 15, "X");
	painter.drawText(dx - 15, dy - 90, "Y");
	painter.drawText(dx - 15, dy + 15, "O");
	painter.setPen(QColor(0, 0, 0));

	painter.end();
}