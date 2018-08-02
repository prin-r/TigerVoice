#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"
#include "cinder/Surface.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Font.h"
#include "cinder/app/AppImplMsw.h"
#include "cinder/Vector.h"
#include "Configurable.h"
#include "BitMultimedia.h"
#include "boost/thread/mutex.hpp"
#include "AClass.h"
#include "cinder/app/MouseEvent.h"

using namespace std;
using namespace Bit;

class button {

public:
	void readParams(Bit::JsonTree* tree, Bit::ParamsRef params);
	void readConfig(Bit::JsonTree* tree);

	void draw();
	void update();
	void setUp(int w, int h);
	void setUp(int w, int h, bool it, Multimedia &m1, Multimedia &m2);

	bool isClickOrTouch(float x, float y);

	bool releaseButton() { return state_ == 2; };

	bool isToggle_;

	void reset();

	DisplayArea da_;

	Multimedia tmp;

	Multimedia *buttonMedia1_;
	Multimedia *buttonMedia2_;

	bool pressing_;

	int x_;
	int y_;

	string boothID_;
	int coin_;
	bool isCashScanner_;

	int width_;
	int height_;

	short state_;
	bool p_;
	bool q_;

};
