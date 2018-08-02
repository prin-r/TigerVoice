#include "textButton.h"

void textButton::setUp(int w, int h, string textRef, vector<string> textSep, DisplayArea da, Multimedia &check, Multimedia &noCheck, float &transitionDuration) {
	checkMedia_ = &check;
	noCheckMedia_ = &noCheck;

	da_ = da;
	da.setup();

	pressing_ = false;
	isMoveText_ = false;

	width_ = w;
	height_ = h;

	text_ = textRef;
	textSepWord_ = textSep;
	textColor_ = Color(1,1,1);
	state_ = 0;

	transitionDuration_ = &transitionDuration;

	cTime1_ = std::clock();
	diff_ = 0;
	tbrCount_ = 0;
	posBias_ = Vec2f(0,0);
	TextLayout layout;
	layout.clear(ColorA(0.0f, 0.0f, 0.0f, 0.0f));
}

void textButton::setTextColor(const Color &c) {
	textColor_ = Color(c);
}

void textButton::computeTexture(const Font &f) {
	TextLayout layout;
	layout.clear(Color(0, 0, 0));

	layout.setFont(f);
	layout.setColor(Color(1, 1, 1));
	layout.addLine(text_);
	rendered_ = layout.render(true, false);
}

bool textButton::isClickOrTouch(float x, float y) {
	float sx = da_.getNormalizedPosition().x;
	float ex = sx + da_.getNormalizedSize().x;

	float sy = da_.getNormalizedPosition().y;
	float ey = sy + da_.getNormalizedSize().y;

	pressing_ = (x > sx && x < ex && y > sy && y < ey);
	return pressing_;
}

void textButton::reset()
{
	pressing_ = false;
}

void textButton::draw()
{

	switch (state_) {
	case 0:
		if (checkMedia_->hasVisuals())
			checkMedia_->draw(da_);
		break;
	case 1:
		if (checkMedia_->hasVisuals())
			checkMedia_->draw(da_);
		break;
	case 2:
		if (noCheckMedia_->hasVisuals())
			noCheckMedia_->draw(da_);
		break;
	case 3:
		if (noCheckMedia_->hasVisuals())
			noCheckMedia_->draw(da_);
		break;
	}

	/*if (pressing_) {
		ci::gl::drawString("rfhrhbgfbfdgdgdfartggjnmyhjyhjyth", Vec2f(200, 400), ColorA(0.3f, 0.3f, 0.7f, 1.0f), Font("Arial", 50));
	}*/
}

string textButton::getTextWithLim(const int &lim) {
	string t = "";
	int l = (lim < textSepWord_.size()) ? lim : textSepWord_.size();
	for (int i = 0; i < l; i++) {
		if (textSepWord_[i] == " ") {
			t += "  ";
		}
		else {
			t += textSepWord_[i];
		}
	}
	return t;
}

bool textButton::moveText(const int &lim) {
	isMoveText_ = textSepWord_.size() > 1 && tbrCount_ > *transitionDuration_;
	posBias_ -= Vec2f((*transitionDuration_) * diff_, 0);
	/*if (isMoveText_) {
		string tmp = textSepWord_[0];
		for (int i = 0; i < textSepWord_.size() - 1; i++) {
			textSepWord_[i] = textSepWord_[i + 1];
		}
		textSepWord_[textSepWord_.size() - 1] = tmp;
		text_ = "";
		int l = (lim < textSepWord_.size()) ? lim : textSepWord_.size();
		for (int i = 0; i < l; i++) {
			if (textSepWord_[i] == " ") {
				text_ += "  ";
			}
			else {
				text_ += textSepWord_[i];
			}
		}
		tbrCount_ = 0;
	}
	else {
		tbrCount_ += diff_;
	}*/
	return isMoveText_;
}

void textButton::update()
{
	diff_ = (std::clock() - cTime1_) / (double)CLOCKS_PER_SEC;
	cTime1_ = std::clock();

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