#ifndef LIA_FRAME_ANIMATION_MANAGER
#define LIA_FRAME_ANIMATION_MANAGER

#include <string>
#include <map>

#include "./../data/FrameAnimationData.hpp"

namespace LIA {
	class FrameAnimationManager {
		private:
			std::map<std::string, FrameAnimationData> _data;
		
		public:
			FrameAnimationData* get(std::string);
	};
}

#endif