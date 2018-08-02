#pragma once

#include <cinder/app/AppBasic.h>
#include <boost/shared_ptr.hpp>
#include "Configurable.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class customRect : public Configurable
{
public:

	void readConfig(Bit::JsonTree* tree);
	void readParams(Bit::JsonTree* tree, Bit::ParamsRef params);

	void resetPage();
	void setup();
	void update();
	void draw();

	Rectf rect_;

private:

};