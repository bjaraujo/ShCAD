
#include "ShCircle.h"
#include "Visitor Pattern\ShVisitor.h"


ShCircleData::ShCircleData() {

}

ShCircleData::ShCircleData(ShPoint3d &center, double radius)
	:center(center), radius(radius) {

}

ShCircleData::ShCircleData(const ShCircleData &data)
	: center(data.center), radius(data.radius) {


}

bool ShCircleData::operator==(const ShCircleData& data) {

	if (this->center == data.center && this->radius == data.radius)
		return true;

	return false;

}

ShCircleData& ShCircleData::operator=(const ShCircleData& data) {

	this->center = data.center;
	this->radius = data.radius;

	return *this;
}


ShCircle::ShCircle() {


}

ShCircle::ShCircle(const ShCircleData &data)
	:data(data) {


}

ShCircle::ShCircle(const ShCircle& other)
	: data(other.data) {


}

ShCircle& ShCircle::operator=(const ShCircle& other) {

	ShLeaf::operator=(other);

	this->data = other.data;

	return *this;
}

ShCircle::~ShCircle() {


}

ShCircle* ShCircle::Clone() {

	return new ShCircle(*this);
}

void ShCircle::Accept(ShVisitor *shVisitor) {

	shVisitor->Visit(this);

}

void ShCircle::SetData(const ShCircleData &data) {

	this->data = data;

}
