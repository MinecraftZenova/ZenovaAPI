#include "Zenova/Minecraft/Inputs.h"
#include "Zenova/Minecraft/InputHeaders.h"

namespace Zenova {
	static void (*__addFullKeyboardGamePlayControls)(VanillaClientInputMappingFactory*, KeyboardInputMapping&, MouseInputMapping&);
	void _addFullKeyboardGamePlayControls(VanillaClientInputMappingFactory* self, KeyboardInputMapping& keyboard, MouseInputMapping& mouse) {
		__addFullKeyboardGamePlayControls(self, keyboard, mouse);

		for (auto& [name, _] : InputManager::getInputs()) {
			self->createKeyboardAndMouseBinding(keyboard, mouse, "button." + name, "key." + name);
		}
	}

	static void (*__addInvariantGamePlayGameControllerControls)(VanillaClientInputMappingFactory*, GameControllerInputMapping&, ClientInputMappingFactory::MappingControllerType);
	void _addInvariantGamePlayGameControllerControls(VanillaClientInputMappingFactory* self, GameControllerInputMapping& gamepad, ClientInputMappingFactory::MappingControllerType controllerType) {
		__addInvariantGamePlayGameControllerControls(self, gamepad, controllerType);

		switch (controllerType) {
			case ClientInputMappingFactory::MappingControllerType::Xbox:
				for (auto& [name, _] : InputManager::getInputs()) {
					self->createGamepadBinding(gamepad, self->mXboxLayout, "button." + name, "key." + name);
				}
				break;
			default:
				for (auto& [name, _] : InputManager::getInputs()) {
					self->createGamepadBinding(gamepad, self->mGenericLayout, "button." + name, "key." + name);
				}
				break;
		}
	}

	enum class RemappingType {
		None,
		Keyboard,
		Gamepad
	};

	inline RemappingType mapType;

	static void (*_assignDefaultMapping)(RemappingLayout*, std::vector<Keymapping>&&);
	void assignDefaultMapping(RemappingLayout* self, std::vector<Keymapping>&& mapping) {
		switch (mapType) {
			case RemappingType::Keyboard:
				for (auto& [name, input] : InputManager::getInputs()) {
					if (input.mKeyboard)
						mapping.emplace_back("key." + name, input.mKeyboard);
				}
				break;

			case RemappingType::Gamepad:
				for (auto& [name, input] : InputManager::getInputs()) {
					if(input.mGamepad)
						mapping.emplace_back("key." + name, input.mGamepad);
				}
				break;

			default: break;
		}

		_assignDefaultMapping(self, std::move(mapping));
	}

	static void (*__populateKeyboardDefaults)(VanillaClientInputMappingFactory*, RemappingLayout&);
	void _populateKeyboardDefaults(VanillaClientInputMappingFactory* self, RemappingLayout& a1) {
		mapType = RemappingType::Keyboard;
		__populateKeyboardDefaults(self, a1);
		mapType = RemappingType::None;
	}

	static void (*__populateGamepadDefaults)(VanillaClientInputMappingFactory*, RemappingLayout&);
	void _populateGamepadDefaults(VanillaClientInputMappingFactory* self, RemappingLayout& a1) {
		mapType = RemappingType::Gamepad;
		__populateGamepadDefaults(self, a1);
		mapType = RemappingType::None;
	}

	static void (*__registerInputHandlers)(MinecraftInputHandler*);
	void _registerInputHandlers(MinecraftInputHandler* self) {
		__registerInputHandlers(self);

		for (auto& [name, input] : InputManager::getInputs()) {
			self->mInput->registerButtonDownHandler("button." + name, 
													[callback = input.mCallback](FocusImpact, IClientInstance&) {
														if (callback) 
															callback(true);
													}, false);
			self->mInput->registerButtonUpHandler("button." + name,
												  [callback = input.mCallback](FocusImpact, IClientInstance&) {
													  if (callback) 
														  callback(false);
												  }, false);
		}
	}

	static void (*__handleDuplicates)(ControlsSettingsScreenController*, RemappingLayout&);
	void _handleDuplicates(ControlsSettingsScreenController* self, RemappingLayout& a2) {}

	inline void createInputHooks() {
		Hook::Create(&MinecraftInputHandler::_registerInputHandlers, &_registerInputHandlers, &__registerInputHandlers);

		Hook::Create(&RemappingLayout::assignDefaultMapping, &assignDefaultMapping, &_assignDefaultMapping);

		Hook::Create(&VanillaClientInputMappingFactory::_populateKeyboardDefaults, &_populateKeyboardDefaults, &__populateKeyboardDefaults);
		Hook::Create(&VanillaClientInputMappingFactory::_populateGamepadDefaults, &_populateGamepadDefaults, &__populateGamepadDefaults);
		Hook::Create(&VanillaClientInputMappingFactory::_addFullKeyboardGamePlayControls, &_addFullKeyboardGamePlayControls, &__addFullKeyboardGamePlayControls);
		Hook::Create(&VanillaClientInputMappingFactory::_addInvariantGamePlayGameControllerControls, &_addInvariantGamePlayGameControllerControls, &__addInvariantGamePlayGameControllerControls);

		Hook::Create(&ControlsSettingsScreenController::_handleDuplicates, &_handleDuplicates, &__handleDuplicates);
	}
}