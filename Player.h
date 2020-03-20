#pragma once
#include "Animation.h"
#include "Skeleton.h"

class Player
{
public:
    Player(Animation *a, Skeleton *s)
    {
        Player::ani = a;
        Player::skel = s;
    }

    Animation *ani;
    Skeleton *skel;

    float timer = 0;

    void update()
    {
        std::vector<float> dofVals;
        for (auto channel : ani->channels)
        {
            dofVals.push_back(channel->evaluate(timer));
        }

        skel->allJoints[0]->offset = glm::vec3(dofVals[0], dofVals[1], dofVals[2]);

        for (int j = 3; j < dofVals.size()-2; j += 3)
        {
            skel->allJoints[j / 3 - 1]->DOFS[0]->setValue(dofVals[j]);
            skel->allJoints[j / 3 - 1]->DOFS[1]->setValue(dofVals[j + 1]);
            skel->allJoints[j / 3 - 1]->DOFS[2]->setValue(dofVals[j + 2]);
        }
        timer += 0.01;

        if (timer > 8) {
            timer = 0;
        }

    }

	void drawchannel(int index)
	{
		if (ani->channels[index]->keyframes.size() > 0)
		{
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glScalef(.20, .25, .25);
			glDisable(GL_LIGHTING);
			glEnable(GL_POINT_SMOOTH);
			glPointSize(5.0);

			//draw curve
			glBegin(GL_LINE_STRIP);
			glColor3f(0, 1, 0);
			for (int i = 0; i < ani->channels[index]->keyframes.size() - 1; i++)
			{
				for (float j = ani->channels[index]->keyframes[i]->time; j < ani->channels[index]->keyframes[i + 1]->time; j += .01)
				{
					float u = (j - ani->channels[index]->keyframes[i]->time) / (ani->channels[index]->keyframes[i + 1]->time - ani->channels[index]->keyframes[i]->time);
					float value = (ani->channels[index]->keyframes[i]->D + u * (ani->channels[index]->keyframes[i]->C + u * (ani->channels[index]->keyframes[i]->B + u * (ani->channels[index]->keyframes[i]->A))));
					glVertex2f(j-.5, value );
				}
			}
			glEnd();

			//draw keyframe
			glColor3f(1, 1, 1);
			glBegin(GL_POINTS);
			for (int i = 0; i < ani->channels[index]->keyframes.size(); i++)
			{
				glVertex2f(ani->channels[index]->keyframes[i]->time -.5, ani->channels[index]->keyframes[i]->val);
			}
			glEnd();

			//draw tangent
			glColor3f(1, 0, 0);
			glBegin(GL_LINES);
			for (int i = 0; i < ani->channels[index]->keyframes.size(); i++)
			{
				float ii = .09;
				float tout = ani->channels[index]->keyframes[i]->tangentOut * (ani->channels[index]->keyframes[i]->time + ii) + (ani->channels[index]->keyframes[i]->val - ani->channels[index]->keyframes[i]->tangentOut * ani->channels[index]->keyframes[i]->time);
				float tin = (-ani->channels[index]->keyframes[i]->tangentIn) * (ani->channels[index]->keyframes[i]->time + ii) + (ani->channels[index]->keyframes[i]->val + ani->channels[index]->keyframes[i]->tangentIn * ani->channels[index]->keyframes[i]->time);
				glVertex2f(ani->channels[index]->keyframes[i]->time - ii -.5, tin);
				glVertex2f(ani->channels[index]->keyframes[i]->time -.5, ani->channels[index]->keyframes[i]->val );
				glVertex2f(ani->channels[index]->keyframes[i]->time -.5, ani->channels[index]->keyframes[i]->val );
				glVertex2f(ani->channels[index]->keyframes[i]->time + ii - .5, tout);
			}
			glEnd();

			glEnable(GL_LIGHTING);
			glDisable(GL_POINT_SMOOTH);
			glPopMatrix();
		}
	}
};
