#ifndef LIA_ANIMATION_MANAGER
#define LIA_ANIMATION_MANAGER

#include <vector>
#include <string>
#include <map>

#include "loaders/xmlLoader.hpp"
#include "frameAnimationManager.hpp"

#include "./../data/AnimationDefinition.hpp"
#include "graphics/scene.hpp"
#include "data/position.hpp"
#include "data/object.hpp"

namespace LIA {
	class AnimationManager {
		private:
            std::map<std::string, XmlLoader::XmlData> _cache;
            std::map<std::string, XmlLoader::XmlData> _animDefCache;
            std::map<std::string, XmlLoader::XmlData> _pathCache;
            std::map<std::string, std::string> _paths;
            std::map<std::string, std::string> _templatePaths;

            std::map<std::string, AnimationDefinition> _animDefinitions;

            FrameAnimationManager _frameAnimationManager;

            std::string getPath(std::string);
            std::string getTemplatePath(std::string);

            bool _create(std::string);
            bool find(std::string);
            AnimationType resolveType(std::string);
		public:
            void reset();
            /***
            * @param {string} name/id
            * @param {string} path
            */
            bool registerPath(std::string, std::string);
            /***
            * @param {string} name/id
            * @param {string} path
            */
            bool registerTemplatePath(std::string, std::string);
            bool registerPathsFromFile(std::string);
            bool registerTemplatePathsFromFile(std::string);

            bool create(std::string);
            AnimationDefinition* get(std::string);

            void pass(Scene*, Object* object, AnimationInfo&, Position& position, Rotation& rotation, Scale& scale);
            void nextFrame(AnimationInfo&);
	};
}
#endif