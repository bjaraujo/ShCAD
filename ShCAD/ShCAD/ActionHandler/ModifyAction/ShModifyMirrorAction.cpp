
#include "ShModifyMirrorAction.h"
#include "Manager\ShLanguageManager.h"
#include "Entity\Composite\ShSelectedEntities.h"
#include "Event\ShNotifyEvent.h"
#include "UnRedo\ShEntityTransaction.h"
#include "Base\ShGlobal.h"
#include "Base\ShMath.h"
#include "Entity\Private\ShMirror.h"

ShModifyMirrorAction::ShModifyMirrorAction(ShCADWidget *widget)
	:ShModifyAction(widget) {


}

ShModifyMirrorAction::~ShModifyMirrorAction() {

}

void ShModifyMirrorAction::mouseLeftPressEvent(ShActionData &data) {

	if (this->status == Status::SelectingEntities) {

		this->triggerSelectingEntities(data.mouseEvent);
	}
	else {

		this->trigger(data.point);
	}

}

void ShModifyMirrorAction::mouseRightPressEvent(ShActionData &data) {

	if (this->status == Status::SelectingEntities) {
		this->finishSelectingEntities();
	}

}

void ShModifyMirrorAction::mouseMoveEvent(ShActionData &data) {

	if (this->status == Status::PickingSecondPoint) {

		this->invalidate(data.point);
	}
}

ActionType ShModifyMirrorAction::getType() {

	return ActionType::ActionModifyRotate;
}

QString ShModifyMirrorAction::getHeadTitle() {

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

void ShModifyMirrorAction::trigger(const ShPoint3d &point) {

	if (this->status == Status::PickingBasePoint) {

		this->status = PickingSecondPoint;
		this->base = point;

		this->widget->getRubberBand().create(ShLineData(this->base, this->base));

		auto itr = this->widget->getSelectedEntities()->begin();

		for (itr; itr != this->widget->getSelectedEntities()->end(); ++itr) {

			this->widget->getPreview().add((*itr)->clone());
		}

		this->triggerSucceeded();

	}
	else if (this->status == Status::PickingSecondPoint) {

		double angle = math::getAbsAngle(this->base.x, this->base.y, point.x, point.y);

		ShMirror mirror(this->base, angle);

		QLinkedList<ShEntity*> list;
		auto itr = this->widget->getSelectedEntities()->begin();
		for (itr; itr != this->widget->getSelectedEntities()->end(); ++itr) {

			mirror.setOriginal((*itr));
			(*itr)->accept(&mirror);
			list.append((*itr));
		}

		ShMirrorEntityTransaction *transaction = new ShMirrorEntityTransaction(this->widget, list, this->base, angle);
		ShGlobal::pushNewTransaction(this->widget, transaction);

		this->actionFinished();

	}
}

void ShModifyMirrorAction::invalidate(ShPoint3d &point) {

	if (this->status == Status::PickingSecondPoint) {

		this->widget->getRubberBand().setEnd(point);

		double angle = math::getAbsAngle(this->base.x, this->base.y, point.x, point.y);

		ShMirror mirror(this->base, angle);

		auto itr = this->widget->getPreview().begin();
		auto originalItr = this->widget->getSelectedEntities()->begin();

		for (itr; itr != this->widget->getPreview().end(); ++itr) {

			mirror.setOriginal((*originalItr));
			(*itr)->accept(&mirror);
			++originalItr;
		}

		this->widget->update((DrawType)(DrawType::DrawCaptureImage | DrawType::DrawPreviewEntities));
	}
}

void ShModifyMirrorAction::finishSelectingEntities() {

	if (this->widget->getSelectedEntities()->getSize() != 0) {

		this->status = Status::PickingBasePoint;

		ShUpdateTextToCommandListEvent event("");
		this->widget->notify(&event);

		this->updateCommandEditHeadTitle();

		this->widget->setCursor(this->getCursorShape());

	}
	else {

		this->actionCanceled();
	}

}