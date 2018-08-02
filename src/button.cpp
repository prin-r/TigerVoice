#include "button.h"

void button::setUp(int w, int h) {
	buttonMedia1_->setup();
	buttonMedia2_->setup();
	isToggle_ = false;
	pressing_ = false;

	width_ = w;
	height_ = h;

	p_ = false;
	q_ = false;
	state_ = 0;
}

void button::setUp(int w, int h, bool it, Multimedia &m1, Multimedia &m2) {
	isToggle_ = it;
	pressing_ = false;

	buttonMedia1_ = &m1;
	buttonMedia2_ = &m2;

	da_ = m1.getDisplayArea();
	//buttonMedia1_->setup();
	//buttonMedia2_->setup();

	width_ = w;
	height_ = h;

	p_ = false;
	q_ = false;
	state_ = 0;
}

void button::readParams(Bit::JsonTree* tree, Bit::ParamsRef params)
{


}

void button::readConfig(Bit::JsonTree* tree)
{
	tmp.readConfig(tree->getChildPtr("buttonMedia"));
	buttonMedia1_ = &tmp;
	buttonMedia2_ = &tmp;
	boothID_ = (tree->getChildPtr("boothID"))->getValueT<string>();
	coin_ = (tree->getChildPtr("coin"))->getValueT<int>();
	if (tree->hasChild("isCashScanner")) {
		isCashScanner_ = tree->getChildPtr("isCashScanner")->getValueT<bool>();
	}
	else {
		isCashScanner_ = false;
	}
}

bool button::isClickOrTouch(float x, float y) {
	float sx = da_.getNormalizedPosition().x;
	float ex = sx + da_.getNormalizedSize().x;

	float sy = da_.getNormalizedPosition().y;
	float ey = sy + da_.getNormalizedSize().y;

	pressing_ = (x > sx && x < ex && y > sy && y < ey);
	return pressing_;
}

void button::reset()
{
	pressing_ = false;
}

void button::draw()
{
	if (!isToggle_) {
		switch (state_) {
		case 0:
			if (buttonMedia1_->hasVisuals())
				buttonMedia1_->draw();
			break;
		case 1:
			if (buttonMedia2_->hasVisuals())
				buttonMedia2_->draw();
			break;
		case 2:
			if (buttonMedia1_->hasVisuals())
				buttonMedia1_->draw();
			break;
		}
		
	}
	else {
		switch (state_) {
		case 0:
			if (buttonMedia1_->hasVisuals())
				buttonMedia1_->draw();
			break;
		case 1:
			if (buttonMedia1_->hasVisuals())
				buttonMedia1_->draw();
			break;
		case 2:
			if (buttonMedia2_->hasVisuals())
				buttonMedia2_->draw();
			break;
		case 3:
			if (buttonMedia2_->hasVisuals())
				buttonMedia2_->draw();
			break;
		}
	}


	/*if (state_ == 3) {
		ci::gl::drawString("rfhrhbgfbfdgdgdfartggjnmyhjyhjyth", Vec2f(200, 400), ColorA(0.3f, 0.3f, 0.7f, 1.0f), Font("Arial", 50));
	}*/

}

void button::update()
{
	if (!isToggle_) {
		switch (state_) {
		case 0:
			if (pressing_) state_ = 1;
			break;
		case 1:
			if (!pressing_) state_ = 2;
			break;
		case 2:
			state_ = 0;
			break;
		}
	}
	else {
		switch (state_) {
		case 0:
			if (pressing_) state_ = 1;
			break;
		case 1:
			if (!pressing_) state_ = 2;
			break;
		case 2:
			if (pressing_) state_ = 3;
			break;
		case 3:
			if (!pressing_) state_ = 0;
			break;
		}
	}
}