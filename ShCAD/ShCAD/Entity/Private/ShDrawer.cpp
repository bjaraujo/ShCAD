
#include "ShDrawer.h"
#include "Interface\ShCADWidget.h"
#include "Base\ShMath.h"
#include <qdebug.h>
#include "Entity\Leaf\ShLine.h"
#include "Entity\Leaf\ShConstructionLine.h"
#include "Entity\Leaf\ShCircle.h"
#include "Entity\Leaf\ShArc.h"
#include "Entity\Leaf\ShRubberBand.h"
#include "Entity\Leaf\ShPoint.h"
#include "Entity\Leaf\ShDot.h"
#include "Entity\Composite\Dim\ShDimLinear.h"
#include "Entity\Composite\Dim\ShDimAligned.h"
#include "Entity\Composite\Dim\ShDimRadius.h"
#include "Entity\Composite\Dim\ShDimDiameter.h"
#include "Entity\Composite\Dim\ShDimArcLength.h"
#include "Entity\Composite\Dim\ShDimAngular.h"
#include <qpainter.h>
#include "Data\ShScrollPosition.h"
#include "Interface\Private\ShAxis.h"
#include "Base\ShPointStyle.h"
#include "Base\ShDimensionStyle.h"

ShDrawerSelectedEntityFactory::ShDrawerSelectedEntityFactory() {

}

ShDrawerSelectedEntityFactory::~ShDrawerSelectedEntityFactory() {

}

ShDrawerSelectedEntity* ShDrawerSelectedEntityFactory::create(ShCADWidget *widget, QPainter *painter, ActionType actionType) {
	qDebug() << "ShDrawerSelectedEntityFactory::create" << actionType;

	if (actionType == ActionType::ActionModifyMove ||
		actionType == ActionType::ActionModifyCopy ||
		actionType == ActionType::ActionModifyRotate ||
		actionType == ActionType::ActionModifyMirror ||
		actionType == ActionType::ActionModifyExtend ||
		actionType == ActionType::ActionModifyStretch ||
		actionType == ActionType::ActionModifyTrim ||
		actionType == ActionType::ActionModifyErase ||
		actionType == ActionType::ActionModifyScale ||
		actionType == ActionType::ActionModifyOffset) {

		return new ShDrawerSelectedEntityNoVertex(widget, painter);
	}

	return new ShDrawerSelectedEntityVertex(widget, painter);
}



///////////////////////////////////////


GLPoint::GLPoint()
	:x(0), y(0) {

}

GLPoint::GLPoint(double x, double y)
	: x(x), y(y) {

}

GLPoint::~GLPoint() {

}

////////////////////////////////////////

GLColor::GLColor(GLfloat red, GLfloat green, GLfloat blue)
	:red(red), green(green), blue(blue) {

}

GLColor::~GLColor() {

}

////////////////////////////////////////

ShDrawerFunctions::ShDrawerFunctions(ShCADWidget *widget)
	:widget(widget) {

}

ShDrawerFunctions::~ShDrawerFunctions() {

}

void ShDrawerFunctions::convertDeviceToOpenGL(int x, int y, double &ox, double &oy) {

	int w = this->widget->width();
	int h = this->widget->height();
	ox = (double)((x - (double)w / 2.0)*(double)(1.0 / (double)(w / 2.0)));
	oy = -(double)((y - (double)h / 2.0)*(double)(1.0 / (double)(h / 2.0)));
}

void ShDrawerFunctions::convertEntityToOpenGL(double x, double y, double &ox, double &oy) {

	double zoomRate = this->widget->getZoomRate();
	double hPos = this->widget->getScrollPosition().horizontal;
	double vPos = this->widget->getScrollPosition().vertical;
	double centerX = this->widget->getAxis().getCenter().x;
	double centerY = this->widget->getAxis().getCenter().y;


	double tempX = (x*zoomRate) - hPos + (centerX*zoomRate);
	double tempY = -1 * ((y*zoomRate) + vPos - (centerY*zoomRate));


	int w = this->widget->width();
	int h = this->widget->height();
	ox = ((tempX - (double)w / 2.0)*(1.0 / ((double)w / 2.0)));
	oy = -((tempY - (double)h / 2.0)*(1.0 / ((double)h / 2.0)));
}

void ShDrawerFunctions::convertEntityToDevice(double x, double y, int &dx, int &dy) {

	double zoomRate = this->widget->getZoomRate();
	double hPos = this->widget->getScrollPosition().horizontal;
	double vPos = this->widget->getScrollPosition().vertical;
	ShPoint3d center = this->widget->getAxis().getCenter();

	double tempX = ((x*zoomRate) - hPos + (center.x*zoomRate));
	double tempY = (-1 * ((y*zoomRate) + vPos - (center.y*zoomRate)));

	dx = math::toInt(tempX);
	dy = math::toInt(tempY);
}

void ShDrawerFunctions::drawLine(const GLPoint& start, const GLPoint& end, const GLColor& color) {

	glColor3f(color.red, color.green, color.blue);
	glBegin(GL_LINES);
	glVertex2f(start.x, start.y);
	glVertex2f(end.x, end.y);
	glEnd();
}

void ShDrawerFunctions::drawFilledRect(const GLPoint& topLeft, const GLPoint& bottomRight, const GLColor& color) {

	glColor3f(color.red, color.green, color.blue);
	glBegin(GL_QUADS);
	glVertex2f(topLeft.x, topLeft.y);
	glVertex2f(bottomRight.x, topLeft.y);
	glVertex2f(bottomRight.x, bottomRight.y);
	glVertex2f(topLeft.x, bottomRight.y);
	glEnd();
}

void ShDrawerFunctions::drawFilledPolygon(GLPoint(*array), int length, const GLColor& color) {

	glColor3f(color.red, color.green, color.blue);
	glBegin(GL_POLYGON);

	for (int i = 0; i < length; i++) {
		glVertex2f(array[i].x, array[i].y);
	}

	glEnd();
}

void ShDrawerFunctions::drawCircle(const ShPoint3d& center, double radius, const GLColor& color, int segments) {

	glColor3f(color.red, color.green, color.blue);

	double theta, x, y;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < segments; i++) {

		theta = 2.0 * 3.1415926 * double(i) / double(segments);
		x = radius*cosf(theta);
		y = radius*sinf(theta);

		this->convertEntityToOpenGL(center.x + x, center.y + y, x, y);
		glVertex2f(x, y);
	}

	glEnd();
}

void ShDrawerFunctions::drawArc(const ShPoint3d& center, double radius, double startAngle, double endAngle, const GLColor& color, int segments) {

	glColor3f(color.red, color.green, color.blue);

	double theta, x, y;

	glBegin(GL_LINE_STRIP);

	theta = 2.0 * 3.1415926 * double(startAngle) / double(segments);
	x = radius*cosf(theta);
	y = radius*sinf(theta);

	this->convertEntityToOpenGL(center.x + x, center.y + y, x, y);
	glVertex2f(x, y);

	for (int i = math::toInt(startAngle + 1); i <= math::toInt(endAngle - 1); i++) {

		theta = 2.0 * 3.1415926 * double(i) / double(segments);
		x = radius*cosf(theta);
		y = radius*sinf(theta);
		this->convertEntityToOpenGL(center.x + x, center.y + y, x, y);
		glVertex2f(x, y);
	}

	theta = 2.0 * 3.1415926 * double(endAngle) / double(segments);
	x = radius*cosf(theta);
	y = radius*sinf(theta);
	this->convertEntityToOpenGL(center.x + x, center.y + y, x, y);
	glVertex2f(x, y);

	glEnd();
}

void ShDrawerFunctions::drawDot(const GLPoint &point, const GLColor &color) {

	glColor3f(color.red, color.green, color.blue);

	glBegin(GL_POINTS);
	glVertex2f(point.x, point.y);
	glEnd();
}

void ShDrawerFunctions::drawFilledTriangle(const GLPoint &p1, const GLPoint &p2, const GLPoint &p3, const GLColor &color) {

	glColor3f(color.red, color.green, color.blue);

	glBegin(GL_POLYGON);
	glVertex2f(p1.x, p1.y);
	glVertex2f(p2.x, p2.y);
	glVertex2f(p3.x, p3.y);
	glEnd();
}

void ShDrawerFunctions::drawTriangle(const GLPoint &p1, const GLPoint &p2, const GLPoint &p3, const GLColor &color) {

	glColor3f(color.red, color.green, color.blue);

	glBegin(GL_LINES);
	glVertex2f(p1.x, p1.y);
	glVertex2f(p2.x, p2.y);
	glVertex2f(p3.x, p3.y);
	glEnd();
}

/////////////////////////////////////////////////////////

ShDrawer::ShDrawer(ShCADWidget *widget, QPainter *painter)
	:widget(widget), painter(painter) {

}

ShDrawer::~ShDrawer() {

}

/////////////////////////////////////////////////////////

ShDrawerUnSelectedEntity::ShDrawerUnSelectedEntity(ShCADWidget *widget, QPainter *painter)
	:ShDrawer(widget, painter) {

}

ShDrawerUnSelectedEntity::~ShDrawerUnSelectedEntity() {

}

void ShDrawerUnSelectedEntity::visit(ShLine *line) {

	ShDrawerFunctions f(this->widget);

	ShLineData data = line->getData();
	ShPropertyData propertyData = line->getPropertyData();

	GLPoint start, end;
	f.convertEntityToOpenGL(data.start.x, data.start.y, start.x, start.y);
	f.convertEntityToOpenGL(data.end.x, data.end.y, end.x, end.y);

	GLColor color(propertyData.getColor().getRed() / 255., propertyData.getColor().getGreen() / 255.,
		propertyData.getColor().getBlue() / 255.);

	glLineStipple(1, propertyData.getLineStyle().getPattern());
	glEnable(GL_LINE_STIPPLE);
	f.drawLine(start, end, color);
	glDisable(GL_LINE_STIPPLE);
	
}

void ShDrawerUnSelectedEntity::visit(ShRubberBand *rubberBand) {

	if (rubberBand->isExist() == true) {
	
		ShDrawerFunctions f(this->widget);

		ShLineData data = rubberBand->getData();

		GLPoint start, end;
		f.convertEntityToOpenGL(data.start.x, data.start.y, start.x, start.y);
		f.convertEntityToOpenGL(data.end.x, data.end.y, end.x, end.y);

		GLColor color(1.0, 153 / 255.0, 51 / 255.0);

		glLineStipple(1, 0x07FF);
		glEnable(GL_LINE_STIPPLE);
		f.drawLine(start, end, color);
		glDisable(GL_LINE_STIPPLE);
		
	}

}

void ShDrawerUnSelectedEntity::visit(ShCircle *circle) {

	ShDrawerFunctions f(this->widget);

	ShCircleData data = circle->getData();
	ShPropertyData propertyData = circle->getPropertyData();

	GLColor color(propertyData.getColor().getRed() / 255., propertyData.getColor().getGreen() / 255.,
		propertyData.getColor().getBlue() / 255.);

	glLineStipple(1, propertyData.getLineStyle().getPattern());
	glEnable(GL_LINE_STIPPLE);
	f.drawCircle(data.center, data.radius, color);
	glDisable(GL_LINE_STIPPLE);
}

void ShDrawerUnSelectedEntity::visit(ShArc *arc) {

	ShDrawerFunctions f(this->widget);

	ShArcData data = arc->getData();
	ShPropertyData propertyData = arc->getPropertyData();

	GLColor color(propertyData.getColor().getRed() / 255., propertyData.getColor().getGreen() / 255.,
		propertyData.getColor().getBlue() / 255.);

	glLineStipple(1, propertyData.getLineStyle().getPattern());
	glEnable(GL_LINE_STIPPLE);

	if (math::compare(data.startAngle, data.endAngle) == 1) {
		f.drawArc(data.center, data.radius, data.startAngle, 360, color);
		f.drawArc(data.center, data.radius, 0, data.endAngle, color);
	}
	else {
	
		f.drawArc(data.center, data.radius, data.startAngle, data.endAngle, color);
	}

	glDisable(GL_LINE_STIPPLE);
}

void ShDrawerUnSelectedEntity::visit(ShPoint *point) {

	QList<ShEntity*> list;
	ShPointStyle::getComponentDependsOnStyle(list, point);

	ShDrawerUnSelectedEntity visitor(this->widget, this->painter);

	for (int i = 0; i < list.size(); i++) {
		list.at(i)->setPropertyData(point->getPropertyData());
		list.at(i)->accept(&visitor);
	}

	
	while (list.isEmpty() == false)
		delete list.takeFirst();
}

void ShDrawerUnSelectedEntity::visit(ShDot *dot) {

	ShDrawerFunctions f(this->widget);

	ShPropertyData propertyData = dot->getPropertyData();
	
	GLColor color(propertyData.getColor().getRed() / 255., propertyData.getColor().getGreen() / 255.,
		propertyData.getColor().getBlue() / 255.);

	GLPoint glPoint;

	f.convertEntityToOpenGL(dot->getPosition().x, dot->getPosition().y, glPoint.x, glPoint.y);
	f.drawDot(glPoint, color);

}

void ShDrawerUnSelectedEntity::visit(ShDimLinear *dimLinear) {

	ShDrawerUnSelectedEntity visitor(this->widget, this->painter);

	auto itr = dimLinear->begin();

	for (itr; itr != dimLinear->end(); ++itr)
		(*itr)->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimLinearData data = dimLinear->getData();
	ShPropertyData propertyData = dimLinear->getPropertyData();
	GLColor color(propertyData.getColor().getRed() / 255., propertyData.getColor().getGreen() / 255.,
		propertyData.getColor().getBlue() / 255.);

	dimLinear->getDimensionStyle()->getDimensionArrowStyle().drawLineArrow(f, data.firstDim, data.secondDim, color);

	int dx, dy;
	f.convertEntityToDevice(data.text.x, data.text.y, dx, dy);
	double angle = math::getAbsAngle(data.firstOrigin.x, data.firstOrigin.y, data.firstDim.x, data.firstDim.y);
	double distance = dimLinear->getDistance();
	QColor qColor(propertyData.getColor().getRed(), propertyData.getColor().getGreen(), propertyData.getColor().getBlue());

	if (this->painter->isActive() == false)
		painter->begin(this->widget);

	dimLinear->getDimensionStyle()->getDimensionTextStyle().drawDimensionDistanceText(this->painter,
		dx, dy, angle - 90, distance, qColor, this->widget->getZoomRate());
}

void ShDrawerUnSelectedEntity::visit(ShDimAligned *dimAligned) {

	ShDrawerUnSelectedEntity visitor(this->widget, this->painter);

	auto itr = dimAligned->begin();

	for (itr; itr != dimAligned->end(); ++itr)
		(*itr)->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimAlignedData data = dimAligned->getData();
	ShPropertyData propertyData = dimAligned->getPropertyData();
	GLColor color(propertyData.getColor().getRed() / 255., propertyData.getColor().getGreen() / 255.,
		propertyData.getColor().getBlue() / 255.);

	dimAligned->getDimensionStyle()->getDimensionArrowStyle().drawLineArrow(f, data.firstDim, data.secondDim, color);

	int dx, dy;
	f.convertEntityToDevice(data.text.x, data.text.y, dx, dy);
	double angle = math::getAbsAngle(data.firstOrigin.x, data.firstOrigin.y, data.firstDim.x, data.firstDim.y);
	double distance = dimAligned->getDistance();
	QColor qColor(propertyData.getColor().getRed(), propertyData.getColor().getGreen(), propertyData.getColor().getBlue());

	if (this->painter->isActive() == false)
		this->painter->begin(this->widget);

	dimAligned->getDimensionStyle()->getDimensionTextStyle().drawDimensionDistanceText(this->painter,
		dx, dy, angle - 90, distance, qColor, this->widget->getZoomRate());
}

void ShDrawerUnSelectedEntity::visit(ShDimRadius *dimRadius) {

	ShDrawerUnSelectedEntity visitor(this->widget, this->painter);
	
	auto itr = dimRadius->begin();

	for (itr; itr != dimRadius->end(); ++itr)
		(*itr)->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimRadiusData data = dimRadius->getData();
	ShPropertyData propertyData = dimRadius->getPropertyData();
	GLColor color(propertyData.getColor().getRed() / 255., propertyData.getColor().getGreen() / 255.,
		propertyData.getColor().getBlue() / 255.);

	double angle = math::getAbsAngle(data.dim.x, data.dim.y, data.text.x, data.text.y);
	dimRadius->getDimensionStyle()->getDimensionArrowStyle().drawArrow(f, data.dim, angle, color);

	int dx, dy;
	f.convertEntityToDevice(data.text.x, data.text.y, dx, dy);
	angle = math::getAbsAngle(data.center.x, data.center.y, data.text.x, data.text.y);
	QColor qColor(propertyData.getColor().getRed(), propertyData.getColor().getGreen(), propertyData.getColor().getBlue());

	if (this->painter->isActive() == false)
		this->painter->begin(this->widget);

	dimRadius->getDimensionStyle()->getDimensionTextStyle().drawDimensionRadiusText(this->painter,
		dx, dy, angle, dimRadius->getRadius(), qColor, this->widget->getZoomRate());
}

void ShDrawerUnSelectedEntity::visit(ShDimDiameter *dimDiameter) {

	ShDrawerUnSelectedEntity visitor(this->widget, this->painter);

	auto itr = dimDiameter->begin();

	for (itr; itr != dimDiameter->end(); ++itr)
		(*itr)->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimDiameterData data = dimDiameter->getData();
	ShPropertyData propertyData = dimDiameter->getPropertyData();
	GLColor color(propertyData.getColor().getRed() / 255., propertyData.getColor().getGreen() / 255.,
		propertyData.getColor().getBlue() / 255.);

	double angle = math::getAbsAngle(data.firstDim.x, data.firstDim.y, data.text.x, data.text.y);
	dimDiameter->getDimensionStyle()->getDimensionArrowStyle().drawArrow(f, data.firstDim, angle, color);

	if (dimDiameter->isCenterToTextDistanceShorterThanRadius() == false) {
		angle = math::getAbsAngle(data.firstDim.x, data.firstDim.y, data.secondDim.x, data.secondDim.y);
		dimDiameter->getDimensionStyle()->getDimensionArrowStyle().drawArrow(f, data.secondDim, angle, color);
	}

	int dx, dy;
	f.convertEntityToDevice(data.text.x, data.text.y, dx, dy);
	angle = math::getAbsAngle(data.center.x, data.center.y, data.text.x, data.text.y);
	QColor qColor(propertyData.getColor().getRed(), propertyData.getColor().getGreen(), propertyData.getColor().getBlue());

	if (this->painter->isActive() == false)
		this->painter->begin(this->widget);

	dimDiameter->getDimensionStyle()->getDimensionTextStyle().drawDimensionDiameterText(this->painter,
		dx, dy, angle, dimDiameter->getDiameter(), qColor, this->widget->getZoomRate());
}

void ShDrawerUnSelectedEntity::visit(ShDimArcLength *dimArcLength) {

	ShDrawerUnSelectedEntity visitor(this->widget, this->painter);

	auto itr = dimArcLength->begin();

	for (itr; itr != dimArcLength->end(); ++itr)
		(*itr)->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimArcLengthData data = dimArcLength->getData();
	ShPropertyData propertyData = dimArcLength->getPropertyData();
	GLColor color(propertyData.getColor().getRed() / 255., propertyData.getColor().getGreen() / 255.,
		propertyData.getColor().getBlue() / 255.);

	dimArcLength->getDimensionStyle()->getDimensionArrowStyle().drawArcArrow(f, data.center, dimArcLength->getArcRadius(),
		dimArcLength->getArcStart(), dimArcLength->getArcEnd(), color);

	int dx, dy;
	f.convertEntityToDevice(data.text.x, data.text.y, dx, dy);
	double angle = math::getAbsAngle(data.center.x, data.center.y, data.text.x, data.text.y);
	QColor qColor(propertyData.getColor().getRed(), propertyData.getColor().getGreen(), propertyData.getColor().getBlue());

	if (this->painter->isActive() == false)
		this->painter->begin(this->widget);

	dimArcLength->getDimensionStyle()->getDimensionTextStyle().drawDimensionArcLengthText(this->painter,
		dx, dy, angle - 90, dimArcLength->getArcLength(), qColor, this->widget->getZoomRate());
}

void ShDrawerUnSelectedEntity::visit(ShDimAngular *dimAngular) {

	ShDrawerUnSelectedEntity visitor(this->widget, this->painter);

	auto itr = dimAngular->begin();

	for (itr; itr != dimAngular->end(); ++itr)
		(*itr)->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimAngularData data = dimAngular->getData();
	ShPropertyData propertyData = dimAngular->getPropertyData();
	GLColor color(propertyData.getColor().getRed() / 255., propertyData.getColor().getGreen() / 255.,
		propertyData.getColor().getBlue() / 255.);

	dimAngular->getDimensionStyle()->getDimensionArrowStyle().drawArcArrow(f, data.center, dimAngular->getArcRadius(),
		dimAngular->getArcStart(), dimAngular->getArcEnd(), color);

	int dx, dy;
	f.convertEntityToDevice(data.text.x, data.text.y, dx, dy);
	double angle = math::getAbsAngle(data.center.x, data.center.y, data.text.x, data.text.y);
	QColor qColor(propertyData.getColor().getRed(), propertyData.getColor().getGreen(), propertyData.getColor().getBlue());

	if (this->painter->isActive() == false)
		this->painter->begin(this->widget);

	dimAngular->getDimensionStyle()->getDimensionTextStyle().drawDimensionAngleText(this->painter,
		dx, dy, angle - 90, dimAngular->getAngle(), qColor, this->widget->getZoomRate());
}

void ShDrawerUnSelectedEntity::visit(ShConstructionLine *constructionLine) {

	ShDrawerFunctions f(this->widget);

	ShLineData data = constructionLine->getData();
	ShPropertyData propertyData = constructionLine->getPropertyData();

	ShPoint3d topLeft, bottomRight;
	this->widget->convertDeviceToEntity(0, 0, topLeft.x, topLeft.y);
	this->widget->convertDeviceToEntity(this->widget->width(), this->widget->height(), bottomRight.x, bottomRight.y);

	double slope, interceptY;
	math::getEquationLine(data.start, data.end, slope, interceptY);

	double y = slope*topLeft.x + interceptY;
	double y2 = slope*bottomRight.x + interceptY;

	GLPoint start, end;

	if (slope != 0) {
		f.convertEntityToOpenGL(topLeft.x, y, start.x, start.y);
		f.convertEntityToOpenGL(bottomRight.x, y2, end.x, end.y);
	}
	else {
		f.convertEntityToOpenGL(data.start.x, topLeft.y, start.x, start.y);
		f.convertEntityToOpenGL(data.end.x, bottomRight.y, end.x, end.y);
	}

	GLColor color(propertyData.getColor().getRed() / 255., propertyData.getColor().getGreen() / 255.,
		propertyData.getColor().getBlue() / 255.);

	glLineStipple(1, propertyData.getLineStyle().getPattern());
	glEnable(GL_LINE_STIPPLE);
	f.drawLine(start, end, color);
	glDisable(GL_LINE_STIPPLE);

}

///////////////////////////////////////////////////////////////

ShDrawerSelectedEntity::ShDrawerSelectedEntity(ShCADWidget *widget, QPainter *painter)
	:ShDrawer(widget, painter) {

}

ShDrawerSelectedEntity::~ShDrawerSelectedEntity() {

}
///////////////////////////////////////////////////////////////

ShDrawerSelectedEntityVertex::ShDrawerSelectedEntityVertex(ShCADWidget *widget, QPainter *painter)
	:ShDrawerSelectedEntity(widget, painter) {

}

ShDrawerSelectedEntityVertex::~ShDrawerSelectedEntityVertex() {

}

void ShDrawerSelectedEntityVertex::visit(ShLine *line) {

	ShDrawerSelectedEntityNoVertex visitor(this->widget, this->painter);
	line->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShLineData data = line->getData();

	int startX, startY, midX, midY, endX, endY;
	f.convertEntityToDevice(data.start.x, data.start.y, startX, startY);
	f.convertEntityToDevice(data.end.x, data.end.y, endX, endY);
	f.convertEntityToDevice(line->getMid().x, line->getMid().y, midX, midY);

	GLPoint topLeft, bottomRight;
	f.convertDeviceToOpenGL(startX - 3, startY - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(startX + 3, startY + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertDeviceToOpenGL(midX - 3, midY - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(midX + 3, midY + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertDeviceToOpenGL(endX - 3, endY - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(endX + 3, endY + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));
}

void ShDrawerSelectedEntityVertex::visit(ShCircle *circle) {

	ShDrawerSelectedEntityNoVertex visitor(this->widget, this->painter);
	circle->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShCircleData data = circle->getData();
	
	int centerX, centerY, centerPlusRadiusX, centerPlusRadiusY;
	f.convertEntityToDevice(data.center.x, data.center.y, centerX, centerY);
	f.convertEntityToDevice(data.center.x + data.radius, data.center.y, centerPlusRadiusX, centerPlusRadiusY);

	int deviceRadius = (int)math::getDistance(centerX, centerY, centerPlusRadiusX, centerPlusRadiusY);

	GLPoint topLeft, bottomRight;
	f.convertDeviceToOpenGL(centerX - 3, centerY - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(centerX + 3, centerY + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertDeviceToOpenGL(centerX + deviceRadius - 3, centerY - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(centerX + deviceRadius + 3, centerY + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertDeviceToOpenGL(centerX - deviceRadius - 3, centerY - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(centerX - deviceRadius + 3, centerY + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertDeviceToOpenGL(centerX - 3, centerY + deviceRadius - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(centerX + 3, centerY + deviceRadius + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertDeviceToOpenGL(centerX - 3, centerY - deviceRadius - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(centerX + 3, centerY - deviceRadius + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));
}


void ShDrawerSelectedEntityVertex::visit(ShArc *arc) {

	ShDrawerSelectedEntityNoVertex visitor(this->widget, this->painter);
	arc->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShArcData data = arc->getData();
	
	int x, y;
	GLPoint topLeft, bottomRight;

	f.convertEntityToDevice(data.center.x, data.center.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertEntityToDevice(arc->getStart().x, arc->getStart().y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertEntityToDevice(arc->getEnd().x, arc->getEnd().y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertEntityToDevice(arc->getMid().x, arc->getMid().y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));
}

void ShDrawerSelectedEntityVertex::visit(ShPoint *point) {

	ShDrawerSelectedEntityNoVertex visitor(this->widget, this->painter);
	point->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	
	int x, y;
	GLPoint topLeft, bottomRight;
	
	f.convertEntityToDevice(point->getPosition().x, point->getPosition().y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));
}

void ShDrawerSelectedEntityVertex::visit(ShDot *dot) {

	ShDrawerFunctions f(this->widget);

	int x, y;
	GLPoint topLeft, bottomRight;

	f.convertEntityToDevice(dot->getPosition().x, dot->getPosition().y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));
}

void ShDrawerSelectedEntityVertex::visit(ShDimLinear *dimLinear) {

	ShDrawerSelectedEntityNoVertex visitor(this->widget, this->painter);
	dimLinear->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimLinearData data = dimLinear->getData();

	int x, y;
	GLPoint topLeft, bottomRight;
	f.convertEntityToDevice(data.firstOrigin.x, data.firstOrigin.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertEntityToDevice(data.secondOrigin.x, data.secondOrigin.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertEntityToDevice(data.firstDim.x, data.firstDim.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertEntityToDevice(data.secondDim.x, data.secondDim.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertEntityToDevice(data.text.x, data.text.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

}

void ShDrawerSelectedEntityVertex::visit(ShDimAligned *dimAligned) {

	ShDrawerSelectedEntityNoVertex visitor(this->widget, this->painter);
	dimAligned->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimAlignedData data = dimAligned->getData();

	int x, y;
	GLPoint topLeft, bottomRight;
	f.convertEntityToDevice(data.firstOrigin.x, data.firstOrigin.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertEntityToDevice(data.secondOrigin.x, data.secondOrigin.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertEntityToDevice(data.firstDim.x, data.firstDim.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertEntityToDevice(data.secondDim.x, data.secondDim.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertEntityToDevice(data.text.x, data.text.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));
}

void ShDrawerSelectedEntityVertex::visit(ShDimRadius *dimRadius) {

	ShDrawerSelectedEntityNoVertex visitor(this->widget, this->painter);
	dimRadius->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimRadiusData data = dimRadius->getData();

	int x, y;
	GLPoint topLeft, bottomRight;
	f.convertEntityToDevice(data.center.x, data.center.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertEntityToDevice(data.dim.x, data.dim.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertEntityToDevice(data.text.x, data.text.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));
}

void ShDrawerSelectedEntityVertex::visit(ShDimDiameter *dimDiameter) {

	ShDrawerSelectedEntityNoVertex visitor(this->widget, this->painter);
	dimDiameter->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimDiameterData data = dimDiameter->getData();

	int x, y;
	GLPoint topLeft, bottomRight;
	f.convertEntityToDevice(data.firstDim.x, data.firstDim.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertEntityToDevice(data.text.x, data.text.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertEntityToDevice(data.secondDim.x, data.secondDim.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

}

void ShDrawerSelectedEntityVertex::visit(ShDimArcLength *dimArcLength) {

	ShDrawerSelectedEntityNoVertex visitor(this->widget, this->painter);
	dimArcLength->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimArcLengthData data = dimArcLength->getData();

	int x, y;
	GLPoint topLeft, bottomRight;
	f.convertEntityToDevice(data.start.x, data.start.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertEntityToDevice(data.end.x, data.end.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertEntityToDevice(data.boundary.x, data.boundary.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertEntityToDevice(data.text.x, data.text.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));
}

void ShDrawerSelectedEntityVertex::visit(ShDimAngular *dimAngular) {

	ShDrawerSelectedEntityNoVertex visitor(this->widget, this->painter);
	dimAngular->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimAngularData data = dimAngular->getData();

	int x, y;
	GLPoint topLeft, bottomRight;

	f.convertEntityToDevice(data.center.x, data.center.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertEntityToDevice(data.start.x, data.start.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertEntityToDevice(data.end.x, data.end.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertEntityToDevice(data.boundary.x, data.boundary.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));

	f.convertEntityToDevice(data.text.x, data.text.y, x, y);
	f.convertDeviceToOpenGL(x - 3, y - 3, topLeft.x, topLeft.y);
	f.convertDeviceToOpenGL(x + 3, y + 3, bottomRight.x, bottomRight.y);
	f.drawFilledRect(topLeft, bottomRight, GLColor(0.0, 153.0 / 255, 1.0));
}

//////////////////////////////////////////////////////////////////////////////////////////

ShDrawerSelectedEntityNoVertex::ShDrawerSelectedEntityNoVertex(ShCADWidget *widget, QPainter *painter)
	:ShDrawerSelectedEntity(widget, painter) {

}

ShDrawerSelectedEntityNoVertex::~ShDrawerSelectedEntityNoVertex() {

}

void ShDrawerSelectedEntityNoVertex::visit(ShLine *line) {

	ShDrawerFunctions f(this->widget);

	ShLineData data = line->getData();

	GLPoint start, end;

	f.convertEntityToOpenGL(data.start.x, data.start.y, start.x, start.y);
	f.convertEntityToOpenGL(data.end.x, data.end.y, end.x, end.y);


	glLineStipple(1, 0xF1F1);
	glEnable(GL_LINE_STIPPLE);

	f.drawLine(start, end, GLColor(153.f / 255, 153.f / 155, 1.f));
	glDisable(GL_LINE_STIPPLE);

}

void ShDrawerSelectedEntityNoVertex::visit(ShCircle *circle) {

	ShDrawerFunctions f(this->widget);

	ShCircleData data = circle->getData();
	
	glLineStipple(1, 0xF1F1);
	glEnable(GL_LINE_STIPPLE);

	f.drawCircle(data.center, data.radius, GLColor(153.f / 255, 153.f / 155, 1.f));
	glDisable(GL_LINE_STIPPLE);
}

void ShDrawerSelectedEntityNoVertex::visit(ShArc *arc) {

	ShDrawerFunctions f(this->widget);

	ShArcData data = arc->getData();

	glLineStipple(1, 0xF1F1);
	glEnable(GL_LINE_STIPPLE);

	if (math::compare(data.startAngle, data.endAngle) == 1) {
		f.drawArc(data.center, data.radius, data.startAngle, 360, GLColor(153.f / 255, 153.f / 155, 1.f));
		f.drawArc(data.center, data.radius, 0, data.endAngle, GLColor(153.f / 255, 153.f / 155, 1.f));
	}
	else {

		f.drawArc(data.center, data.radius, data.startAngle, data.endAngle, GLColor(153.f / 255, 153.f / 155, 1.f));
	}

	glDisable(GL_LINE_STIPPLE);
}

void ShDrawerSelectedEntityNoVertex::visit(ShPoint *point) {
	
	QList<ShEntity*> list;
	ShPointStyle::getComponentDependsOnStyle(list, point);

	ShDrawerSelectedEntityNoVertex visitor(this->widget, this->painter);

	for (int i = 0; i < list.size(); i++) {
		list.at(i)->accept(&visitor);
	}


	while (list.isEmpty() == false)
		delete list.takeFirst();
}

void ShDrawerSelectedEntityNoVertex::visit(ShDot *dot) {

	ShDrawerFunctions f(this->widget);

	GLPoint point;
	f.convertEntityToOpenGL(dot->getPosition().x, dot->getPosition().y, point.x, point.y);

	glLineStipple(1, 0xF1F1);
	glEnable(GL_LINE_STIPPLE);
	f.drawDot(point, GLColor(153.f / 255, 153.f / 155, 1.f));
	glDisable(GL_LINE_STIPPLE);

}

void ShDrawerSelectedEntityNoVertex::visit(ShDimLinear *dimLinear) {

	ShDrawerSelectedEntityNoVertex visitor(this->widget, this->painter);

	auto itr = dimLinear->begin();
	for (itr; itr != dimLinear->end(); ++itr)
		(*itr)->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimLinearData data = dimLinear->getData();

	GLColor color(153.f / 255, 153.f / 155, 1.f);

	glLineStipple(1, 0xF1F1);
	glEnable(GL_LINE_STIPPLE);
	dimLinear->getDimensionStyle()->getDimensionArrowStyle().drawLineArrow(f, data.firstDim, data.secondDim, color);
	glDisable(GL_LINE_STIPPLE);

	int dx, dy;
	f.convertEntityToDevice(data.text.x, data.text.y, dx, dy);
	double angle = math::getAbsAngle(data.firstOrigin.x, data.firstOrigin.y, data.firstDim.x, data.firstDim.y);
	double distance = dimLinear->getDistance();
	QColor qColor(153, 153, 155);

	if (this->painter->isActive() == false)
		painter->begin(this->widget);

	dimLinear->getDimensionStyle()->getDimensionTextStyle().drawDimensionDistanceText(this->painter,
		dx, dy, angle - 90, distance, qColor, this->widget->getZoomRate());
}

void ShDrawerSelectedEntityNoVertex::visit(ShDimAligned *dimAligned) {

	ShDrawerSelectedEntityNoVertex visitor(this->widget, this->painter);

	auto itr = dimAligned->begin();
	for (itr; itr != dimAligned->end(); ++itr)
		(*itr)->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimAlignedData data = dimAligned->getData();

	GLColor color(153.f / 255, 153.f / 155, 1.f);

	glLineStipple(1, 0xF1F1);
	glEnable(GL_LINE_STIPPLE);
	dimAligned->getDimensionStyle()->getDimensionArrowStyle().drawLineArrow(f, data.firstDim, data.secondDim, color);
	glDisable(GL_LINE_STIPPLE);

	int dx, dy;
	f.convertEntityToDevice(data.text.x, data.text.y, dx, dy);
	double angle = math::getAbsAngle(data.firstOrigin.x, data.firstOrigin.y, data.firstDim.x, data.firstDim.y);
	double distance = dimAligned->getDistance();
	QColor qColor(153, 153, 155);

	if (this->painter->isActive() == false)
		painter->begin(this->widget);

	dimAligned->getDimensionStyle()->getDimensionTextStyle().drawDimensionDistanceText(this->painter,
		dx, dy, angle - 90, distance, qColor, this->widget->getZoomRate());
}

void ShDrawerSelectedEntityNoVertex::visit(ShDimRadius *dimRadius) {

	ShDrawerSelectedEntityNoVertex visitor(this->widget, this->painter);

	auto itr = dimRadius->begin();
	for (itr; itr != dimRadius->end(); ++itr)
		(*itr)->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimRadiusData data = dimRadius->getData();

	GLColor color(153.f / 255, 153.f / 155, 1.f);

	glLineStipple(1, 0xF1F1);
	glEnable(GL_LINE_STIPPLE);
	double angle = math::getAbsAngle(data.dim.x, data.dim.y, data.text.x, data.text.y);
	dimRadius->getDimensionStyle()->getDimensionArrowStyle().drawArrow(f, data.dim, angle, color);
	glDisable(GL_LINE_STIPPLE);

	int dx, dy;
	f.convertEntityToDevice(data.text.x, data.text.y, dx, dy);
	angle = math::getAbsAngle(data.center.x, data.center.y, data.text.x, data.text.y);
	QColor qColor(153, 153, 155);

	if (this->painter->isActive() == false)
		this->painter->begin(this->widget);

	dimRadius->getDimensionStyle()->getDimensionTextStyle().drawDimensionRadiusText(this->painter,
		dx, dy, angle, dimRadius->getRadius(), qColor, this->widget->getZoomRate());
}

void ShDrawerSelectedEntityNoVertex::visit(ShDimDiameter *dimDiameter) {

	ShDrawerSelectedEntityNoVertex visitor(this->widget, this->painter);

	auto itr = dimDiameter->begin();
	for (itr; itr != dimDiameter->end(); ++itr)
		(*itr)->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimDiameterData data = dimDiameter->getData();

	GLColor color(153.f / 255, 153.f / 155, 1.f);

	glLineStipple(1, 0xF1F1);
	glEnable(GL_LINE_STIPPLE);
	double angle = math::getAbsAngle(data.firstDim.x, data.firstDim.y, data.text.x, data.text.y);
	dimDiameter->getDimensionStyle()->getDimensionArrowStyle().drawArrow(f, data.firstDim, angle, color);

	if (dimDiameter->isCenterToTextDistanceShorterThanRadius() == false) {
		angle = math::getAbsAngle(data.firstDim.x, data.firstDim.y, data.secondDim.x, data.secondDim.y);
		dimDiameter->getDimensionStyle()->getDimensionArrowStyle().drawArrow(f, data.secondDim, angle, color);
	}
	glDisable(GL_LINE_STIPPLE);

	int dx, dy;
	f.convertEntityToDevice(data.text.x, data.text.y, dx, dy);
	angle = math::getAbsAngle(data.center.x, data.center.y, data.text.x, data.text.y);
	QColor qColor(153, 153, 155);

	if (this->painter->isActive() == false)
		this->painter->begin(this->widget);

	dimDiameter->getDimensionStyle()->getDimensionTextStyle().drawDimensionDiameterText(this->painter,
		dx, dy, angle, dimDiameter->getDiameter(), qColor, this->widget->getZoomRate());
}

void ShDrawerSelectedEntityNoVertex::visit(ShDimArcLength *dimArcLength) {

	ShDrawerSelectedEntityNoVertex visitor(this->widget, this->painter);

	auto itr = dimArcLength->begin();
	for (itr; itr != dimArcLength->end(); ++itr)
		(*itr)->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimArcLengthData data = dimArcLength->getData();

	GLColor color(153.f / 255, 153.f / 155, 1.f);

	glLineStipple(1, 0xF1F1);
	glEnable(GL_LINE_STIPPLE);
	dimArcLength->getDimensionStyle()->getDimensionArrowStyle().drawArcArrow(f, data.center, dimArcLength->getArcRadius(),
		dimArcLength->getArcStart(), dimArcLength->getArcEnd(), color);
	glDisable(GL_LINE_STIPPLE);

	int dx, dy;
	f.convertEntityToDevice(data.text.x, data.text.y, dx, dy);
	double angle = math::getAbsAngle(data.center.x, data.center.y, data.text.x, data.text.y);
	QColor qColor(153, 153, 155);

	if (this->painter->isActive() == false)
		this->painter->begin(this->widget);

	dimArcLength->getDimensionStyle()->getDimensionTextStyle().drawDimensionArcLengthText(this->painter,
		dx, dy, angle - 90, dimArcLength->getArcLength(), qColor, this->widget->getZoomRate());
}

void ShDrawerSelectedEntityNoVertex::visit(ShDimAngular *dimAngular) {

	ShDrawerSelectedEntityNoVertex visitor(this->widget, this->painter);

	auto itr = dimAngular->begin();
	for (itr; itr != dimAngular->end(); ++itr)
		(*itr)->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimAngularData data = dimAngular->getData();

	GLColor color(153.f / 255, 153.f / 155, 1.f);

	glLineStipple(1, 0xF1F1);
	glEnable(GL_LINE_STIPPLE);
	dimAngular->getDimensionStyle()->getDimensionArrowStyle().drawArcArrow(f, data.center, dimAngular->getArcRadius(),
		dimAngular->getArcStart(), dimAngular->getArcEnd(), color);
	glDisable(GL_LINE_STIPPLE);

	int dx, dy;
	f.convertEntityToDevice(data.text.x, data.text.y, dx, dy);
	double angle = math::getAbsAngle(data.center.x, data.center.y, data.text.x, data.text.y);
	QColor qColor(153, 153, 155);

	if (this->painter->isActive() == false)
		this->painter->begin(this->widget);

	dimAngular->getDimensionStyle()->getDimensionTextStyle().drawDimensionAngleText(this->painter,
		dx, dy, angle - 90, dimAngular->getAngle(), qColor, this->widget->getZoomRate());
}

//////////////////////////////////////////////////////////////////////

ShDrawerEraseBackGround::ShDrawerEraseBackGround(ShCADWidget *widget, QPainter *painter)
	:ShDrawer(widget, painter) {

}

ShDrawerEraseBackGround::~ShDrawerEraseBackGround() {

}


void ShDrawerEraseBackGround::visit(ShLine *line) {

	ShDrawerFunctions f(this->widget);

	ShLineData data = line->getData();

	GLPoint start, end;

	f.convertEntityToOpenGL(data.start.x, data.start.y, start.x, start.y);
	f.convertEntityToOpenGL(data.end.x, data.end.y, end.x, end.y);

	f.drawLine(start, end, GLColor(0, 0, 0));
}

void ShDrawerEraseBackGround::visit(ShCircle *circle) {

	ShDrawerFunctions f(this->widget);

	f.drawCircle(circle->getCenter(), circle->getRadius(), GLColor(0, 0, 0));
}

void ShDrawerEraseBackGround::visit(ShArc *arc) {

	ShDrawerFunctions f(this->widget);

	ShArcData data = arc->getData();

	if (math::compare(data.startAngle, data.endAngle) == 1) {
		f.drawArc(data.center, data.radius, data.startAngle, 360, GLColor(0, 0, 0));
		f.drawArc(data.center, data.radius, 0, data.endAngle, GLColor(0, 0, 0));
	}
	else {

		f.drawArc(data.center, data.radius, data.startAngle, data.endAngle, GLColor(0, 0, 0));
	}
}

void ShDrawerEraseBackGround::visit(ShPoint *point) {

	QList<ShEntity*> list;
	ShPointStyle::getComponentDependsOnStyle(list, point);

	ShDrawerEraseBackGround visitor(this->widget, this->painter);

	for (int i = 0; i < list.size(); i++) {
		list.at(i)->accept(&visitor);
	}


	while (list.isEmpty() == false)
		delete list.takeFirst();
}

void ShDrawerEraseBackGround::visit(ShDot *dot) {

	ShDrawerFunctions f(this->widget);

	GLPoint point;
	f.convertEntityToOpenGL(dot->getPosition().x, dot->getPosition().y, point.x, point.y);

	f.drawDot(point, GLColor(0, 0, 0));
}

void ShDrawerEraseBackGround::visit(ShDimLinear *dimLinear) {

	ShDrawerEraseBackGround visitor(this->widget, this->painter);

	auto itr = dimLinear->begin();
	for (itr; itr != dimLinear->end(); ++itr)
		(*itr)->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimLinearData data = dimLinear->getData();

	GLColor color(0, 0, 0);

	dimLinear->getDimensionStyle()->getDimensionArrowStyle().drawLineArrow(f, data.firstDim, data.secondDim, color);
	
	int dx, dy;
	f.convertEntityToDevice(data.text.x, data.text.y, dx, dy);
	double angle = math::getAbsAngle(data.firstOrigin.x, data.firstOrigin.y, data.firstDim.x, data.firstDim.y);
	double distance = dimLinear->getDistance();
	QColor qColor(0, 0, 0);

	if (this->painter->isActive() == false)
		painter->begin(this->widget);

	dimLinear->getDimensionStyle()->getDimensionTextStyle().drawDimensionDistanceText(this->painter,
		dx, dy, angle - 90, distance, qColor, this->widget->getZoomRate());
}

void ShDrawerEraseBackGround::visit(ShDimAligned *dimAligned) {

	ShDrawerEraseBackGround visitor(this->widget, this->painter);

	auto itr = dimAligned->begin();
	for (itr; itr != dimAligned->end(); ++itr)
		(*itr)->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimAlignedData data = dimAligned->getData();

	GLColor color(0, 0, 0);

	dimAligned->getDimensionStyle()->getDimensionArrowStyle().drawLineArrow(f, data.firstDim, data.secondDim, color);
	
	int dx, dy;
	f.convertEntityToDevice(data.text.x, data.text.y, dx, dy);
	double angle = math::getAbsAngle(data.firstOrigin.x, data.firstOrigin.y, data.firstDim.x, data.firstDim.y);
	double distance = dimAligned->getDistance();
	QColor qColor(0, 0, 0);

	if (this->painter->isActive() == false)
		painter->begin(this->widget);

	dimAligned->getDimensionStyle()->getDimensionTextStyle().drawDimensionDistanceText(this->painter,
		dx, dy, angle - 90, distance, qColor, this->widget->getZoomRate());
}

void ShDrawerEraseBackGround::visit(ShDimRadius *dimRadius) {

	ShDrawerEraseBackGround visitor(this->widget, this->painter);

	auto itr = dimRadius->begin();
	for (itr; itr != dimRadius->end(); ++itr)
		(*itr)->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimRadiusData data = dimRadius->getData();

	GLColor color(0, 0, 0);

	double angle = math::getAbsAngle(data.dim.x, data.dim.y, data.text.x, data.text.y);
	dimRadius->getDimensionStyle()->getDimensionArrowStyle().drawArrow(f, data.dim, angle, color);

	int dx, dy;
	f.convertEntityToDevice(data.text.x, data.text.y, dx, dy);
	angle = math::getAbsAngle(data.center.x, data.center.y, data.text.x, data.text.y);
	QColor qColor(0, 0, 0);

	if (this->painter->isActive() == false)
		this->painter->begin(this->widget);

	dimRadius->getDimensionStyle()->getDimensionTextStyle().drawDimensionRadiusText(this->painter,
		dx, dy, angle, dimRadius->getRadius(), qColor, this->widget->getZoomRate());
}

void ShDrawerEraseBackGround::visit(ShDimDiameter *dimDiameter) {

	ShDrawerEraseBackGround visitor(this->widget, this->painter);

	auto itr = dimDiameter->begin();
	for (itr; itr != dimDiameter->end(); ++itr)
		(*itr)->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimDiameterData data = dimDiameter->getData();

	GLColor color(0, 0, 0);

	double angle = math::getAbsAngle(data.firstDim.x, data.firstDim.y, data.text.x, data.text.y);
	dimDiameter->getDimensionStyle()->getDimensionArrowStyle().drawArrow(f, data.firstDim, angle, color);

	if (dimDiameter->isCenterToTextDistanceShorterThanRadius() == false) {
		angle = math::getAbsAngle(data.firstDim.x, data.firstDim.y, data.secondDim.x, data.secondDim.y);
		dimDiameter->getDimensionStyle()->getDimensionArrowStyle().drawArrow(f, data.secondDim, angle, color);
	}

	int dx, dy;
	f.convertEntityToDevice(data.text.x, data.text.y, dx, dy);
	angle = math::getAbsAngle(data.center.x, data.center.y, data.text.x, data.text.y);
	QColor qColor(0, 0, 0);

	if (this->painter->isActive() == false)
		this->painter->begin(this->widget);

	dimDiameter->getDimensionStyle()->getDimensionTextStyle().drawDimensionDiameterText(this->painter,
		dx, dy, angle, dimDiameter->getDiameter(), qColor, this->widget->getZoomRate());
}

void ShDrawerEraseBackGround::visit(ShDimArcLength *dimArcLength) {

	ShDrawerEraseBackGround visitor(this->widget, this->painter);

	auto itr = dimArcLength->begin();
	for (itr; itr != dimArcLength->end(); ++itr)
		(*itr)->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimArcLengthData data = dimArcLength->getData();

	GLColor color(0, 0, 0);

	dimArcLength->getDimensionStyle()->getDimensionArrowStyle().drawArcArrow(f, data.center, dimArcLength->getArcRadius(),
		dimArcLength->getArcStart(), dimArcLength->getArcEnd(), color);

	int dx, dy;
	f.convertEntityToDevice(data.text.x, data.text.y, dx, dy);
	double angle = math::getAbsAngle(data.center.x, data.center.y, data.text.x, data.text.y);
	QColor qColor(0, 0, 0);

	if (this->painter->isActive() == false)
		this->painter->begin(this->widget);

	dimArcLength->getDimensionStyle()->getDimensionTextStyle().drawDimensionArcLengthText(this->painter,
		dx, dy, angle - 90, dimArcLength->getArcLength(), qColor, this->widget->getZoomRate());
}

void ShDrawerEraseBackGround::visit(ShDimAngular *dimAngular) {

	ShDrawerEraseBackGround visitor(this->widget, this->painter);

	auto itr = dimAngular->begin();
	for (itr; itr != dimAngular->end(); ++itr)
		(*itr)->accept(&visitor);

	ShDrawerFunctions f(this->widget);
	ShDimAngularData data = dimAngular->getData();

	GLColor color(0, 0, 0);

	dimAngular->getDimensionStyle()->getDimensionArrowStyle().drawArcArrow(f, data.center, dimAngular->getArcRadius(),
		dimAngular->getArcStart(), dimAngular->getArcEnd(), color);

	int dx, dy;
	f.convertEntityToDevice(data.text.x, data.text.y, dx, dy);
	double angle = math::getAbsAngle(data.center.x, data.center.y, data.text.x, data.text.y);
	QColor qColor(0, 0, 0);

	if (this->painter->isActive() == false)
		this->painter->begin(this->widget);

	dimAngular->getDimensionStyle()->getDimensionTextStyle().drawDimensionAngleText(this->painter,
		dx, dy, angle - 90, dimAngular->getAngle(), qColor, this->widget->getZoomRate());
}
//////////////////////////////////////////////////////////////////////////


ShApparentExtensionDrawer::ShApparentExtensionDrawer(ShCADWidget *widget, QPainter *painter)
	:ShDrawer(widget, painter) {

}


ShApparentExtensionDrawer::~ShApparentExtensionDrawer() {

}

void ShApparentExtensionDrawer::visit(ShLine *line) {

	if (painter->isActive() == false)
		painter->begin(this->widget);
	
	int dx, dy, dx2, dy2;

	QPen oldPen = painter->pen();
	QPen pen;
	pen.setWidth(2);
	pen.setStyle(Qt::PenStyle::DotLine);
	pen.setColor(QColor(000, 204, 000));
	painter->setPen(pen);

	this->widget->convertEntityToDevice(this->start.x, this->start.y, dx, dy);
	this->widget->convertEntityToDevice(this->end.x, this->end.y, dx2, dy2);

	painter->drawLine(dx, dy, dx2, dy2);

	painter->setPen(oldPen);
}

void ShApparentExtensionDrawer::visit(ShCircle *circle) {

}

void ShApparentExtensionDrawer::visit(ShArc *arc) {

	if (painter->isActive() == false)
		painter->begin(this->widget);

	int centerX, centerY, radiusX, radiusY, radius;

	this->widget->convertEntityToDevice(arc->getCenter().x, arc->getCenter().y, centerX, centerY);
	this->widget->convertEntityToDevice(arc->getCenter().x + arc->getRadius(), arc->getCenter().y, radiusX, radiusY);

	radius = math::toInt(math::getDistance(centerX, centerY, radiusX, radiusY));
	
	QRectF rect(centerX - radius, centerY - radius, radius * 2, radius * 2);
	
	double startAngle = math::getAbsAngle(arc->getCenter().x, arc->getCenter().y, this->start.x, this->start.y);
	double endAngle = math::getAbsAngle(arc->getCenter().x, arc->getCenter().y, this->end.x, this->end.y);
	double disAngle;

	if (this->start == arc->getStart())
		disAngle = math::getAngleDifference(endAngle, startAngle);
	else
		disAngle = math::getAngleDifference(startAngle, endAngle);

	QPen oldPen = painter->pen();
	QPen pen;
	pen.setWidth(2);
	pen.setStyle(Qt::PenStyle::DotLine);
	pen.setColor(QColor(000, 204, 000));
	painter->setPen(pen);

	if (this->start == arc->getStart())
		painter->drawArc(rect, math::toInt(endAngle * 16), math::toInt(disAngle * 16));
	else
		painter->drawArc(rect, math::toInt(startAngle * 16), math::toInt(disAngle * 16));

	painter->setPen(oldPen);

}