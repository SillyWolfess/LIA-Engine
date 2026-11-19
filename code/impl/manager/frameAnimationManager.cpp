#include "manager/frameAnimationManager.hpp"

#include "precompiled.hpp"

LIA::FrameAnimationData* LIA::FrameAnimationManager::get(std::string name) {
	LIA_TRY
		if (_data.find(name) == _data.end()) {
			return nullptr;
		}
		return &_data.at(name);
	LIA_CATCH(return nullptr;)
}