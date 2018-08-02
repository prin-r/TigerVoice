#include "customRect.h"

void customRect::readConfig(Bit::JsonTree* tree) {
	rect_ = Rectf(tree->getChildPtr("x1")->getValue<float>() * ci::app::getWindowSize().x,
		tree->getChildPtr("y1")->getValue<float>() * ci::app::getWindowSize().y,
		tree->getChildPtr("x2")->getValue<float>() * ci::app::getWindowSize().x,
		tree->getChildPtr("y2")->getValue<float>() * ci::app::getWindowSize().y);
}

void customRect::readParams(Bit::JsonTree* tree, Bit::ParamsRef params) {

}

void customRect::resetPage() {

}

void customRect::setup() {

}

void customRect::update() {

}

void customRect::draw() {

}