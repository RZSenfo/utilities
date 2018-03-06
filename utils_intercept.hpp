#pragma once

#include "intercept.hpp"
#include <unordered_map>
#include <shared_mutex>

namespace utils {
	float distance2D(vector3& x, vector3 &y);

	vector3 get_pos(object &_target);


	vector3 get_rel_pos(const vector3 &_base, int dis, int dir);
    vector2 get_rel_pos(const vector2 &_base, int dis, int dir);

	vector3 get_random_pos(const vector3& _base, int dis);
	int get_rel_dir(const vector3 &_base, const vector3& _target);

	const float PI = 3.1415926535897f;

	template<typename T>
	inline T to_radians(T deg_) { return deg_ * (PI / 180); }

	template<typename T>
	inline T to_degrees(T rad_) { return rad_ * (180 / PI); }

	inline int normalize_degrees(int deg_) { return (deg_ + 360) % 360; }

    std::string deep_array_serialize(game_value& _array);

    void add_item_correctly(object & _container, std::string & _item, int _amount = 0);

    std::string load_parameterized_file(const std::string & _path, const std::vector<std::string>& _parameters);

    bool is_straight_road(const object& _road);
    
};


#define DLL_MAIN_LOGGING(LOG_NAME)                      \
std::shared_ptr<spdlog::logger> logging::logfile{};     \
void Init(void) {                                       \
    spdlog::set_level(spdlog::level::debug);            \
    logging::logfile = spdlog::rotating_logger_mt(      \
        "logfile",                                      \
        "logs/" + std::string(                          \
            LOG_NAME                                    \
        ) + "_dll.log",                                 \
        1024000,                                        \
        3                                               \
    );                                                  \
                                                        \
    logging::logfile->flush_on(spdlog::level::debug);   \
    spdlog::set_pattern("[%H:%M:%S]-{%l}- %v");         \
                                                        \
    LOG(INFO, "Plugin DLL Loaded");                     \
}                                                       \
                                                        \
void Cleanup(void) {                                    \
    logging::logfile->flush();                          \
    spdlog::drop_all();                                 \
}                                                       \


#ifdef _MSC_VER 

#define DLL_MAIN_ENTRY                                  \
BOOL APIENTRY DllMain(HMODULE hModule,                  \
    DWORD  ul_reason_for_call,                          \
    LPVOID lpReserved                                   \
)                                                       \
{                                                       \
    switch (ul_reason_for_call)                         \
    {                                                   \
    case DLL_PROCESS_ATTACH:                            \
        Init();                                         \
        break;                                          \
    case DLL_THREAD_ATTACH:                             \
    case DLL_THREAD_DETACH:                             \
    case DLL_PROCESS_DETACH:                            \
        Cleanup();                                      \
        break;                                          \
    }                                                   \
    return TRUE;                                        \
};

#else

#define DLL_MAIN_ENTRY                                  \
static void __attribute__((constructor)) so_attach(void) { \
    Init();                                             \
}                                                       \
                                                        \
static void __attribute__((destructor)) so_detach(void) { \
    Cleanup();                                          \
}

#endif