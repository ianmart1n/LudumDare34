#pragma once

#include <sweet/UI.h>
#include <scenario/Conversation.h>

class UIEvent : public VerticalLinearLayout, public ConversationIterator{
public:
	TextArea * text;
	TextArea * optionOne;
	TextArea * optionTwo;
	HorizontalLinearLayout * buttonsLayout;

	UIEvent(BulletWorld * _world, Shader * _textShader);
	~UIEvent();

	Scenario * currentEvent;
	virtual void update(Step * _step) override;
	void startEvent(Scenario * _event);

	virtual bool sayNext() override;
};