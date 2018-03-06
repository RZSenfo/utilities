#pragma once

#include "intercept.hpp"
#include "singleton.hpp"

struct location_information {
    std::string type;
    std::string readable_name;
    
    enum importance_grade {
        NAME,
        TASK,
        NATUR,
        MARINE,
        LOCAL,
        VILLAGE,
        CITY,
        CITYCAPITAL
    };
    importance_grade importance;

    location_information() { importance = importance_grade::NAME; }
    location_information(std::string _type, std::string _readable_name, int _importance) {
        type = _type;
        readable_name = _readable_name;
        if (_importance >= 7)       importance = importance_grade::CITYCAPITAL;
        else if (_importance == 6)  importance = importance_grade::CITY;
        else if(_importance == 5)   importance = importance_grade::VILLAGE;
        else if(_importance == 4)   importance = importance_grade::LOCAL;
        else if(_importance == 3)   importance = importance_grade::MARINE;
        else if(_importance == 2)   importance = importance_grade::NATUR;
        else if(_importance == 1)   importance = importance_grade::TASK;
        else                        importance = importance_grade::NAME;
    }
};

struct location_detail {
    vector3 pos;
    std::string name;
    std::string type;
    vector2 size;

    location_detail() {}
    location_detail(vector3 _pos, std::string _name, std::string _type, vector2 _size)
        : pos(_pos), name(_name), type(_type), size(_size) {

    }
};

class sqf_values : public singleton<sqf_values> {
    friend class singleton<sqf_values>;
private:
    sqf_values() {

    }
    ~sqf_values() {

    }
public:
    //const variables set on game start
    side independent;
    side east;
    side west;
    side civilian;
    config config_file;
    intercept::sqf::config_entry config_file_entry;
    config mission_config_file;
    intercept::sqf::config_entry mission_config_file_entry;
    intercept::sqf::config_entry cfg_vehicles;
    intercept::sqf::config_entry cfg_magazines;
    intercept::sqf::config_entry cfg_weapons;
    intercept::sqf::config_entry cfg_ammo;
    rv_namespace mission_name_space;
    rv_namespace profile_name_space;
    rv_namespace ui_name_space;
    float world_size;
    std::string world_name;

    bool dedicated;
    bool has_interface;

    int client_owner;

    std::unordered_map<std::string, location_information> location_types;
    std::unordered_map<location_information::importance_grade, std::vector<location> > locations_by_importance;
    std::unordered_map<location, std::pair<std::string, location_detail> > locations;

    void pre_start() {
        //fetch some static values
        west = intercept::sqf::west();
        east = intercept::sqf::east();
        civilian = intercept::sqf::civilian();
        independent = intercept::sqf::independent();
        config_file = intercept::sqf::config_file();
        config_file_entry = intercept::sqf::config_entry(config_file);
        cfg_vehicles = config_file_entry >> "CfgVehicles";
        cfg_magazines = config_file_entry >> "CfgMagazines";
        cfg_ammo = config_file_entry >> "CfgAmmo";
        cfg_weapons = config_file_entry >> "CfgWeapons";
        mission_config_file = intercept::sqf::mission_config_file();
        mission_config_file_entry = intercept::sqf::config_entry(mission_config_file);
        mission_name_space = intercept::sqf::mission_namespace();
        profile_name_space = intercept::sqf::profile_namespace();
        ui_name_space = intercept::sqf::ui_namespace();
        world_size = intercept::sqf::world_size();

        dedicated = intercept::sqf::is_dedicated();
        has_interface = intercept::sqf::has_interface();

        client_owner = static_cast<int>(intercept::sqf::client_owner());

        
    }

    void pre_init() {
        //read the world config
    
        mission_config_file = intercept::sqf::mission_config_file();
        mission_config_file_entry = intercept::sqf::config_entry(mission_config_file);
        mission_name_space = intercept::sqf::mission_namespace();
        profile_name_space = intercept::sqf::profile_namespace();
        world_size = intercept::sqf::world_size();
        world_name = intercept::sqf::world_name();

        for (int _i = 0; _i < 8; _i++) {
            locations_by_importance[(location_information::importance_grade) _i] = { };
        }
        
        auto _ws_half = world_size / 2.f;

        auto _loc_classes = intercept::sqf::config_classes("true", config_file_entry >> "CfgLocationTypes");
        for (intercept::sqf::config_entry _x : _loc_classes) {
            
            auto _cfg_name = static_cast<std::string>(intercept::sqf::config_name(_x));
            auto _readable = static_cast<std::string>(intercept::sqf::get_text(_x >> "name"));
            int _importance = static_cast<int>(intercept::sqf::get_number(_x >> "importance"));

            location_information _info = location_information(_cfg_name, _readable, _importance);

            location_types[_cfg_name] = _info;
            

            std::vector<r_string> _loc_types;
            _loc_types.emplace_back(r_string(_cfg_name));
            std::vector<location> _locs = intercept::sqf::nearest_locations(vector3({ _ws_half, _ws_half, 0.f }), _loc_types, world_size);
            for (auto &_loc : _locs) {
                locations[_loc] = std::pair<std::string,location_detail>(
                    _cfg_name,
                    location_detail(
                        intercept::sqf::position(_loc),
                        static_cast<std::string>(intercept::sqf::name(_loc)),
                        static_cast<std::string>(intercept::sqf::type(_loc)),
                        intercept::sqf::size(_loc)
                    )
                );
                locations_by_importance[_info.importance].emplace_back(_loc);
            }
        }
    }
};