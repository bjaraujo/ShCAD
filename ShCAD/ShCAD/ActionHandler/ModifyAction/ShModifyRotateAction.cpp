
#include "ShModifyRotateAction.h"
#include "Entity\Composite\ShSelectedEntities.h"
#include "UnRedo\ShEntityTransaction.h"
#include "Base\ShGlobal.h"
#include "Entity\Private\Algorithm\ShRotater.h"
#include "Base\ShMath.h"
#include "Command\ShAvailableCommands.h"
#include "KeyHandler\ShKeyHandler.h"
#include "Base\ShCursorShape.h"


ShModifyRotateAction::ShModifyRotateAction(ShCADWidget *widget)
	:ShModifyAction(widget), status(Status::SelectingEntities) {

	this->keyHandler = ShKeyHandler::ShBuilder(this->widget, this).
		allowKey(KeyType::Enter).
		allowKey(KeyType::Return).
		allowKey(KeyType::Control_A).
		allowKey(KeyType::EscCancelCurrent).
		allowInput().
		build();
}

ShModifyRotateAction::~ShModifyRotateAction() {

}

void ShModifyRotateAction::mouseLeftPressEvent(ShActionData &data) {

	if (this->status == Status::SelectingEntities) {

		this->triggerSelectingEntities(data.mouseEvent);
	}
	else {
	
		this->trigger(data.point);
	}
	
}

void ShModifyRotateAction::mouseRightPressEvent(ShActionData &data) {

	if (this->status == Status::SelectingEntities) {
		this->finishSelectingEntities();
	}

}

void ShModifyRotateAction::mouseMoveEvent(ShActionData &data) {

	if (this->status == Status::PickingSecondPoint) {

		this->invalidate(data.point);
	}
}

ActionType ShModifyRotateAction::getType() {

	return ActionType::ActionModifyRotate;
}

QString ShModifyRotateAction::getHeadTitle() {

	QString text;

	if (this->status == Status::SelectingEntities) {
		text = "Rotate >> " + shGetLanValue_command("Command/Select objects") + ": ";
	}
	else if (this->status == Status::PickingBasePoint) {
		text = "Rotate >> " + shGetLanValue_command("Command/Specify base point") + ": ";
	}
	else if (this->status == Status::PickingSecondPoint) {
		text = "Rotate >> " + shGetLanValue_command("Command/Specify rotation angle") + ": ";
	}

	return text;
}

QCursor ShModifyRotateAction::getCursorShape() {

	QCursor cursor;

	if (this->status == Status::SelectingEntities) {

		cursor = ShCursorShape::getCursor(ShCursorShape::CursorType::Selecting);
	}

	else if (this->status == Status::PickingBasePoint ||
		this->status == Status::PickingSecondPoint) {

		cursor = ShCursorShape::getCursor(ShCursorShape::CursorType::Drawing);
	}


	return cursor;
}

ShAvailableDraft ShModifyRotateAction::getAvailableDraft() {

	ShAvailableDraft draft;

	if (this->status == Status::PickingBasePoint) {

		draft.setAvailableOrthogonal(true);
		draft.setAvailableSnap(true);
		draft.setOrthogonalBasePoint(this->widget->getMousePoint());
		draft.setSnapBasePoint(this->widget->getMousePoint());
	}
	else if (this->status == Status::PickingSecondPoint) {

		draft.setAvailableOrthogonal(true);
		draft.setAvailableSnap(true);
		draft.setOrthogonalBasePoint(this->widget->getRubberBand().getStart());
		draft.setSnapBasePoint(this->widget->getRubberBand().getStart());
	}

	return draft;
}

void ShModifyRotateAction::trigger(const ShPoint3d &point) {

	if (this->status == Status::PickingBasePoint) {

		this->status = PickingSecondPoint;
		this->base = point;

		this->widget->getRubberBand().create(ShLineData(this->base, this->base));

		auto itr = this->widget->getSelectedEntities()->begin();

		for (itr; itr != this->widget->getSelectedEntities()->end(); ++itr) {

			this->widget->getPreview().add((*itr)->clone());
		}


		this->prevAngle = 0.0;

		this->availableCommands->remove(CommandType::DistanceFromBase);
		this->availableCommands->add(CommandType::AngleFromBase);

		this->setLastBasePoint(point);
		this->triggerSucceeded();

	}
	else if (this->status == Status::PickingSecondPoint) {

		double angle = math::getAbsAngle(this->base.x, this->base.y, point.x, point.y);

		ShRotater rotater(this->base, angle);

		QLinkedList<ShEntity*> list;
		auto itr = this->widget->getSelectedEntities()->begin();
		for (itr; itr != this->widget->getSelectedEntities()->end(); ++itr) {

			(*itr)->accept(&rotater);
			list.append((*itr));
		}

		ShRotateEntityTransaction *transaction = new ShRotateEntityTransaction(this->widget, list, this->base, angle);
		ShGlobal::pushNewTransaction(this->widget, transaction);

		this->setLastBasePoint(point);
		this->actionFinished();

	}
}

void ShModifyRotateAction::invalidate(ShPoint3d &point) {

	if (this->status == Status::PickingSecondPoint) {

		this->widget->getRubberBand().setEnd(point);

		double angle = math::getAbsAngle(this->base.x, this->base.y, point.x, point.y);

		ShRotater rotater(this->base, angle - this->prevAngle);

		auto itr = this->widget->getPreview().begin();

		for (itr; itr != this->widget->getPreview().end(); ++itr) {

			(*itr)->accept(&rotater);
		}

		this->prevAngle = angle;

		this->widget->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
	}
}

void ShModifyRotateAction::finishSelectingEntities() {

	if (this->widget->getSelectedEntities()->getSize() != 0) {

		this->status = Status::PickingBasePoint;

		shCommandLogManager->appendListEditTextWith("");
		this->updateCommandEditHeadTitle();

		this->widget->setCursor(this->getCursorShape());

		this->availableCommands = ShAvailableCommands::ShBuilder(this->widget, this).
			addAvailableCommand(CommandType::AbsoluteCoordinate).
			addAvailableCommand(CommandType::Empty_Cancel).
			addAvailableCommand(CommandType::RelativeCoordinate).
			addAvailableCommand(CommandType::PolarCoordinate).
			addAvailableCommand(CommandType::DistanceFromBase).
			build();

		this->keyHandler->disAllowKey(KeyType::Control_A);
	}
	else {

		this->actionCanceled();
	}

}