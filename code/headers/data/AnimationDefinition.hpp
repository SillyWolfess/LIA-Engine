#ifndef LIA_ANIMATION_DEFINITION
#define LIA_ANIMATION_DEFINITION

#include <string>

namespace LIA {
	enum class AnimationType {NONE, FRAMES};
	struct AnimationDefinition {
		std::string name;
		std::string path;

		AnimationType type;
		
		bool loaded;
		bool repeat;
	};
}
#endif