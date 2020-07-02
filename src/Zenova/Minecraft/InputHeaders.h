#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "Zenova/Minecraft/Inputs.h"

//Keymapping.h
class Keymapping {
public:
	std::string mKeyName;
	std::vector<int> mKeys;
	bool mCreateGui; //this bool is used to create the button in the input manager

	Keymapping(const std::string& name, int key) {
		mKeyName = name;
		mKeys = { key };
		mCreateGui = true;
	}

	Keymapping(const std::string& name, const Keybind& bind) {
		mKeyName = name;
		mKeys = bind.mKeys;
		mCreateGui = bind.mCreateGui;
	}

	bool isAltKey() {
		for (auto& key : mKeys) {
			if (key < 0) {
				return true;
			}
		}

		return false;
	}

	bool isAssigned() {
		return mKeys[0] != 0;
	}
};

//RemappingLayout.h
class RemappingLayout {
public:
	std::vector<Keymapping> mMapping;
	std::vector<Keymapping> mDefaultMapping;

	Keymapping getKeymappingByAction(const std::string& name) const {
		auto result = std::find_if(mMapping.begin(), mMapping.end(), 
								   [name](const Keymapping& keymap) { return name == keymap.mKeyName; });

		if (result != mMapping.end()) {
			return *result;
		}
		else {
			return Keymapping("", 0);
		}
	}

	virtual ~RemappingLayout();
	virtual void setMappingWithRawInput(const std::string&, int, enum class RawInputType);
	virtual int getAdjustedKey(int) const; 

	void assignDefaultMapping(std::vector<Keymapping>&&);
};

class GamePadRemappingLayout : public RemappingLayout {
	int mControllerType;
	//the size_t and char* is just a gsl::string_span :P
	size_t mUnkSize;
	char* mTexture;
	bool mABSwapped, mXYSwapped;
};

//Bindings.h
enum class FocusImpact : char {
	Neutral
};

class KeyboardKeyBinding {
std::string mButton;
int mKey;
FocusImpact mFocus;

public:
	KeyboardKeyBinding(const std::string& name, int key, FocusImpact focus) {
		mButton = name;
		mKey = key;
		mFocus = focus;
	}
};

class MouseButtonBinding {
	std::string mButton;
	int mKey;

public:
	MouseButtonBinding(const std::string& name, int key) {
		mButton = name;
		mKey = key;
	}
};

class GameControllerButtonBinding {
	std::string mButton;
	int mKey;
	float mActivator;
	bool mUnk;

public:
	GameControllerButtonBinding(const std::string& name, int key, bool unk = false, float activator = -1.0f) {
		mButton = name;
		mKey = key;
		mActivator = activator;
		mUnk = unk;
	}
};

class GameControllerTriggerBinding {
	std::string mButton;
	int mKey;
	float mActivator;

public:
	GameControllerTriggerBinding(const std::string& name, int key, float activator) {
		mButton = name;
		mKey = key;
		mActivator = activator;
	}

	GameControllerTriggerBinding(const std::string& name, int key) : GameControllerTriggerBinding(name, key, 0.5) {}
};

class GameControllerStickBinding {}; //literally never used
class GameControllerStickToButtonBinding {};

//Mappings.h
class KeyboardInputMapping {
public:
	std::vector<KeyboardKeyBinding> mBindings;
};

class MouseInputMapping {
public:
	std::vector<MouseButtonBinding> mBindings;
};


class GameControllerInputMapping {
public:
	std::vector<GameControllerButtonBinding> mButtonBindings;
	std::vector<GameControllerStickBinding> mStickBindings; //only including for when dealing with different archs
	std::vector<GameControllerTriggerBinding> mTriggerBindings;
	std::vector<GameControllerStickToButtonBinding> mStickToButtonBindings;
}; 

//MinecraftInputHandler.h
class IClientInstance {};

class InputHandler {
public:
	void registerButtonDownHandler(std::string, std::function<void(FocusImpact, IClientInstance&)>, bool);
	void registerButtonUpHandler(std::string, std::function<void(FocusImpact, IClientInstance&)>, bool);
};

class MinecraftInputHandler {
	uintptr_t** vtable;
public:
	std::unique_ptr<InputHandler> mInput;

	void _registerInputHandlers();
};

namespace ClientInputMappingFactory {
	enum class MappingControllerType : int {
		Xbox
	};
}

//VanillaClientInputMappingFactory.h
class VanillaClientInputMappingFactory {
	char buffer[0x90];
public:
	GamePadRemappingLayout mXboxLayout; //0x90
	GamePadRemappingLayout mGenericLayout; //0xE8
	std::shared_ptr<RemappingLayout> mLayout; //0x140

	void createKeyboardAndMouseBinding(KeyboardInputMapping& keyboard, MouseInputMapping& mouse, const std::string& buttonName, const std::string& keyName, FocusImpact impact = FocusImpact::Neutral) {
		Keymapping mapping = mLayout->getKeymappingByAction(keyName);

		for(int key : mapping.mKeys) {
			key = mLayout->getAdjustedKey(key);
			if(mapping.isAssigned()) {
				if(mapping.isAltKey())
					mouse.mBindings.emplace_back(buttonName, key);
				else
					keyboard.mBindings.emplace_back(buttonName, key, impact);
			}
		}
	}

	void createGamepadBinding(GameControllerInputMapping& gamepad, const GamePadRemappingLayout& gamepadLayout, const std::string& buttonName, const std::string& keyName, float activator = -1.0f) {
		Keymapping mapping = gamepadLayout.getKeymappingByAction(keyName);
		for(int key : mapping.mKeys) {
			key = gamepadLayout.getAdjustedKey(key);
			if(mapping.isAssigned()) {
				if(!mapping.isAltKey())
					gamepad.mButtonBindings.emplace_back(buttonName, key);

				if(activator <= 0.0f)
					gamepad.mTriggerBindings.emplace_back(buttonName, key);
				else
					gamepad.mTriggerBindings.emplace_back(buttonName, key, activator);
			}
		}
	}

	void _populateKeyboardDefaults(RemappingLayout&);
	void _populateGamepadDefaults(RemappingLayout&);

	void _addFullKeyboardGamePlayControls(KeyboardInputMapping&, MouseInputMapping&);
	void _addInvariantGamePlayGameControllerControls(GameControllerInputMapping&, ClientInputMappingFactory::MappingControllerType);
}; 

//ControlsSettingsScreenController.h
class ControlsSettingsScreenController {
public:
	void _handleDuplicates(RemappingLayout&);
};