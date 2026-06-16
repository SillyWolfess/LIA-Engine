#ifndef LIA_FPS_COUNTERS
#define LIA_FPS_COUNTERS
namespace LIA {
    class EventManager;
    class FpsCounter {
        public:
            void update(double, EventManager&);
            double getDelta() { return _deltaTime; };
            int getFps() { return _fps; };
        private:
            int _fps;
            int _nFrames;
            double _lastTime;
            double _deltaTime;
            double _lastFrameTime;
    };
}
#endif