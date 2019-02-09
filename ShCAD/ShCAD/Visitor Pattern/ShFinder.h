

#ifndef _SHFINDER_H
#define _SHFINDER_H

#include "ShVisitor.h"

class ShEntity;
class ShLine;
class ShCircle;
class ShArc;
class ShFinder : public ShVisitor {

private:
	double x;
	double y;
	double zoomRate;
	ShEntity* *foundEntity;

public:
	ShFinder(double x, double y, double zoomRate, ShEntity* *foundEntity);
	~ShFinder();

	void Visit(ShLine *line);
	void Visit(ShCircle *circle);
	void Visit(ShArc *arc);

};

#endif //_SHFINDER_H