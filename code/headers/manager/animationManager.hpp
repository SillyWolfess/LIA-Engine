#ifndef LIA_ANIMATION_MANAGER
#define LIA_ANIMATION_MANAGER

#include <vector>
#include <string>
#include <map>

#include "loaders/xmlLoader.hpp"

#include "./../data/AnimationDefinition.hpp"

namespace LIA {
	class AnimationManager {
		private:
            std::map<std::string, XmlLoader::XmlData> _cache;
            std::map<std::string, XmlLoader::XmlData> _animDefCache;
            std::map<std::string, XmlLoader::XmlData> _pathCache;
            std::map<std::string, std::string> _paths;
            std::map<std::string, std::string> _templatePaths;

            std::vector<AnimationDefinition> _animDefinitions;

            std::string getPath(std::string);
            std::string getTemplatePath(std::string);

            int create();
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

            int create(std::string);
            AnimationDefinition* get(int);
	};
}
#endif