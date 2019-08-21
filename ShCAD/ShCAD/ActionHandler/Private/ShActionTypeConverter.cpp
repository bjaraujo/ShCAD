
#include "ShActionTypeConverter.h"

ShActionTypeConverter::ShActionTypeConverter() {

}

ShActionTypeConverter::~ShActionTypeConverter() {


}

QString ShActionTypeConverter::convert(ActionType actionType) {

	QString text;

	switch (actionType)
	{
	case ActionType::ActionDefault:
		text = ""; break;
	case ActionType::ActionDrawLine:
		text = "_Line"; break;
	case ActionType::ActionModifyMove:
		text = "_Move"; break;

	default:
		text = ""; break;
	}

	return text;

}

ActionType ShActionTypeConverter::convert(const QString &text) {

	return ActionType::ActionUnknown;

}