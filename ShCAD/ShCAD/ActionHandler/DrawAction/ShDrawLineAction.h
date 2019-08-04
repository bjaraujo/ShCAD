

#ifndef _SHDRAWLINEACTION_H
#define _SHDRAWLINEACTION_H

#include "ShDrawAction.h"

class ShDrawLineAction : public ShDrawAction {
	
	friend class ShSubDrawLineAction;

public:
	enum Status {
		PickedNothing, // no point picked. About to pick start.
		PickedStart    // start point already picked. About to pick end.
	};

	enum SubAction {
		Default,
		Perpendicular,
	};

private:
	Status status;
	SubAction subAction;
	ShSubDrawLineAction *subDrawLineAction;

public:
	ShDrawLineAction(ShCADWidget *widget);
	~ShDrawLineAction();

	virtual void mouseLeftPressEvent(ShActionData &data);
	virtual void mouseMoveEvent(ShActionData &data);

	virtual ActionType getType();
	virtual QString getHeadTitle();

	//void takeNextStep(const ShPoint3d &point, const ShPoint3d &nextPoint);

	virtual ShAvailableDraft getAvailableDraft();

	virtual void invalidate(ShPoint3d point);

	void temp();

	inline Status getStatus() const { return this->status; }
	inline SubAction getSubAction() const { return this->subAction; }

	void changeSubAction(SubAction subAction);
	ShSubDrawLineAction* getSubDrawLineAction() const { return this->subDrawLineAction; }

};

////////////////////////////////////////////////////////////////////


class ShSubDrawLineAction {

protected:
	ShDrawLineAction *drawLineAction;
	ShCADWidget *widget;

public:
	ShSubDrawLineAction(ShDrawLineAction *drawLineAction, ShCADWidget *widget);
	virtual ~ShSubDrawLineAction() = 0;

	virtual void mouseLeftPressEvent(ShActionData &data) = 0;
	virtual void mouseMoveEvent(ShActionData &data) = 0;

	virtual void takeNextStep(const ShPoint3d &point, const ShPoint3d &nextPoint) = 0;
	virtual void invalidate(ShPoint3d point) = 0;

protected:
	ShDrawLineAction::Status& getStatus();
	void addEntity(ShEntity *newEntity, const QString &type);
	void updateCommandEditHeadTitle();

};

//////////////////////////////////////////////////////////////////////

class ShSubDrawLineAction_Default : public ShSubDrawLineAction {

public:
	ShSubDrawLineAction_Default(ShDrawLineAction *drawLineAction, ShCADWidget *widget);
	~ShSubDrawLineAction_Default();

	virtual void mouseLeftPressEvent(ShActionData &data);
	virtual void mouseMoveEvent(ShActionData &data);

	virtual void takeNextStep(const ShPoint3d &point, const ShPoint3d &nextPoint);
	virtual void invalidate(ShPoint3d point);
};

////////////////////////////////////////////////////////////////////////

class ShSubDrawLineAction_Perpendicular : public ShSubDrawLineAction {

private:
	ShEntity *perpendicularBase;

public:
	ShSubDrawLineAction_Perpendicular(ShDrawLineAction *drawLineAction, ShCADWidget *widget);
	~ShSubDrawLineAction_Perpendicular();

	virtual void mouseLeftPressEvent(ShActionData &data);
	virtual void mouseMoveEvent(ShActionData &data);

	virtual void takeNextStep(const ShPoint3d &point, const ShPoint3d &nextPoint);
	virtual void invalidate(ShPoint3d point);


	inline ShEntity* getPerpendicularBase() const { return this->perpendicularBase; }
	void setPerpendicularBase(ShEntity *base) { this->perpendicularBase = base; }

};



#endif //_SHDRAWLINEACTION_H