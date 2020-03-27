#include "Zenova/Mod.h"

namespace Zenova {
	Mod::Mod(const char* t_name, Version t_version, const char* t_description) : name(t_name), version(t_version), description(t_description) {
		//Send a message? /shrug
	}
	
	Mod::~Mod() {
		//Send a message? /shrug
	}
}