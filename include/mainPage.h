#pragma once

#include <cinder/app/AppBasic.h>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <ctime>

#include "BitTime.h"

#include "Configurable.h"
#include "customRect.h"
#include "led.h"
#include "WebControl.h"
#include "Spout.h"

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/Unicode.h"

#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
#include "cinder/Font.h"
#include "Resources.h"

#include "Log.h"

#include "button.h"
#include "textButton.h"
#include <fstream>

using namespace ci;
using namespace ci::app;
using namespace std;

enum states{IDLE, FADE_IN, DRAW_IDLE, FADE_OUT, NUM_STATE};

class mainPage : public Configurable
{
public:

	void readConfig(Bit::JsonTree* tree);
	void readParams(Bit::JsonTree* tree, Bit::ParamsRef params);

	void resetPage();
	void setup();
	void update();
	void draw();

	void clearList();

	void shutdown();

	void computeQuestionTexture();

	void mouseDown(ci::app::MouseEvent event);
	void mouseUp(ci::app::MouseEvent event);
	void mouseWheel(ci::app::MouseEvent event);
	void mouseMove(ci::app::MouseEvent event);

	void touchesBegan(ci::app::TouchEvent event);
	void touchesEnded(ci::app::TouchEvent event);

	void drawText(Surface8u &rend, const string &s2t, const Vec2f &pos, const Color &c, int fontSize);
	void drawTextAdditive(Surface8u &rend, const string &s2t, const Vec2f &pos, const ColorA &c, int fontSize);
	void drawText(Surface8u &rend, const string &s2t, const Vec2f &pos, const Color &c, const Color &clearColor, int fontSize);

	void drawTextFromVOS(Surface8u &rend, const vector<string> &vos, const Vec2f &pos, const Color &c, int fontSize);

	void addTextButton(string s, vector<string> vs, Color c, string logText);
	void addQuestion(string s);

	void initRendered(int times);

	void readFile();
	void printCharSurfaceSize(const vector<string> &a, ofstream &myfile);
	void sending();
	void receiveSomeThing();
	string textRandomization(textButton &tbRef);
	string textRandomization2();

private:

	const std::string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";


	static inline bool is_base64(unsigned char c) {
		return (isalnum(c) || (c == '+') || (c == '/'));
	}

	std::string base64_decode(std::string const& encoded_string) {
		int in_len = encoded_string.size();
		int i = 0;
		int j = 0;
		int in_ = 0;
		unsigned char char_array_4[4], char_array_3[3];
		std::string ret;

		while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
			char_array_4[i++] = encoded_string[in_]; in_++;
			if (i == 4) {
				for (i = 0; i <4; i++)
					char_array_4[i] = base64_chars.find(char_array_4[i]);

				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

				for (i = 0; (i < 3); i++)
					ret += char_array_3[i];
				i = 0;
			}
		}

		if (i) {
			for (j = i; j <4; j++)
				char_array_4[j] = 0;

			for (j = 0; j <4; j++)
				char_array_4[j] = base64_chars.find(char_array_4[j]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
		}

		return ret;
	}

	string testString_;

	customRect mainRectArea_;

	Bit::WebControl webControlClient_;

	button approveButton_;

	string fontName_;
	string questionPath_;

	vector<vector<string>> messageForDisplayRandomly_;

	SpoutSender spoutsender_;
	bool bInitialized_;							// true if a sender initializes OK
	bool bMemoryMode_;
	bool receiveSomeThing_;// tells us if texture share compatible
	bool changeQuestion_;
	char SenderName_[256];				// sender name 
	bool isFadeInState_;

	ci::gl::Texture spoutTexture_;
	vector<vector<Surface8u>> rendered_;
	Surface8u questionRendered_;
	vector<Font> font_;

	int state_;
	int messagesTrueSize_;
	int tr2Counter_;

	int width_;
	int height_;
	int numberOfDisplayText_;

	int spaceBetweenFrontAndBack_;
	int questionFontSize_;
	int answerFontSize_;
	int maxTextInLine_;
	int maxShownAnswers_;

	bool pressing_;

	float changingProb_;
	float answerQueueStride_;
	float textMoveDuration_;
	float textMoveSpeed_;
	float textShadowAlpha_;
	int rightBound_;
	int leftBound_;

	double transitionDuration_;
	double cTime1_;
	double diff_;
	double timeBeetweenRandomization_;
	double tbrCount_;

	double nextMessageCountDown_;
	double nextMessageCounter_;

	Vec2f displayTextOffset_;
	Vec2f displayTextSpace_;
	Vec2f questionPosition_;

	vector<string> engUpper_;
	vector<string> engLower_;

	vector<string> thaiNormal_;
	vector<string> thaiUpper_;
	vector<string> thaiLower_;

	vector<textButton> currentTb_;
	vector<textButton> listOfQuestion_;

	std::list<textButton> textButtons_;
	std::list<button> uiButtons_;

	Multimedia bg_;

	Multimedia checkMedia_;
	Multimedia noCheckMedia_;

	Multimedia refresh_;
	Multimedia noRefresh_;

	Multimedia showAllAnswer_;
	Multimedia noshowAllAnswer_;
	
	Multimedia skip_;
	Multimedia noSkip_;

	Multimedia clear_;
	Multimedia noClear_;

	Log logAccept_;
	Log logReject_;

	Rectf previewFrame_;

};