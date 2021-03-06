

#ifndef _SHACTIONHANDLERPROXY_H
#define _SHACTIONHANDLERPROXY_H

#include "Base\ShVariable.h"
#include <qcursor.h>

class QMouseEvent;
class QKeyEvent;
class ShActionHandler;
class ShCADWidget;
class ShTemporaryAction;
class QPainter;
class ShDecoratorAction;

class ShActionHandlerProxy {

private:
	ShCADWidget *widget;
	ShActionHandler *currentAction;
	ShDecoratorAction *decoratorAction;

public:
	ShActionHandlerProxy(ShCADWidget *widget);
	~ShActionHandlerProxy();

	void mouseLeftPressEvent(QMouseEvent *event);
	void mouseMidPressEvent(QMouseEvent *event);
	void mouseRightPressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);

	void setCurrentAction(ShActionHandler *actionHandler);
	void setDecoratorAction(ShDecoratorAction *decoratorAction);

	void invalidate();

	QCursor getCursorShape();
	ActionType getType();
	ActionType getTypeIgonoringTemp();

	void draw(QPainter *painter);

	inline ShActionHandler* getCurrentAction() const { return this->currentAction; }
	inline ShDecoratorAction* getDecoratorAction() const { return this->decoratorAction; }

	//void changeDecoratorAction();
};



#endif //_SHACTIONHANDLERPROXY_H