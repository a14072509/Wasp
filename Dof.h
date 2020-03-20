#pragma once

class Dof {

public:
	Dof(float value, float min, float max);
	~Dof();

	void setValue(float val);
	float getValue();
	float getMin();
	float getMax();

	void setMin(float min);
	void setMax(float max);

private:
	float Value;
	float Min;
	float Max;
};
