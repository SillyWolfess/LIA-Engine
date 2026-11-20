#ifndef LIA_FRAME_ANIMATION_MANAGER
#define LIA_FRAME_ANIMATION_MANAGER

#include <string>
#include <map>

#include "./../data/FrameAnimationData.hpp"
#include "manager/modelManager.hpp"
#include "data/position.hpp"
#include "graphics/scene.hpp"
#include "data/object.hpp"

namespace LIA {
	class FrameAnimationManager {
		private:
			std::map<std::string, FrameAnimationData> _data;
			ModelManager _modelManager;
		public:
			FrameAnimationData* get(std::string);
			void pass(Scene*, Object*, int, std::string, Position&, Rotation&, Scale&);
	};
}

#endif