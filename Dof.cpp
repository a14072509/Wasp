#include "Dof.h"

Dof::Dof(float value, float min, float max) {
	Value = value;
	Min = min;
	Max = max;
}

void Dof::setValue(float val) {
	if (val > Max) {
		Value = Max;
	}
	else if (val < Min) {
		Value = Min;
	}
	else {
		Value = val;
	}
}

float Dof::getValue() {
	return Value;
}

float Dof::getMin() {
	return Min;
}

float Dof::getMax() {
	return Max;
}

void Dof::setMin(float min) {
	Min = min;
}

void Dof::setMax(float max) {
	Max = max;
}

Dof::~Dof() {}
