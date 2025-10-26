#ifndef LIA_T_SIM_STATE
#define LIA_T_SIM_STATE
namespace LIA {
    enum class LIA_SIM_STATE {
        EMPTY,
        LOADING,
        LOADED,
        PAUSED,
        RUNNING,
        ENDED
    };
    enum class LIA_GAME_STATE {
        RUNNING,
        WON,
        LOST
    };
}
#endif