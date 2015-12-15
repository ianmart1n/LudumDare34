#pragma once

#include <UIEvent.h>
#include <MY_ResourceManager.h>
#include <TextArea.h>
#include <Keyboard.h>
#include <scenario/Dialogue.h>

UIEvent::UIEvent(BulletWorld * _world, Shader * _textShader) :
VerticalLinearLayout(_world),
text(new TextArea(_world, MY_ResourceManager::scenario->getFont("HURLY-BURLY")->font, _textShader, 1.f)),
optionOne(new TextArea(_world, MY_ResourceManager::scenario->getFont("HURLY-BURLY")->font, _textShader, 300)),
optionTwo(new TextArea(_world, MY_ResourceManager::scenario->getFont("HURLY-BURLY")->font, _textShader, 300)),
buttonsLayout(new HorizontalLinearLayout(_world)),
currentEvent(nullptr),
readyForNewEvent(true)
{
	setRenderMode(kTEXTURE);

	//optionOne->renderMode = kTEXTURE;

	verticalAlignment = kTOP;
	horizontalAlignment = kCENTER;

	setBackgroundColour(1.f, 1.f, 1.f, 1.f);
	background->setVisible(false);


	setMargin(0.05f);
	setPadding(0.125f, 0.125f);
	setRationalWidth(1.f);
	setRationalHeight(1.f);

	addChild(text);
	addChild(buttonsLayout);
	buttonsLayout->addChild(optionOne);
	buttonsLayout->addChild(optionTwo);

	text->setWidth(600);
	text->setHeight(MY_ResourceManager::scenario->getFont("HURLY-BURLY")->font->getLineHeight() * 2);
	//text->setHeight(0.4f);
	text->setText(L"Bacon ipsum dolor amet anim occaecat pork loin.");
	text->verticalAlignment = kTOP;
	text->setWrapMode(kWORD);

	/*nextButton->eventManager.addEventListener("click", [this](sweet::Event * _event){
		if (!this->sayNext()){
			// prevent the user from clicking buttons while they're not visible
			nextButton->setMouseEnabled(false);
			optionOne->setMouseEnabled(false);
			optionTwo->setMouseEnabled(false);
			setVisible(false);
		}
	});
	optionOne->eventManager.addEventListener("click", [this](sweet::Event * _event){
		select(0);
	});
	optionTwo->eventManager.addEventListener("click", [this](sweet::Event * _event){
		select(1);
	});*/

	setVisible(false);
}

UIEvent::~UIEvent(){}

void UIEvent::update(Step * _step) {
	if (isVisible()){
	Keyboard& keyboard = Keyboard::getInstance();
	if (waitingForInput) {
		if (keyboard.keyJustDown(GLFW_KEY_LEFT)) {
			select(0);
		}

		if (keyboard.keyJustDown(GLFW_KEY_RIGHT)) {
			select(1);
		}
	}
	else {
		if (keyboard.keyJustDown(GLFW_KEY_RIGHT) || keyboard.keyJustDown(GLFW_KEY_LEFT)) {
			if (!this->sayNext()){
				end();
				setVisible(false);
				readyForNewEvent = true;
			}
		}
	}
	}

	VerticalLinearLayout::update(_step);

}

void UIEvent::startEvent(Scenario * _scenario){
	MY_ResourceManager::scenario->getAudio("EVENT_OPEN")->sound->play();
	currentEvent = _scenario;
	setVisible(true);

	currentConversation = _scenario->conversations["intro"];
	currentConversation->reset();
	sayNext();
	readyForNewEvent = false;
}

bool UIEvent::sayNext(){
	invalidateLayout();
	if (ConversationIterator::sayNext()){
		text->setText(currentConversation->getCurrentDialogue()->getCurrentText());

		if (waitingForInput){
			optionOne->setVisible(true);
			optionTwo->setVisible(true);

			optionOne->setText(currentConversation->options.at(0)->text);
			optionTwo->setText(currentConversation->options.at(1)->text);
		}
		else{
			optionOne->setVisible(false);
			optionTwo->setVisible(false);
		}

		return true;
	}
	else{
		return false;
	}
}