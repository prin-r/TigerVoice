#include "mainPage.h"
#include "math.h"

void mainPage::readConfig(Bit::JsonTree* tree) {
	testString_ = tree->getChildPtr("testAnswer")->getValue<string>();
	mainRectArea_.readConfig(tree->getChildPtr("mainRectArea"));
	//transitionDuration_ = tree->getChildPtr("transitionDuration")->getValue<double>();
	//timeBeetweenRandomization_ = tree->getChildPtr("timeBeetweenRandomization")->getValue<double>();

	answerQueueStride_ = tree->getChildPtr("answerQueueStride")->getValue<float>();

	spaceBetweenFrontAndBack_ = tree->getChildPtr("spaceBetweenFrontAndBack")->getValue<int>();
	numberOfDisplayText_ = tree->getChildPtr("numberOfDisplayText")->getValue<int>();

	displayTextOffset_ = tree->getChildPtr("displayTextOffset")->getValueT<ci::Vec2f>();
	displayTextSpace_ = tree->getChildPtr("displayTextSpace")->getValueT<ci::Vec2f>();
	questionPosition_ = tree->getChildPtr("questionPosition")->getValueT<ci::Vec2f>();

	questionFontSize_ = tree->getChildPtr("questionFontSize")->getValue<int>();
	answerFontSize_ = tree->getChildPtr("answerFontSize")->getValue<int>();
	maxShownAnswers_ = tree->getChildPtr("maxShownAnswers")->getValue<int>();

	questionPath_ = tree->getChildPtr("questionPath")->getValue<string>();

	currentTb_.resize(numberOfDisplayText_);

	//----------------------------------------------------------------------------------------------------------------------
	Bit::JsonTree* tnn = tree->getChildPtr("thaiNormal");
	for (Bit::JsonTree::Iter l = tnn->begin(); l != tnn->end(); l++)
	{
		string tmp = l->getValue<string>();
		thaiNormal_.push_back(tmp);
	}
	thaiNormal_.shrink_to_fit();
	//----------------------------------------------------------------------------------------------------------------------
	Bit::JsonTree* tnu = tree->getChildPtr("thaiUpper");
	for (Bit::JsonTree::Iter l = tnu->begin(); l != tnu->end(); l++)
	{
		string tmp = l->getValue<string>();
		thaiUpper_.push_back(tmp);
	}
	thaiUpper_.shrink_to_fit();
	//----------------------------------------------------------------------------------------------------------------------
	Bit::JsonTree* enl = tree->getChildPtr("engLower");
	for (Bit::JsonTree::Iter l = enl->begin(); l != enl->end(); l++)
	{
		string tmp = l->getValue<string>();
		engLower_.push_back(tmp);
	}
	engLower_.shrink_to_fit();
	//----------------------------------------------------------------------------------------------------------------------	
	Bit::JsonTree* enu = tree->getChildPtr("engUpper");
	for (Bit::JsonTree::Iter l = enu->begin(); l != enu->end(); l++)
	{
		string tmp = l->getValue<string>();
		engUpper_.push_back(tmp);
	}
	engUpper_.shrink_to_fit();
	//----------------------------------------------------------------------------------------------------------------------

	webControlClient_.readConfig(tree->getChildPtr("webControlClient"));
	approveButton_.readConfig(tree->getChildPtr("approveButton"));
	fontName_ = tree->getChildPtr("fontName")->getValue<string>();

	bg_.readConfig(tree->getChildPtr("bg"));

	checkMedia_.readConfig(tree->getChildPtr("checkButton"));
	noCheckMedia_.readConfig(tree->getChildPtr("noCheckButton"));

	refresh_.readConfig(tree->getChildPtr("refresh"));
	noRefresh_.readConfig(tree->getChildPtr("noRefresh"));

	showAllAnswer_.readConfig(tree->getChildPtr("showAllAnswer"));
	noshowAllAnswer_.readConfig(tree->getChildPtr("noshowAllAnswer"));

	skip_.readConfig(tree->getChildPtr("skip"));
	noSkip_.readConfig(tree->getChildPtr("noSkip"));

	clear_.readConfig(tree->getChildPtr("clear"));
	noClear_.readConfig(tree->getChildPtr("noClear"));

	Bit::JsonTree* pv = tree->getChildPtr("previewArea");
	float x1 = pv->getChildPtr("x1")->getValue<float>() * getWindowWidth();
	float y1 = pv->getChildPtr("y2")->getValue<float>() * getWindowHeight();
	float x2 = pv->getChildPtr("x2")->getValue<float>() * getWindowWidth();
	float y2 = pv->getChildPtr("y1")->getValue<float>() * getWindowHeight();
	previewFrame_ = Rectf(x1,y1,x2,y2);
}

void mainPage::readParams(Bit::JsonTree* tree, Bit::ParamsRef params) {
	//params->addParam<float>(tree->getChildPtr("textMoveDuration"), textMoveDuration_);
	params->addParam<float>(tree->getChildPtr("textMoveSpeed"), textMoveSpeed_);
	params->addParam<double>(tree->getChildPtr("nextMessageCountDown"), nextMessageCountDown_);

	params->addParam<double>(tree->getChildPtr("transitionDuration"), transitionDuration_);
	params->addParam<double>(tree->getChildPtr("timeBeetweenRandomization"), timeBeetweenRandomization_);
	params->addParam<float>(tree->getChildPtr("textShadowAlpha"), textShadowAlpha_);

	params->addParam<int>(tree->getChildPtr("rightBound"), rightBound_);
	params->addParam<int>(tree->getChildPtr("leftBound"), leftBound_);
	
	maxTextInLine_ = tree->getChildPtr("maxTextInLine")->getValueT<int>();

	Bit::JsonTree* loqTmp = tree->getChildPtr("listOfQuestion");
	for (Bit::JsonTree::Iter q = loqTmp->begin(); q != loqTmp->end(); q++)
	{
		string tmpS = q->getValue<string>();
		addQuestion(tmpS);
	}
	listOfQuestion_.shrink_to_fit();

	for (int i = 1; i < listOfQuestion_.size(); i++) {
		listOfQuestion_[i].state_ = 2;
	}
}

void mainPage::resetPage() {

}

void mainPage::sending() {
	if (bInitialized_) {
		// Grab the screen (current read buffer) into the local spout texture
		spoutTexture_.bind();
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, getWindowWidth(), getWindowHeight());
		spoutTexture_.unbind();

		// Send the texture for all receivers to use
		// NOTE : if SendTexture is called with a framebuffer object bound,
		// include the FBO id as an argument so that the binding is restored afterwards
		// because Spout uses an fbo for intermediate rendering
		spoutsender_.SendTexture(spoutTexture_.getId(), spoutTexture_.getTarget(), getWindowWidth(), getWindowHeight());

	}
}

void mainPage::readFile() {
	std::ifstream infile(questionPath_);
	std::string line;
	int i = 0;
	while (std::getline(infile, line))
	{
		if (i < listOfQuestion_.size()) {
			listOfQuestion_[i].text_ = line;
			listOfQuestion_[i].computeTexture(font_[30]);
			i++;
		}
		else {
			break;
		}
	}
	infile.close();
	computeQuestionTexture();
}

void mainPage::shutdown() {
	spoutsender_.ReleaseSender();
}

void mainPage::setup() {
	ci::app::getWindow()->connectMouseDown(&mainPage::mouseDown, this);
	ci::app::getWindow()->connectMouseUp(&mainPage::mouseUp, this);

	ci::app::getWindow()->connectTouchesBegan(&mainPage::touchesBegan, this);
	ci::app::getWindow()->connectTouchesEnded(&mainPage::touchesEnded, this);

	questionPosition_ *= Vec2f(app::getWindowWidth(), app::getWindowHeight());

	webControlClient_.connectToServer("Longrak");

	spoutTexture_ = ci::gl::Texture(getWindowWidth(), getWindowHeight());
	strcpy_s(SenderName_, "CinderSpoutSender"); // we have to set a sender name first

	bInitialized_ = spoutsender_.CreateSender(SenderName_, getWindowWidth(), getWindowHeight());
	bMemoryMode_ = spoutsender_.GetMemoryShareMode();
	receiveSomeThing_ = false;
	changeQuestion_ = false;

	messagesTrueSize_ = 0;
	tr2Counter_ = 0;
	nextMessageCounter_ = nextMessageCountDown_;
	
	diff_ = 0;
	state_ = 0;
	cTime1_ = std::clock();
	changingProb_ = 100;
	tbrCount_ = 0;
	isFadeInState_ = false;

	width_ = getWindowWidth();
	height_ = getWindowHeight();

	displayTextOffset_ *= Vec2f(width_,height_);
	displayTextSpace_ *= Vec2f(width_, height_);

	pressing_ = false;

	int allFont = 120;
	for (int i = 0; i < allFont; i++) {
		font_.push_back(Font(loadAsset(fontName_), i+1));
	}

	approveButton_.setUp(getWindowWidth(), getWindowHeight());
	bg_.setup();
	checkMedia_.setup();
	noCheckMedia_.setup();

	refresh_.setup();
	noRefresh_.setup();

	showAllAnswer_.setup();
	noshowAllAnswer_.setup();

	skip_.setup();
	noSkip_.setup();

	clear_.setup();
	noClear_.setup();

	initRendered(2);

	textButtons_.clear();
	vector<string> aqpb;
	aqpb.push_back("");
	addTextButton("", aqpb,Color(1,1,1),"");

	button refresh;
	button showAllAnswer;
	button skip;
	button clear;

	refresh.setUp(width_, height_, false, noRefresh_, refresh_);
	showAllAnswer.setUp(width_, height_, true, noshowAllAnswer_,showAllAnswer_ );
	skip.setUp(width_, height_, false, noSkip_, skip_);
	clear.setUp(width_, height_, false, noClear_,clear_ );

	uiButtons_.push_back(refresh);
	uiButtons_.push_back(showAllAnswer);
	uiButtons_.push_back(skip);
	uiButtons_.push_back(clear);

	for (int i = 0; i < currentTb_.size(); i++) {
		currentTb_[i] = textButtons_.front();
	}

	computeQuestionTexture();
	/*
	ofstream myfile;
	myfile.open("C://Users//Q-Bit///DESKTOP//output.txt");
	printCharSurfaceSize(engUpper_, myfile);
	printCharSurfaceSize(engLower_, myfile);

	printCharSurfaceSize(thaiNormal_, myfile);
	printCharSurfaceSize(thaiUpper_, myfile);
	printCharSurfaceSize(thaiLower_, myfile);
	myfile.close();
	*/
	logAccept_ = Log("logAccept");
	logAccept_.setFormat(".csv");

	logReject_ = Log("logReject");
	logReject_.setFormat(".csv");

	readFile();
}

void mainPage::printCharSurfaceSize(const vector<string> &a, ofstream &myfile) {
	for (int i = 0; i < a.size(); i++) {
		drawText(rendered_[0][1], "a", Vec2f(100, 100), Color(1, 1, 1), answerFontSize_);
		drawText(rendered_[0][0], a[i] + "a", Vec2f(100, 100), Color(1, 1, 1), answerFontSize_);
		myfile << a[i] + " " + std::to_string(rendered_[0][0].getWidth() - rendered_[0][1].getWidth()) << endl;
	}
}

void mainPage::initRendered(int times) {
	TextLayout layout;
	messageForDisplayRandomly_.resize(numberOfDisplayText_);
	rendered_.resize(numberOfDisplayText_);
	for (int i = 0; i < numberOfDisplayText_; i++) {
		rendered_[i].resize(times);
		for (int j = 0; j < times; j++) {
			messageForDisplayRandomly_[i].push_back("");
			rendered_[i][j] = layout.render(true, false);
			drawText(rendered_[i][j], "", Vec2f(0, 0), Color(0, 0, 0), answerFontSize_);
		}
		messageForDisplayRandomly_[i].shrink_to_fit();
	}
	messageForDisplayRandomly_.shrink_to_fit();
}

void mainPage::receiveSomeThing() {
	ci::JsonTree tree;
	Bit::WebControlHeader header;

	receiveSomeThing_ = false;
	//read
	// header contains user id.
	while (webControlClient_.readJson(tree, header))
	{
		//app::console() << tree.serialize() << endl;
		try{
			receiveSomeThing_ = true;
			vector<string> tmp;

			string logS = Bit::Time::getCurrentTimeInSecondInString();
			logS += "," + base64_decode(tree.getChild("fb_name").getValue<string>());
			logS += "," + base64_decode(tree.getChild("email").getValue<string>());
			logS += "," + base64_decode(tree.getChild("link").getValue<string>());
			logS += "," + base64_decode(tree.getChild("age_range").getValue<string>());
			logS += "," + base64_decode(tree.getChild("gender").getValue<string>());
			logS += "," + base64_decode(tree.getChild("share").getValue<string>());

			float r = tree.getChild("r").getValue<float>();
			float g = tree.getChild("g").getValue<float>();
			float b = tree.getChild("b").getValue<float>();

			for (Bit::JsonTree::Iter l = tree.getChild("message").begin(); l != tree.getChild("message").end(); l++)
			{
				string s = l->getValue<string>();
				tmp.push_back(base64_decode(s));
			}

			/*int spaceCounter = 0;
			while (tmp.size() < maxTextInLine_) {
				spaceCounter++;
				tmp.push_back(" ");
			}

			for (int i = 0; i < spaceBetweenFrontAndBack_ && spaceCounter < spaceBetweenFrontAndBack_; i++) {
				spaceCounter++;
				tmp.push_back(" ");
			}*/

			//ci::app::console() << tree.serialize() << std::endl;

			tmp.shrink_to_fit();
			messagesTrueSize_ = tmp.size();

			string sNoSpace = "";
			for (int i = 0; i < tmp.size(); i++) {
				sNoSpace += tmp[i];
			}

			string sNoSpaceNoComma = sNoSpace + ",";
			boost::erase_all(sNoSpaceNoComma, ",");

			logS += "," + sNoSpaceNoComma;

			//log_.addLog(logS, false);

			addTextButton(sNoSpace, tmp, Color(r, g, b), logS);
			/*
			if (tmp.size() != textButtons_.back().textSepWord_.size()) {
				addTextButton(sNoSpace,tmp,Color(r,g,b));
			}
			else {
				bool isEqual = true;
				int minSize = (tmp.size() < textButtons_.back().textSepWord_.size()) ? tmp.size() : textButtons_.back().textSepWord_.size();
				for (int i = 0; i < minSize; i++) {
					if (tmp[i] != textButtons_.back().textSepWord_[i]) {
						isEqual = false;
						break;
					}
				}
				if (!isEqual) {
					addTextButton(sNoSpace, tmp, Color(r, g, b));
				}
			}
			*/
		}
		catch (...)
		{
			ci::app::console() << "receiveSomeThing() receiveSomeThing() receiveSomeThing()" << std::endl;
		}
	}
}

void mainPage::drawTextFromVOS(Surface8u &rend, const vector<string> &vos, const Vec2f &pos, const Color &c, int fontSize) {
	if (vos.size() < 31) {
		TextLayout layout;
		layout.clear(ColorA(0.0f, 0.0f, 0.0f, 0.0f));
		layout.setFont(font_[fontSize]);
		layout.setColor(c);
		string s2t = "";
		for (int i = 0; i < vos.size(); i++) {
			s2t += vos[i];
		}
		layout.addLine(s2t);
		rend = layout.render(true, false);
		ci::gl::enableAlphaBlending();
		ci::gl::draw(ci::gl::Texture(rend), pos);
		ci::gl::disableAlphaBlending();
	}
	else {
		Surface8u sf1;
		Surface8u sf2;

		string s2t = "";
		string s2t1 = "";

		for (int i = 0; i < vos.size(); i++) {
			if (i < 31) {
				s2t += vos[i];
			}
			else {
				s2t1 += vos[i];
			}
		}

		TextLayout layout;
		layout.clear(ColorA(0.0f, 0.0f, 0.0f, 0.0f));
		layout.setFont(font_[fontSize]);
		layout.setColor(c);
		layout.addLine(s2t);
		sf1 = layout.render(true, false);

		TextLayout layout2;
		layout2.clear(ColorA(0.0f, 0.0f, 0.0f, 0.0f));
		layout2.setFont(font_[fontSize]);
		layout2.setColor(c);
		layout2.addLine(s2t1);
		sf2 = layout2.render(true, false);

		rend = Surface8u(sf1.getWidth() + sf2.getWidth(), sf1.getHeight(), SurfaceChannelOrder::RGBA);

		Area area(0, 0, sf1.getWidth(), sf1.getHeight());
		Surface::Iter iter = (&rend)->getIter(area);

		Surface::Iter sf1Iter = (&sf1)->getIter();
		Surface::Iter sf2Iter = (&sf2)->getIter();

		while (iter.line() && sf1Iter.line()) {
			while (iter.pixel() && sf1Iter.pixel()) {
				iter.r() = sf1Iter.r();
				iter.g() = sf1Iter.g();
				iter.b() = sf1Iter.b();
			}
		}

		Area area2(sf1.getWidth(), 0, sf1.getWidth() + sf2.getWidth(), sf1.getHeight());
		Surface::Iter iter2 = (&rend)->getIter(area2);

		while (iter2.line() && sf2Iter.line()) {
			while (iter2.pixel() && sf2Iter.pixel()) {
				iter2.r() = sf2Iter.r();
				iter2.g() = sf2Iter.g();
				iter2.b() = sf2Iter.b();
			}
		}

		ci::gl::enableAlphaBlending();
		ci::gl::draw(ci::gl::Texture(rend), pos);
		ci::gl::disableAlphaBlending();

	}
}

void mainPage::drawText(Surface8u &rend, const string &s2t, const Vec2f &pos, const Color &c, int fontSize) {
	TextLayout layout;
	layout.clear(ColorA(0.0f, 0.0f, 0.0f, 0.0f));

	layout.setFont(font_[fontSize]);
	layout.setColor(c);
	layout.addLine(s2t);
	rend = layout.render(true, false);
	ci::gl::enableAlphaBlending();
	ci::gl::draw(ci::gl::Texture(rend), pos);
	ci::gl::disableAlphaBlending();
}

void mainPage::drawTextAdditive(Surface8u &rend, const string &s2t, const Vec2f &pos, const ColorA &c, int fontSize) {
	TextLayout layout;
	layout.clear(ColorA(0.0f, 0.0f, 0.0f, 0.0f));

	layout.setFont(font_[fontSize]);
	layout.setColor(c);
	layout.addLine(s2t);
	rend = layout.render(true, false);
	ci::gl::enableAlphaBlending();
	ci::gl::draw(ci::gl::Texture(rend), pos);
	ci::gl::disableAlphaBlending();
}

void mainPage::drawText(Surface8u &rend, const string &s2t, const Vec2f &pos, const Color &c, const Color &clearColor, int fontSize) {
	TextLayout layout;
	layout.clear(clearColor);

	layout.setFont(font_[fontSize]);
	layout.setColor(c);
	layout.addLine(s2t);
	rend = layout.render(true, false);
	ci::gl::enableAlphaBlending();
	ci::gl::draw(ci::gl::Texture(rend), pos);
	ci::gl::disableAlphaBlending();
}

void mainPage::computeQuestionTexture() {
	for (int i = 0; i < listOfQuestion_.size(); i++) {
		if (listOfQuestion_[i].state_ == 0) {
			//drawText(rendered_[0], listOfQuestion_[i].text_, Vec2f(500, 100), Color(1, 1, 1), questionFontSize_);
			TextLayout layout;
			layout.clear(ColorA(0, 0, 0, 0));

			layout.setFont(font_[questionFontSize_]);
			layout.setColor(Color(1, 1, 1));
			layout.addLine(listOfQuestion_[i].text_);
			questionRendered_ = layout.render(true, false);

			//ci::gl::draw(ci::gl::Texture(questionRendered_), questionPosition_ - 0.5 * Vec2f(questionRendered_.getWidth(), questionRendered_.getHeight()));
			//ci::gl::drawString(listOfQuestion_[i].text_, questionPosition_, ColorA(1, 1, 1, 1), font_[questionFontSize_]);
			break;
		}
	}
}

void mainPage::clearList() {
	textButtons_.clear();
	vector<string> aqpb;
	aqpb.push_back("");
	addTextButton("", aqpb, currentTb_[0].textColor_, "");
	nextMessageCounter_ = nextMessageCountDown_;
	for (int i = 0; i < currentTb_.size(); i++) {
		currentTb_[i].text_ = "";
		currentTb_[i].textSepWord_.clear();
		currentTb_[i].textSepWord_.shrink_to_fit();
		TextLayout layout;
		layout.clear(ColorA(0.0f, 0.0f, 0.0f, 0.0f));
		currentTb_[i].rendered_ = layout.render(true, false);
	}

	for (int i = 0; i < rendered_.size(); i++) {
		drawText(rendered_[i][0], "", Vec2f(0, 0), Color(0, 0, 0), 25);
		drawText(rendered_[i][1], "", Vec2f(0, 0), Color(0, 0, 0), 25);
	}
	changingProb_ = 100;
	state_ = IDLE;
}

string mainPage::textRandomization(textButton &tbRef) {
	string tmp = "";
	string val = "";
	if (textButtons_.empty()) return "";

	if (changingProb_ < 5) {
		return tbRef.getTextWithLim(maxTextInLine_);
	}
	int limText = (maxTextInLine_ < tbRef.textSepWord_.size()) ? maxTextInLine_ : tbRef.textSepWord_.size();
	for (int i = 0; i < limText; i++) {
		val = tbRef.textSepWord_[i];
		int r = rand() % 100;
		if (r < changingProb_) {
			if ((std::find(engLower_.begin(), engLower_.end(), val) != engLower_.end())) {
				tmp += engLower_[rand() % engLower_.size()];
			}
			else if ((std::find(engUpper_.begin(), engUpper_.end(), val) != engUpper_.end())) {
				tmp += engUpper_[rand() % engUpper_.size()];
			}
			else if ((std::find(thaiUpper_.begin(), thaiUpper_.end(), val) != thaiUpper_.end())) {
				tmp += thaiUpper_[rand() % thaiUpper_.size()];
			}
			else if (val == " ") {
				tmp += " ";
			}
			else {
				tmp += thaiNormal_[rand() % thaiNormal_.size()];
			}
		}
		else {
			tmp += val;
		}
	}
	return tmp;
}

string mainPage::textRandomization2() {

	if (textButtons_.empty()) return "";

	if (tr2Counter_ >= textButtons_.front().textSepWord_.size()) {
		string s = "";
		for (int i = 0; i < textButtons_.front().textSepWord_.size(); i++) {
			s += textButtons_.front().textSepWord_[i];
		}
		return s;
	}

	string tmp = "";
	string val = "";
	for (int i = 0; i < tr2Counter_; i++) {
		val = textButtons_.front().textSepWord_[i];
		int r = rand() % 100;
		if (i == tr2Counter_ - 1) {
			if ((std::find(engLower_.begin(), engLower_.end(), val) != engLower_.end())) {
				tmp += engLower_[rand() % engLower_.size()];
			}
			else if ((std::find(engUpper_.begin(), engUpper_.end(), val) != engUpper_.end())) {
				tmp += engUpper_[rand() % engUpper_.size()];
			}
			else if (val == " ") {
				tmp += " ";
			}
			else {
				tmp += thaiNormal_[rand() % thaiNormal_.size()];
			}
		}
		else {
			tmp += val;
		}
	}
	return tmp;
}

void mainPage::addQuestion(string s) {
	textButton tb;
	DisplayArea da = checkMedia_.getDisplayArea();
	da.setNormalizedPosition(checkMedia_.getDisplayArea().getNormalizedPosition());
	vector<string> vs;
	vs.push_back("");
	tb.setUp(getWindowWidth(), getWindowHeight(), s, vs, da, checkMedia_, noCheckMedia_, textMoveSpeed_);
	listOfQuestion_.push_back(tb);
	listOfQuestion_.back().setTextColor(Color(1, 1, 1));
}

void mainPage::addTextButton(string s, vector<string> vs, Color c, string logText) {
	textButton tb;
	DisplayArea da = checkMedia_.getDisplayArea();
	da.setNormalizedPosition(checkMedia_.getDisplayArea().getNormalizedPosition());
	
	tb.setUp(getWindowWidth(), getWindowHeight(), s, vs, da, checkMedia_, noCheckMedia_, textMoveSpeed_);
	tb.logText_ = logText;
	textButtons_.push_back(tb);
	textButtons_.back().setTextColor(c);
	drawText(textButtons_.back().rendered_, textButtons_.back().text_, Vec2f(0, 0), Color(1,1,1), 20);
}

void mainPage::update() {

	receiveSomeThing();


	for (std::list<button>::iterator b = uiButtons_.begin(); b != uiButtons_.end(); ++b) {
		b->update();
	}

	for (int i = 0; i < listOfQuestion_.size(); i++) {
		listOfQuestion_[i].update();
	}

	if (changeQuestion_) {
		computeQuestionTexture();
		changeQuestion_ = false;
	}

	diff_ = (std::clock() - cTime1_) / (double)CLOCKS_PER_SEC;
	cTime1_ = std::clock();

	if (std::next(uiButtons_.begin(), 3)->releaseButton()) clearList();
	if (std::next(uiButtons_.begin(), 0)->releaseButton()) readFile();

	switch (state_) {
	case IDLE:
		if (textButtons_.size() > 1 || (std::next(uiButtons_.begin(), 2)->releaseButton() && textButtons_.size() > 1)) {
			textButtons_.pop_front();
			if (textButtons_.front().logText_ != "") {
				if (textButtons_.front().state_ != 2) {
					logAccept_.addLog(textButtons_.front().logText_, false);
				}
				else {
					logReject_.addLog(textButtons_.front().logText_, false);
				}

			}

			std::list<textButton>::iterator tb = textButtons_.begin();
			if (tb->state_ != 2) {
				tr2Counter_ = 0;
				nextMessageCounter_ = nextMessageCountDown_;
				changingProb_ = 100;
				state_ = FADE_IN;
			}
		}
		else {
			for (int i = 0; i < currentTb_.size(); i++) {
				currentTb_[i].moveText(maxTextInLine_);
				currentTb_[i].update();
				messageForDisplayRandomly_[i][1] = currentTb_[i].text_;
				messageForDisplayRandomly_[i][0] = currentTb_[i].text_;
			}
		}
		break;
	case FADE_IN:
		if (std::next(uiButtons_.begin(), 1)->state_ == 0) {
			for (int i = 0; i < currentTb_.size(); i++) {
				currentTb_[i] = textButtons_.front();
			}
		}
		else {
			std::list<textButton>::iterator tb = textButtons_.begin();
			int i = 0;
			while (i < currentTb_.size() && tb != textButtons_.end()) {
				if (tb->state_ != 2) {
					currentTb_[i] = *tb;
					i++;
				}
				tb++;
			}
			int j = 0;
			while (i < currentTb_.size()) {
				currentTb_[i] = currentTb_[j];
				i++;
				j++;
			}
		}

		if (tbrCount_ > timeBeetweenRandomization_) {
			for (int i = 0; i < numberOfDisplayText_; i++) {
				messageForDisplayRandomly_[i][1] = messageForDisplayRandomly_[i][0];
				messageForDisplayRandomly_[i][0] = textRandomization(currentTb_[i]);
			}
			tbrCount_ = 0;
		}
		else {
			tbrCount_ += diff_;
		}
		
		changingProb_ -= diff_ * (100.0f / transitionDuration_);
		if (changingProb_ < 0) {
			state_ = DRAW_IDLE;
		}
		break;
	case DRAW_IDLE:

		nextMessageCounter_ -= diff_;
		for (int i = 0; i < currentTb_.size(); i++) {
			currentTb_[i].moveText(maxTextInLine_);
			currentTb_[i].update();
			messageForDisplayRandomly_[i][1] = currentTb_[i].text_;
			messageForDisplayRandomly_[i][0] = currentTb_[i].text_;
		}

		if ((nextMessageCounter_ < 0 || std::next(uiButtons_.begin(), 2)->releaseButton()) && textButtons_.size() > 1) {
			textButtons_.pop_front();
			if (textButtons_.front().logText_ != "") {
				if (textButtons_.front().state_ != 2) {
					logAccept_.addLog(textButtons_.front().logText_, false);
				}
				else {
					logReject_.addLog(textButtons_.front().logText_, false);
				}
			}

			std::list<textButton>::iterator tb = textButtons_.begin();
			if (tb->state_ != 2) {
				changingProb_ = 100;
				tr2Counter_ = 0;
				nextMessageCounter_ = nextMessageCountDown_;
				state_ = FADE_IN;
			}
			else {
				nextMessageCounter_ = nextMessageCountDown_;
				changingProb_ = 100;
				state_ = IDLE;
			}
		}
		else if (textButtons_.size() <= 1 && nextMessageCounter_ < 1) {
			nextMessageCounter_ = nextMessageCountDown_;
			changingProb_ = 100;
			state_ = IDLE;
		}
		break;
	case FADE_OUT:
		break;
	}

	int c = 1;
	for (std::list<textButton>::iterator tb = textButtons_.begin(); tb != textButtons_.end(); ++tb) {
		Vec2f pos = checkMedia_.getDisplayArea().getPosition() + Vec2f(0, height_ * answerQueueStride_ * (c++));
		pos = pos / Vec2f(width_, height_);
		tb->da_.setNormalizedPosition(pos);
		tb->update();
	}
}

void mainPage::draw() {

	ci::gl::enableAlphaBlending();
	ci::gl::draw(ci::gl::Texture(questionRendered_), questionPosition_ - 0.5 * Vec2f(questionRendered_.getWidth(), questionRendered_.getHeight()));
	ci::gl::disableAlphaBlending();

	switch (state_) {
	case FADE_IN:
		isFadeInState_ = true;
		if (tbrCount_ == 0) {
			for (int i = 0; i < numberOfDisplayText_; i++) {
				drawTextAdditive(rendered_[i][1], messageForDisplayRandomly_[i][1], displayTextOffset_ + i * displayTextSpace_, ColorA(currentTb_[i].textColor_.r, currentTb_[i].textColor_.g, currentTb_[i].textColor_.b, textShadowAlpha_), answerFontSize_);
				drawText(rendered_[i][0], messageForDisplayRandomly_[i][0], displayTextOffset_ + i * displayTextSpace_, currentTb_[i].textColor_, answerFontSize_);
			}
		}
		else {
			for (int i = 0; i < numberOfDisplayText_; i++) {
				ci::gl::draw(rendered_[i][1], displayTextOffset_ + i * displayTextSpace_);
				ci::gl::enableAlphaBlending();
				ci::gl::draw(rendered_[i][0], displayTextOffset_ + i * displayTextSpace_);
				ci::gl::disableAlphaBlending();
			}
		}
		break;
	default:
		if (isFadeInState_) {
			for (int i = 0; i < numberOfDisplayText_; i++) {

				/*if (currentTb_[i].isMoveText_) {
				drawText(rendered_[i][0], messageForDisplayRandomly_[i][0], displayTextOffset_ + i * displayTextSpace_, currentTb_[i].textColor_, answerFontSize_);
				}
				else {
				ci::gl::color(0.2,0.2,0.2);
				ci::gl::draw(rendered_[i][0], displayTextOffset_ + i * displayTextSpace_ + currentTb_[i].posBias_);
				ci::gl::color(1,1,1);
				}*/

				/*
				Vec2f pos1 = displayTextOffset_ + i * displayTextSpace_ + currentTb_[i].posBias_;
				Vec2f pos2 = pos1 + Vec2f(rendered_[i][0].getWidth(), 0);

				if (rightBound_ - pos2.x > leftBound_ - pos1.x  && pos2.x < rightBound_ && pos1.x < leftBound_) {
				float tmp = leftBound_ - pos1.x;
				pos2.x = rightBound_ - tmp;
				}

				if (pos2.x < displayTextOffset_.x) {
				pos1 = displayTextOffset_;
				pos2 = pos1 + Vec2f(rendered_[i][0].getWidth(), 0);
				currentTb_[i].posBias_ = Vec2f(0,0);
				}
				ci::gl::draw(rendered_[i][0], pos1);
				if (pos2.x < rightBound_ && pos1.x < leftBound_) {
				ci::gl::draw(rendered_[i][0], pos2);
				}*/
				//drawTextFromVOS(rendered_[i][0], currentTb_[i].textSepWord_, displayTextOffset_ + i * displayTextSpace_ + currentTb_[i].posBias_, currentTb_[i].textColor_, answerFontSize_);
				//app::console() << rendered_[i][0].getWidth() << endl;
				drawText(rendered_[i][0], currentTb_[i].text_, displayTextOffset_ + i * displayTextSpace_, currentTb_[i].textColor_, answerFontSize_);
			}
			isFadeInState_ = false;
		}
		else {
			for (int i = 0; i < numberOfDisplayText_; i++) {

				Vec2f pos1 = displayTextOffset_ + i * displayTextSpace_ + currentTb_[i].posBias_;
				Vec2f pos2 = pos1 + Vec2f(rendered_[i][0].getWidth(), 0);

				if (rightBound_ - pos2.x > leftBound_ - pos1.x  && pos2.x < rightBound_ && pos1.x < leftBound_) {
					float tmp = leftBound_ - pos1.x;
					pos2.x = rightBound_ - tmp;
				}

				if (pos2.x < displayTextOffset_.x) {
					pos1 = displayTextOffset_ + i * displayTextSpace_;
					pos2 = pos1 + Vec2f(rendered_[i][0].getWidth(), 0);
					currentTb_[i].posBias_ = Vec2f(0,0);
				}
				ci::gl::draw(rendered_[i][0], pos1);
				if (pos2.x < rightBound_ && pos1.x < leftBound_) {
					ci::gl::draw(rendered_[i][0], pos2);
				}
			}
		}
		break;
	}

	ci::gl::color(0, 0, 0, 0);
	ci::gl::drawSolidRect(Rectf(0, displayTextOffset_.y, leftBound_, height_));
	ci::gl::drawSolidRect(Rectf(rightBound_, displayTextOffset_.y, width_, height_));
	ci::gl::color(1, 1, 1);

	sending();

	bg_.draw();
	ci::gl::draw(spoutTexture_, previewFrame_);

	for (int i = 0; i < listOfQuestion_.size(); i++) {
		listOfQuestion_[i].da_.setNormalizedPosition(Vec2f(100, 50 * (i + 1) + 300) / Vec2f(width_,height_));
		listOfQuestion_[i].da_.setNormalizedSize(checkMedia_.getDisplayArea().getNormalizedSize());
		listOfQuestion_[i].draw();

		ci::gl::draw(listOfQuestion_[i].rendered_, Vec2f(100, 50 * (i + 1) + 280) + listOfQuestion_[i].da_.getNormalizedSize() * Vec2f(width_, 0));
		//ci::gl::enableAlphaBlending();
		//ci::gl::drawString(listOfQuestion_[i].text_, Vec2f(100, 50 * (i + 1) + 290) + listOfQuestion_[i].da_.getNormalizedSize() * Vec2f(width_, 0), ColorA(1, 1, 1, 1), font_[30]);
		//ci::gl::disableAlphaBlending();
	}

	ci::gl::enableAlphaBlending();
	ci::gl::drawString("QUESTION", Vec2f(75, checkMedia_.getDisplayArea().getPosition().y) - Vec2f(0, 40), ColorA(1, 1, 1, 1), font_[40]);
	ci::gl::drawString("QUEUE : " + std::to_string((textButtons_.size())), checkMedia_.getDisplayArea().getPosition() - Vec2f(30, 40), ColorA(1, 1, 1, 1), font_[40]);
	ci::gl::drawString("                     COUNTDOWN " + std::to_string(int(nextMessageCounter_)), checkMedia_.getDisplayArea().getPosition() - Vec2f(30, 40), ColorA(1, 1, 1, 1), font_[40]);
	ci::gl::disableAlphaBlending();

	//ci::gl::drawString(std::to_string(textButtons_.size()), Vec2f(1500, 35), ColorA(0.3f, 0.3f, 0.7f, 1.0f), Font("Arial", 30));
	//ci::gl::drawString(std::to_string(state_), Vec2f(1500, 100), ColorA(0.3f, 0.3f, 0.7f, 1.0f), Font("Arial", 30));
	
	ci::gl::enableAlphaBlending();
	ci::gl::draw(currentTb_[0].rendered_, checkMedia_.getDisplayArea().getPosition() + checkMedia_.getDisplayArea().getNormalizedSize() * Vec2f(width_, height_ * 0.5));
	ci::gl::disableAlphaBlending();
	int c = 1;
	for (std::list<textButton>::iterator tb = (++textButtons_.begin()); tb != textButtons_.end() && c < maxShownAnswers_; ++tb) {
		ci::gl::enableAlphaBlending();
		ci::gl::draw(tb->rendered_, tb->da_.getPosition() + checkMedia_.getDisplayArea().getNormalizedSize() * Vec2f(width_, - height_ * 0.5));
		ci::gl::disableAlphaBlending();
		tb->draw();
		c++;
	}

	for (std::list<button>::iterator b = uiButtons_.begin(); b != uiButtons_.end(); ++b) {
		b->draw();
	}

	//ci::gl::drawString(std::to_string(changeQuestion_), Vec2f(1500, 100), ColorA(0.3f, 0.3f, 0.7f, 1.0f), Font("Arial", 300));

}

void mainPage::touchesEnded(ci::app::TouchEvent event) {
	pressing_ = false;
	for (std::list<textButton>::iterator tb = textButtons_.begin(); tb != textButtons_.end(); ++tb) {
		tb->reset();
	}
	for (std::list<button>::iterator b = uiButtons_.begin(); b != uiButtons_.end(); ++b) {
		b->reset();
	}
	for (int i = 0; i < listOfQuestion_.size(); i++) {
		listOfQuestion_[i].reset();
	}
	changeQuestion_ = true;
}

void mainPage::touchesBegan(ci::app::TouchEvent event) {
	float px = event.getTouches()[0].getPos().x / (width_ + 0.0f);
	float py = event.getTouches()[0].getPos().y / (height_ + 0.0f);

	for (std::list<textButton>::iterator tb = (++textButtons_.begin()); tb != textButtons_.end(); ++tb) {
		pressing_ = tb->isClickOrTouch(px, py);
		if (pressing_) break;
	}

	if (!pressing_) {
		for (std::list<button>::iterator b = uiButtons_.begin(); b != uiButtons_.end(); ++b) {
			pressing_ = b->isClickOrTouch(px, py);
			if (pressing_) break;
		}
	}

	if (!pressing_) {
		int j = 0;
		bool p = false;
		for (int i = 0; i < listOfQuestion_.size(); i++) {
			pressing_ = listOfQuestion_[i].isClickOrTouch(px, py);
			j = i;
			if (pressing_) {
				p = true;
				break;
			}
		}
		if (p) {
			for (int i = 0; i < listOfQuestion_.size(); i++) {
				if (i != j) {
					listOfQuestion_[i].reset();
					listOfQuestion_[i].state_ = 2;
				}
			}
		}
	}
}

void mainPage::mouseUp(ci::app::MouseEvent event) {
	pressing_ = false;
	for (std::list<textButton>::iterator tb = textButtons_.begin(); tb != textButtons_.end(); ++tb) {
		tb->reset();
	}
	for (std::list<button>::iterator b = uiButtons_.begin(); b != uiButtons_.end(); ++b) {
		b->reset();
	}
	for (int i = 0; i < listOfQuestion_.size(); i++) {
		listOfQuestion_[i].reset();
	}
	changeQuestion_ = true;
}

void mainPage::mouseDown(ci::app::MouseEvent event)
{
	float px = event.getPos().x / (width_ + 0.0f);
	float py = event.getPos().y / (height_ + 0.0f);

	for (std::list<textButton>::iterator tb = (++textButtons_.begin()); tb != textButtons_.end(); ++tb) {
		pressing_ = tb->isClickOrTouch(px, py);
		if (pressing_) break;
	}

	if (!pressing_) {
		for (std::list<button>::iterator b = uiButtons_.begin(); b != uiButtons_.end(); ++b) {
			pressing_ = b->isClickOrTouch(px, py);
			if (pressing_) break;
		}
	}

	if (!pressing_) {
		int j = 0;
		bool p = false;
		for (int i = 0; i < listOfQuestion_.size(); i++) {
			pressing_ = listOfQuestion_[i].isClickOrTouch(px, py);
			j = i;
			if (pressing_) {
				p = true;
				break;
			}
		}
		if (p) {
			for (int i = 0; i < listOfQuestion_.size(); i++) {
				if (i != j) {
					listOfQuestion_[i].reset();
					listOfQuestion_[i].state_ = 2;
				}
			}
		}
	}
}

void mainPage::mouseWheel(ci::app::MouseEvent event) {

}

void mainPage::mouseMove(ci::app::MouseEvent event) {

}