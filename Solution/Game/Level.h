#pragma once
#include "Subscriber.h"
class Level : public Subscriber
{
public:
	Level();
	~Level();

	void Update(float aDelta);
	void Render();

private:


};

