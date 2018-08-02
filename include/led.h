#pragma once

#include <cinder/app/AppBasic.h>
#include <boost/shared_ptr.hpp>
#include "Configurable.h"
#include "customRect.h"

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/Unicode.h"

#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
#include "cinder/Font.h"
#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class led : public Configurable
{
public:

	void readConfig(Bit::JsonTree* tree);
	void readParams(Bit::JsonTree* tree, Bit::ParamsRef params);

	void resetPage();
	void setup();
	void setup(const Rectf &rectRef);
	void update();
	void draw();
	void drawText();

	string ledType_;
	string text_;
	Rectf ledArea_;

	Vec2f topLeftOffset_;
	Font font_;

private:

	void setLedAreaByType(const Rectf &rectRef);

};