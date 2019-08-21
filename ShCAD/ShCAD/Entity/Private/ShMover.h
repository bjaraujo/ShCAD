
#ifndef _SHMOVER_H
#define _SHMOVER_H

#include "ShVisitor.h"

class ShMover : public ShVisitor {

private:
	double disX;
	double disY;

public:
	ShMover(double disX, double disY);
	~ShMover();

	virtual void visit(ShLine *line);

};

#endif //_SHMOVER_H