#pragma once

#include <ResourceManager.h>
#include <scenario\Scenario.h>

class MY_ResourceManager : public ResourceManager{
public:
	static Scenario * scenario;
	static Scenario * endScenario;
	static void init();

	static std::vector<Scenario*> events;
	static std::vector<bool> eventUsed;
};