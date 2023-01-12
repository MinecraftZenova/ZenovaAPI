#include "Zenova/Minecraft/Inputs.h"
#include "Zenova/Minecraft/InputHeaders.h"

namespace Zenova {
	static void (*__addFullKeyboardGamePlayControls)(VanillaClientInputMappingFactory*, KeyboardInputMapping&, MouseInputMapping&);
	void _addFullKeyboardGamePlayControls(VanillaClientInputMappingFactory* self, KeyboardInputMapping& keyboard, MouseInputMapping& mouse) {
		__addFullKeyboardGamePlayControls(self, keyboard, mouse);

		for (auto& input : InputManager::getInputs()) {
			self->createKeyboardAndMouseBinding(keyboard, mouse, input.buttonName, input.keyName);
		}
	}

	static void (*__addInvariantGamePlayGameControllerControls)(VanillaClientInputMappingFactory*, GameControllerInputMapping&, ClientInputMappingFactory::MappingControllerType);
	void _addInvariantGamePlayGameControllerControls(VanillaClientInputMappingFactory* self, GameControllerInputMapping& gamepad, ClientInputMappingFactory::MappingControllerType controllerType) {
		__addInvariantGamePlayGameControllerControls(self, gamepad, controllerType);

		auto& layout = (controllerType == ClientInputMappingFactory::MappingControllerType::Xbox) 
			? self->mXboxLayout : self->mGenericLayout;
		for (auto& input : InputManager::getInputs()) {
			self->createGamepadBinding(gamepad, layout, input.buttonName, input.keyName);
		}
	}

	enum class RemappingType {
		None,
		Keyboard,
		Gamepad
	};

	inline RemappingType mapType;

	void mapBind(std::vector<Keymapping>& mapping, const std::string& name, const Keybind& bind) {
		if (bind.hasKeys()) {
			mapping.emplace_back(name, bind.mKeys, bind.mCreateGui);
		}
	}

	static void (*_assignDefaultMapping)(RemappingLayout*, std::vector<Keymapping>&&);
	void assignDefaultMapping(RemappingLayout* self, std::vector<Keymapping>&& mapping) {

		switch (mapType) {
			case RemappingType::Keyboard:
				for (auto& data : InputManager::getInputs()) {
					mapBind(mapping, data.keyName, data.input.mKeyboard);
				}
				break;

			case RemappingType::Gamepad:
				for (auto& data : InputManager::getInputs()) {
					mapBind(mapping, data.keyName, data.input.mGamepad);
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

		for (auto& data : InputManager::getInputs()) {
			self->mInput->registerButtonDownHandler(data.buttonName,
				[callback = data.input.mCallback](FocusImpact, IClientInstance& client) {
				if (callback)
					callback(true, client);
			}, false);
			self->mInput->registerButtonUpHandler(data.buttonName,
				[callback = data.input.mCallback](FocusImpact, IClientInstance& client) {
				if (callback)
					callback(false, client);
			}, false);
		}
	}

	static void (*__handleDuplicates)(ControlsSettingsScreenController*, RemappingLayout&);
	void _handleDuplicates(ControlsSettingsScreenController* self, RemappingLayout& a2) {}

	inline void createInputHooks() {
		Zenova_Hook(MinecraftInputHandler::_registerInputHandlers, &_registerInputHandlers, &__registerInputHandlers);

		Zenova_Hook(RemappingLayout::assignDefaultMapping, &assignDefaultMapping, &_assignDefaultMapping);

		Zenova_Hook(VanillaClientInputMappingFactory::_populateKeyboardDefaults, &_populateKeyboardDefaults, &__populateKeyboardDefaults);
		Zenova_Hook(VanillaClientInputMappingFactory::_populateGamepadDefaults, &_populateGamepadDefaults, &__populateGamepadDefaults);
		Zenova_Hook(VanillaClientInputMappingFactory::_addFullKeyboardGamePlayControls, &_addFullKeyboardGamePlayControls, &__addFullKeyboardGamePlayControls);
		Zenova_Hook(VanillaClientInputMappingFactory::_addInvariantGamePlayGameControllerControls, &_addInvariantGamePlayGameControllerControls, &__addInvariantGamePlayGameControllerControls);

		Zenova_Hook(ControlsSettingsScreenController::_handleDuplicates, &_handleDuplicates, &__handleDuplicates);
	}
}