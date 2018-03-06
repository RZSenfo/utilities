#include "utils_intercept.hpp"
#include "utils.hpp"

#include <cmath>
#include <iomanip> // setprecision
#include <sstream> // stringstream


float utils::distance2D(vector3& x, vector3 &y) { 
	vector3 _dist = x - y; 
	_dist = _dist * _dist; 
	return std::sqrt(_dist.x + _dist.y);
};

vector3 utils::get_pos(object &_target) {
	vector3 _ret;
#ifdef __use_matrix__ 
	_ret = ((game_data_object*)(_target.data.getRef()))->get_position_matrix()._position;
#else
	{
		intercept::client::invoker_lock lock;
		_ret = intercept::sqf::get_pos_world(_target);
	}
#endif
	return _ret;
}

vector3 utils::get_rel_pos(const vector3& _base, int distance, int dir) {
	return vector3({
		_base.x + distance * static_cast<float>(std::sin(to_radians(static_cast<float>(dir)))),
		_base.y + distance * static_cast<float>(std::cos(to_radians(static_cast<float>(dir)))),
		_base.z });
}

vector2 utils::get_rel_pos(const vector2& _base, int distance, int dir) {
    return vector2({
        _base.x + distance * static_cast<float>(std::sin(to_radians(static_cast<float>(dir)))),
        _base.y + distance * static_cast<float>(std::cos(to_radians(static_cast<float>(dir))))
    });
}

vector3 utils::get_random_pos(const vector3& _base, int distance) {
	float _dir = static_cast<float>(utils::random(360));
	distance = utils::random(distance);
	return vector3({
		_base.x + distance * static_cast<float>(std::sin(to_radians(_dir))),
		_base.y + distance * static_cast<float>(std::cos(to_radians(_dir))),
		_base.z });
}

int utils::get_rel_dir(const vector3& pos1_, const vector3& pos2_) {
	vector3 pos_diff{ pos2_ - pos1_ };
	
	float deg = to_degrees(std::atan2(pos_diff.y, pos_diff.x));
	deg += 90 - 180;
	deg = std::abs(deg);

	return static_cast<int>(deg);
}

std::string utils::deep_array_serialize(game_value& _array) {
    std::string _result = "[";

    for (size_t _k = 0; _k < _array.size(); _k++) {
        game_value _arg = _array[_k];

        if (_arg.type() == intercept::types::game_data_string::type_def) {
            _result += "\"" + static_cast<std::string>(_arg) + "\"";
        }
        else if (_arg.type() == intercept::types::game_data_number::type_def) {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(32) << static_cast<float>(_arg);
            _result += ss.str();
        }
        else if (_arg.type() == intercept::types::game_data_bool::type_def) {
            _result += _arg ? "true" : "false";
        }
        else if (_arg.type() == intercept::types::game_data_array::type_def) {
            _result += utils::deep_array_serialize(_arg);
        }
        else {
            //DEBUG_LOG("<DB>: ERROR: Unsupported type in params");
            _result += "<any>"; //escapes nil?
        }

        _result += _k < (_array.size() - 1) ? "," : "]";
    }
    return _result;
}

void utils::add_item_correctly(object& _container, std::string &_item, int _amount) {
    float _type = 0;

    /*
    case "Magazine": _type2 = 1;
    case "Item": _type2 = 0;
    case "Weapon":_type2 = 2;
    case "Equipment":
    if ((_itemType select 1) isEqualTo "Backpack" ) then
    _type2 = 3;
    else
    _type2 = 0;
    case "Mine":_type2 = 1;
    */
    intercept::client::invoker_lock lock;

    if (intercept::sqf::is_class(intercept::sqf::config_entry(intercept::sqf::config_file()) >> "cfgweapons" >> _item)) {
        config _cfgItem = intercept::sqf::config_entry(intercept::sqf::config_file()) >> "cfgweapons" >> _item;
        std::string _simulation = static_cast<std::string>(intercept::sqf::get_text(intercept::sqf::config_entry(_cfgItem) >> "simulation"));
        
        //_itemCategory = "Item";
        if (utils::iequals(_simulation, "weapon")) {
            //_itemCategory = "Weapon";
            int _type2;
            if (intercept::sqf::is_text(intercept::sqf::config_entry(_cfgItem) >> "type")) {
                _type2 = std::stoi(static_cast<std::string>(intercept::sqf::get_text(intercept::sqf::config_entry(_cfgItem) >> "type")));
            }
            else {
                _type2 = static_cast<int>(intercept::sqf::get_number(intercept::sqf::config_entry(_cfgItem) >> "type"));
            }
            
            if (_type2 == 4096 || _type2 == 131072) {
                //_itemCategory = "Item";
                //			"LaserDesignator" or item
                intercept::sqf::add_item_cargo_global(_container, _item, _amount);
            }
            else {
                intercept::sqf::add_weapon_cargo_global(_container, _item, _amount);
            }
        }
        else {
            intercept::sqf::add_item_cargo_global(_container, _item, _amount);
        }
    }
    else if (intercept::sqf::is_class(intercept::sqf::config_entry(intercept::sqf::config_file()) >> "cfgmagazines" >> _item)) {
        intercept::sqf::add_magazine_cargo_global(_container, _item, _amount);
    }
    else if (intercept::sqf::is_class(intercept::sqf::config_entry(intercept::sqf::config_file()) >> "cfgvehicles" >> _item)) {
        float _isBackpack = intercept::sqf::get_number(intercept::sqf::config_entry(intercept::sqf::config_file()) >> "cfgvehicles" >> _item >> "isBackpack");
        if (_isBackpack > 0) {
            intercept::sqf::add_backpack_cargo_global(_container, _item, _amount);
        }
        else {
            intercept::sqf::add_item_cargo_global(_container, _item, _amount);
        }
    }
    else if (intercept::sqf::is_class(intercept::sqf::config_entry(intercept::sqf::config_file()) >> "cfgGlasses" >> _item)) {
        //"Equipment", "Glasses"
        intercept::sqf::add_item_cargo_global(_container, _item, _amount);
    }
    else {
        //dunno
        intercept::sqf::add_item_cargo_global(_container, _item, _amount);
    }
}

std::string utils::load_parameterized_file(const std::string& _path, const std::vector<std::string>& _parameters) {
    std::string _raw = static_cast<std::string>(intercept::sqf::preprocess_file_line_numbers(_path));
    std::vector<std::string> _parts = utils::split(_raw, '%');

    std::string _result;
    for (size_t _t = 0; _t < _parts.size(); _t++) {
        _result += _parts[_t];
        if (_t < _parameters.size()) {
            _result += _parameters[_t];
        }
    }

    return _result;
}

bool utils::is_straight_road(const object& _road) {
    
    intercept::client::invoker_lock lock;
    
    vector3 _agl = intercept::sqf::asl_to_agl(intercept::sqf::get_pos_asl(_road));
    int _dir = static_cast<int>(intercept::sqf::direction(_road));

    vector3 _infront = utils::get_rel_pos(_agl, 25, _dir);
    vector3 _behind = utils::get_rel_pos(_agl, 25, _dir - 180);
    if (intercept::sqf::is_on_road(_infront) && intercept::sqf::is_on_road(_behind)) return true;

    //due to inconsistency, some roads have their direction orthogonally to the direction they lead to
    _infront = utils::get_rel_pos(_agl, 25, _dir + 90);
    _behind = utils::get_rel_pos(_agl, 25, _dir + 270);

    return (intercept::sqf::is_on_road(_infront) && intercept::sqf::is_on_road(_behind));
}