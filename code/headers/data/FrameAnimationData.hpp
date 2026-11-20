#ifndef LIA_FRAME_ANIMATION_DATA
#define LIA_FRAME_ANIMATION_DATA

#include <string>
#include <vector>

namespace LIA {
	struct FrameAnimationData {
		std::string name;
		int start;
		int end;
		std::vector<int> frame;
	};
}
#endif