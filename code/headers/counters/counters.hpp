#ifndef LIA_COUNTERS
#define LIA_COUNTERS

#include <map>
#include <string>

#define LIA_DEBUG_ON 0

#if LIA_DEBUG_ON
    #define LIA_DEBUG_CONSTRUCTOR_NO_LOG(x) LIA_DEBUG::Counters::constructor(x, false)
    #define LIA_DEBUG_CONSTRUCTOR_COPY_NO_LOG(x) LIA_DEBUG::Counters::constructorCopy(x, false)
    #define LIA_DEBUG_DESTRUCTOR_NO_LOG(x) LIA_DEBUG::Counters::destructor(x, false)
    #define LIA_DEBUG_CONSTRUCTOR(x) LIA_DEBUG::Counters::constructor(x, true)
    #define LIA_DEBUG_CONSTRUCTOR_COPY(x) LIA_DEBUG::Counters::constructorCopy(x, true)
    #define LIA_DEBUG_DESTRUCTOR(x) LIA_DEBUG::Counters::destructor(x, true)
    #define LIA_DEBUG_MAX_COUNT(x, y) LIA_DEBUG::Counters::maxCounter(x, y)
    #define LIA_DEBUG_LOG LIA_DEBUG::Counters::log();
#else
    #define LIA_DEBUG_CONSTRUCTOR_NO_LOG(x)
    #define LIA_DEBUG_DESTRUCTOR_NO_LOG(x)
    #define LIA_DEBUG_CONSTRUCTOR_COPY_NO_LOG(x)
    #define LIA_DEBUG_CONSTRUCTOR(x)
    #define LIA_DEBUG_CONSTRUCTOR_COPY(x)
    #define LIA_DEBUG_DESTRUCTOR(x)
    #define LIA_DEBUG_MAX_COUNT(x, y)
    #define LIA_DEBUG_LOG
#endif

namespace LIA_DEBUG {
    static class Counters {
        private:
            static std::map<std::string, int> _constructorCalls;
            static std::map<std::string, int> _constructorCopyCalls;
            static std::map<std::string, int> _destructorCalls;
            static std::map<std::string, int> _diffCalls;
            static std::map<std::string, int> _maxCount;
            static void ensureExists(std::string);
        public:
            static void constructor(std::string, bool);
            static void constructorCopy(std::string, bool);
            static void destructor(std::string, bool);
            static void maxCounter(std::string, int);
            static void log();
    };
}
#endif