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
#include "cinder/Text.h"
#include "cinder/Unicode.h"

#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
#include "cinder/Font.h"

using namespace std;
using namespace Bit;

class textButton {

public:

	void draw();
	void update();
	bool moveText(const int &lim);
	string getTextWithLim(const int &lim);
	void setUp(int w, int h, string textRef, vector<string> textSep, DisplayArea da, Multimedia &check, Multimedia &noCheck, float &transitionDuration);
	void setTextColor(const Color &c);
	void computeTexture(const Font &f);

	bool isClickOrTouch(float x, float y);
	void reset();

	Color textColor_;

	Multimedia *checkMedia_;
	Multimedia *noCheckMedia_;

	DisplayArea da_;

	bool pressing_;

	int x_;
	int y_;
	bool isMoveText_;
	string text_ = "text";
	string logText_ = "";
	vector<string> textSepWord_;

	Surface8u rendered_;

	int width_;
	int height_;

	short state_;

	Vec2f posBias_;

	float *transitionDuration_;
	double cTime1_;
	double diff_;
	double tbrCount_;

};