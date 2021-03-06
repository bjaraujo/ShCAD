
#include "ShChangeActionStrategy.h"
#include <qdebug.h>
#include "ActionHandler\TemporaryAction\ShTemporaryAction.h"
#include "ActionHandler\ShActionHandlerProxy.h"
#include "ActionHandler\Private\ShActionHandlerFactory.h"
#include "Manager\ShLanguageManager.h"
#include "ActionHandler\Private\ShActionTypeConverter.h"
#include "ActionHandler\TemporaryAction\ShPanAction.h"
#include "Entity\Composite\ShSelectedEntities.h"
#include "ActionHandler\ModifyAction\ShModifyAction.h"
#include "Manager\ShCommandLogManager.h"
#include "ActionHandler\Private\ShDecorateActionStrategy.h"
#include "Event\ShNotifyEvent.h"

ShChangeActionStrategy::ShChangeActionStrategy()
	:widget(nullptr) {

}

ShChangeActionStrategy::~ShChangeActionStrategy() {

}

////////////////////////////////////////////////////////////


ShChangeActionAfterCancelingCurrentStrategy::ShChangeActionAfterCancelingCurrentStrategy(ActionType typeToChange)
	:typeToChange(typeToChange) {

	
}

ShChangeActionAfterCancelingCurrentStrategy::~ShChangeActionAfterCancelingCurrentStrategy() {

}

void ShChangeActionAfterCancelingCurrentStrategy::change() {

	if (this->widget == nullptr)
		Q_ASSERT("ShChangeActionAfterCancelingCurrentStrategy::change() >> widget is null ptr");


	ShChangeDefaultAfterCancelingCurrentStrategy strategy;
	strategy.widget = this->widget;
	strategy.change();

	ShChangeActionFromDefaultStrategy strategy2(this->typeToChange);
	strategy2.widget = this->widget;
	strategy2.change();

}

//////////////////////////////////////////////////////////////

ShChangeDefaultAfterCancelingCurrentStrategy::ShChangeDefaultAfterCancelingCurrentStrategy() {

}

ShChangeDefaultAfterCancelingCurrentStrategy::~ShChangeDefaultAfterCancelingCurrentStrategy() {

}

void ShChangeDefaultAfterCancelingCurrentStrategy::change() {

	if (this->widget == nullptr)
		Q_ASSERT("ShChangeDefaultAfterCancelingCurrentStrategy::change() >> widget is null ptr");

	
	ShDecorateDisposableSnapActionStrategy strategy(ObjectSnap::ObjectSnapNothing);
	this->widget->changeAction(strategy);
	
	DrawType drawType = DrawType::DrawCaptureImage;

	this->widget->getRubberBand().clear();
	this->widget->getPreview().clear();

	if (this->widget->getSelectedEntities()->getSize() > 0) {
		this->widget->getSelectedEntities()->unSelectAll();
		drawType = (DrawType)(drawType | DrawType::DrawAll);
	}

	if ((drawType & DrawType::DrawAll) == DrawType::DrawAll) {
		this->widget->update(DrawType::DrawAll);
		this->widget->captureImage();
	}
	else if ((drawType & DrawType::DrawCaptureImage) == DrawType::DrawCaptureImage)
		this->widget->update(DrawType::DrawCaptureImage);

	if (this->widget->getActionHandlerProxy()->getCurrentAction() == nullptr)
		Q_ASSERT("ShChangeDefaultAfterCancelingCurrentStrategy::change() >> currentAction is null ptr");
	
	if (this->widget->getActionHandlerProxy()->getType() != ActionType::ActionDefault) {

		shCommandLogManager->appendListEditTextWith(shGetLanValue_command("Command/<Cancel>"));

		ShActionHandler *newAction = ShActionHandlerFactory::create(ActionType::ActionDefault, this->widget);

		delete this->widget->getActionHandlerProxy()->getCurrentAction();

		this->widget->getActionHandlerProxy()->setCurrentAction(newAction);
		this->widget->setCursor(newAction->getCursorShape());
		
		shCommandLogManager->replaceHeadTitle(newAction->getHeadTitle());

		ShActionChangedEvent event(newAction->getType());
		this->widget->notify(&event);
	}
	
}


//////////////////////////////////////////////////////////////


ShChangeActionFromDefaultStrategy::ShChangeActionFromDefaultStrategy(ActionType typeToChange)
	:typeToChange(typeToChange) {

}

ShChangeActionFromDefaultStrategy::~ShChangeActionFromDefaultStrategy() {

}

void ShChangeActionFromDefaultStrategy::change() {

	if (this->widget == nullptr)
		Q_ASSERT("ShChangeActionFromDefaultStrategy::change() >> widget is null ptr");

	DrawType drawType = DrawType::DrawCaptureImage;

	this->widget->getRubberBand().clear();
	this->widget->getPreview().clear();

	if (this->widget->getSelectedEntities()->getSize() > 0) {
		this->widget->getSelectedEntities()->unSelectAll();
		drawType = (DrawType)(drawType | DrawType::DrawAll);
	}

	if ((drawType & DrawType::DrawAll) == DrawType::DrawAll) {
		this->widget->update(DrawType::DrawAll);
		this->widget->captureImage();
	}
	else if ((drawType & DrawType::DrawCaptureImage) == DrawType::DrawCaptureImage)
		this->widget->update(DrawType::DrawCaptureImage);


	ShActionHandler *newAction = ShActionHandlerFactory::create(this->typeToChange, this->widget);

	if (this->widget->getActionHandlerProxy()->getCurrentAction() == nullptr)
		Q_ASSERT("ShChangeActionFromDefaultStrategy::change() >> currentAction is null ptr");

	if (this->typeToChange != ActionType::ActionDefault) {

		QString text = ShActionTypeConverter::convert(this->typeToChange);
		shCommandLogManager->appendListEditTextAndNewLineWith(text);
	}

	delete this->widget->getActionHandlerProxy()->getCurrentAction();

	this->widget->getActionHandlerProxy()->setCurrentAction(newAction);
	this->widget->setCursor(newAction->getCursorShape());

	shCommandLogManager->replaceHeadTitle(newAction->getHeadTitle());

	ShActionChangedEvent event(newAction->getType());
	this->widget->notify(&event);
}


/////////////////////////////////////////////////////////////

ShChangeDefaultAfterFinishingCurrentStrategy::ShChangeDefaultAfterFinishingCurrentStrategy() {

}

ShChangeDefaultAfterFinishingCurrentStrategy::~ShChangeDefaultAfterFinishingCurrentStrategy() {

}

void ShChangeDefaultAfterFinishingCurrentStrategy::change() {

	if (this->widget == nullptr)
		Q_ASSERT("ShChangeDefaultAfterFinishingCurrentStrategy::change() >> widget is null ptr");

	DrawType drawType = DrawType::DrawCaptureImage;

	this->widget->getRubberBand().clear();
	this->widget->getPreview().clear();

	if (this->widget->getSelectedEntities()->getSize() > 0) {
		this->widget->getSelectedEntities()->unSelectAll();
		drawType = (DrawType)(drawType | DrawType::DrawAll);
	}

	if ((drawType & DrawType::DrawAll) == DrawType::DrawAll) {
		this->widget->update(DrawType::DrawAll);
		this->widget->captureImage();
	}
	else if ((drawType & DrawType::DrawCaptureImage) == DrawType::DrawCaptureImage)
		this->widget->update(DrawType::DrawCaptureImage);

	ShActionHandler *newAction = ShActionHandlerFactory::create(ActionType::ActionDefault, this->widget);

	delete this->widget->getActionHandlerProxy()->getCurrentAction();

	this->widget->getActionHandlerProxy()->setCurrentAction(newAction);
	this->widget->setCursor(newAction->getCursorShape());
	
	shCommandLogManager->appendListEditTextWith("");
	shCommandLogManager->replaceHeadTitle(newAction->getHeadTitle());

	ShActionChangedEvent event(newAction->getType());
	this->widget->notify(&event);
}

///////////////////////////////////////////////////////////////

ShChangeTemporaryStrategy::ShChangeTemporaryStrategy(ShTemporaryAction *temporaryAction, ShActionHandler *previousAction)
	:temporaryAction(temporaryAction), previousAction(previousAction) {

	
}

ShChangeTemporaryStrategy::~ShChangeTemporaryStrategy() {

}

void ShChangeTemporaryStrategy::change() {

	if (this->widget == nullptr)
		Q_ASSERT("ShChangeTemporaryStrategy::change() >> widget is null ptr");

	this->temporaryAction->setPreviousAction(this->previousAction);

	this->widget->getActionHandlerProxy()->setCurrentAction(this->temporaryAction);
	this->widget->setCursor(this->temporaryAction->getCursorShape());
	
	shCommandLogManager->replaceHeadTitle(this->temporaryAction->getHeadTitle());

	ShActionChangedEvent event(this->temporaryAction->getType());
	this->widget->notify(&event);

}

/////////////////////////////////////////////////////////////////

ShChangeTemporaryPanStrategy::ShChangeTemporaryPanStrategy(ShActionHandler *previousAction)
	:previousAction(previousAction) {

}

ShChangeTemporaryPanStrategy::~ShChangeTemporaryPanStrategy() {

}


void ShChangeTemporaryPanStrategy::change() {

	if (this->widget == nullptr)
		Q_ASSERT("ShChangeTemporaryPanStrategy::change() >> widget is null ptr");

	ShPanAction *panAction = new ShPanAction(this->widget);

	panAction->setPreviousAction(this->previousAction);

	this->widget->getActionHandlerProxy()->setCurrentAction(panAction);
	this->widget->setCursor(panAction->getCursorShape());

	ShActionChangedEvent event(panAction->getType());
	this->widget->notify(&event);
}



//////////////////////////////////////////////////////////



ShReturnToPreviousFromPanStrategy::ShReturnToPreviousFromPanStrategy(ShPanAction *panAction)
	:panAction(panAction) {

}

ShReturnToPreviousFromPanStrategy::~ShReturnToPreviousFromPanStrategy() {

}

void ShReturnToPreviousFromPanStrategy::change() {

	if (this->widget == nullptr)
		Q_ASSERT("ShReturnToPreviousFromPanStrategy::change() >> widget is null ptr");

	ShActionHandler *previous = this->panAction->getPreviousAction();

	this->panAction->setPreviousAction(nullptr);
	delete this->panAction;

	this->widget->getActionHandlerProxy()->setCurrentAction(previous);
	this->widget->setCursor(previous->getCursorShape());
	
	ShActionChangedEvent event(previous->getType());
	this->widget->notify(&event);
}


/////////////////////////////////////////////////////////////////

ShReturnToPreviousFromTemporaryStrategy::ShReturnToPreviousFromTemporaryStrategy(ShTemporaryAction *temporaryAction)
	:temporaryAction(temporaryAction) {

}

ShReturnToPreviousFromTemporaryStrategy::~ShReturnToPreviousFromTemporaryStrategy() {


}

void ShReturnToPreviousFromTemporaryStrategy::change() {

	if(this->widget==nullptr)
		Q_ASSERT("ShReturnToPreviousFromTemporaryStrategy::change() >> widget is null ptr");

	ShActionHandler *previous = this->temporaryAction->getPreviousAction();

	this->temporaryAction->setPreviousAction(nullptr);
	delete this->temporaryAction;


	//previous->temporaryActionFinished();

	this->widget->getActionHandlerProxy()->setCurrentAction(previous);
	this->widget->setCursor(previous->getCursorShape());

	shCommandLogManager->replaceHeadTitle(previous->getHeadTitle());

	ShActionChangedEvent event(previous->getType());
	this->widget->notify(&event);
}

/////////////////////////////////////////////////////////////////////////////

ShReturnToPreviousAfterCancelingTemporaryStrategy::ShReturnToPreviousAfterCancelingTemporaryStrategy(ShTemporaryAction *temporaryAction)
	:temporaryAction(temporaryAction) {

}

ShReturnToPreviousAfterCancelingTemporaryStrategy::~ShReturnToPreviousAfterCancelingTemporaryStrategy() {

}

void ShReturnToPreviousAfterCancelingTemporaryStrategy::change() {

	if (this->widget == nullptr)
		Q_ASSERT("ShReturnToPreviousAfterCancelingTemporaryStrategy::change() >> widget is null ptr");

	ShDecorateDisposableSnapActionStrategy strategy(ObjectSnap::ObjectSnapNothing);
	this->widget->changeAction(strategy);

	this->widget->update(DrawType::DrawCaptureImage);

	shCommandLogManager->appendListEditTextWith(shGetLanValue_command("Command/<Cancel>"));

	ShActionHandler *previous = this->temporaryAction->getPreviousAction();

	this->temporaryAction->setPreviousAction(nullptr);
	delete this->temporaryAction;

	this->widget->getActionHandlerProxy()->setCurrentAction(previous);
	this->widget->setCursor(previous->getCursorShape());
	
	shCommandLogManager->replaceHeadTitle(previous->getHeadTitle());

	ShActionChangedEvent event(previous->getType());
	this->widget->notify(&event);
}


//////////////////////////////////////////////////////////////////

ShChangeModifyAfterCancelingCurrentStrategy::ShChangeModifyAfterCancelingCurrentStrategy(ActionType typeToChange)
	:typeToChange(typeToChange) {

}

ShChangeModifyAfterCancelingCurrentStrategy::~ShChangeModifyAfterCancelingCurrentStrategy() {

}

void ShChangeModifyAfterCancelingCurrentStrategy::change() {

	if (this->widget == nullptr)
		Q_ASSERT("ShChangeModifyAfterCancelingCurrentStrategy::change() >> widget is null ptr");

	ShDecorateDisposableSnapActionStrategy strategy(ObjectSnap::ObjectSnapNothing);
	this->widget->changeAction(strategy);

	if (this->widget->getActionHandlerProxy()->getType() != ActionType::ActionDefault) {
	
		ShChangeActionAfterCancelingCurrentStrategy strategy(this->typeToChange);
		this->widget->changeAction(strategy);
	}
	else {

		if (this->widget->getSelectedEntities()->getSize() == 0) {
		
			ShChangeActionFromDefaultStrategy strategy(this->typeToChange);
			this->widget->changeAction(strategy);
		}
		else {

			ShActionHandler *newAction = ShActionHandlerFactory::create(this->typeToChange, this->widget);

			if (this->widget->getActionHandlerProxy()->getCurrentAction() == nullptr)
				Q_ASSERT("ShChangeModifyAfterCancelingCurrentStrategy::change() >> currentAction is null ptr");

			if (this->typeToChange != ActionType::ActionDefault) {

				QString text = ShActionTypeConverter::convert(this->typeToChange);
			
				shCommandLogManager->appendListEditTextAndNewLineWith(text);
			}

			delete this->widget->getActionHandlerProxy()->getCurrentAction();

			this->widget->getActionHandlerProxy()->setCurrentAction(newAction);

			if (!dynamic_cast<ShModifyAction*>(newAction))
				Q_ASSERT("ShChangeModifyAfterCancelingCurrentStrategy::change() >> newAction is not modifyAction");

			ShActionChangedEvent event(newAction->getType());
			this->widget->notify(&event);


			dynamic_cast<ShModifyAction*>(newAction)->finishSelectingEntities();

			this->widget->update(DrawType::DrawAll);
			this->widget->captureImage();
		}
	}

}