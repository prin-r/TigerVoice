#include "led.h"

void led::readConfig(Bit::JsonTree* tree) {
	ledType_ = tree->getChildPtr("type")->getValue<string>();
	text_ = tree->getChildPtr("text")->getValue<string>();
	topLeftOffset_ = tree->getChildPtr("topLeftOffset")->getValueT<ci::Vec2f>();
}

void led::readParams(Bit::JsonTree* tree, Bit::ParamsRef params) {

}

void led::resetPage() {

}

void led::drawText() {
	TextLayout layout;
	layout.clear(ColorA(0.0f, 0.0f, 0.0f, 0.0f));

	layout.setFont(font_);
	layout.setColor(Color(1, 1, 1));
	layout.addLine(text_);
	Surface8u rendered = layout.render(true, false);
	gl::enableAdditiveBlending();
	gl::draw(gl::Texture(rendered), Vec2f(ledArea_.x1, ledArea_.y1));
	gl::disableAlphaBlending();
}

void led::setLedAreaByType(const Rectf &rectRef) {
	float x1 = rectRef.x1 + topLeftOffset_.x * rectRef.getWidth();
	float y1 = rectRef.y1 + topLeftOffset_.y * rectRef.getHeight();
	float x2 = 999;
	float y2 = 999;
	if (ledType_ == "a") {
		y2 = y1 + 0.05 * rectRef.getHeight();
		x2 = x1 + (y2 - y1) * 10;
	}
	else if (ledType_ == "b") {
		y2 = y1 + 0.05 * rectRef.getHeight();
		x2 = x1 + (y2 - y1) * 8;
	}
	else if (ledType_ == "c") {
		y2 = y1 + 0.05 * rectRef.getHeight();
		x2 = x1 + (y2 - y1) * 6;
	}
	else if (ledType_ == "d") {
		y2 = y1 + 0.1 * rectRef.getHeight();
		x2 = x1 + (y2 - y1);
	}
	ledArea_ = Rectf(x1, y1, x2, y2);
}

void led::setup() {

}

void led::setup(const Rectf &rectRef) {
	setLedAreaByType(rectRef);
	font_ = Font(loadAsset("KrungsriSimple.ttf"), 40);
}

void led::update() {

}

void led::draw() {
	ci::gl::color(0, 0, 0);
	//ci::gl::drawSolidRect(ledArea_);
	ci::gl::color(1, 1, 1);
	//ci::gl::enableAlphaBlending();
	drawText();
	//ci::gl::disableAlphaBlending();
}