#pragma once

#ifndef TS_U_MAP_H
#define TS_U_MAP_H

#include <unordered_map>
#include <shared_mutex>
#include <mutex>

template<typename K, typename V>
class threadsafe_unordered_map {

private:
    std::unordered_map<K, V> map;
    std::shared_mutex mtx;

public:
    threadsafe_unordered_map() {};
    ~threadsafe_unordered_map() {};

    threadsafe_unordered_map(const threadsafe_unordered_map& _map) = delete;

    void insert_or_assign(const K& _key, const V& _val) {
        std::unique_lock<std::shared_mutex>(mtx);
        map.insert_or_assign(_key, _val);
    };
    //throws std::out_of_range if item not found
    V get(const K& _key) {
        std::shared_lock<std::shared_mutex>(mtx);
        return map.at(_key);
    };
    std::vector<V> get_all_values() {
        std::vector<V> _vec;
        std::shared_lock<std::shared_mutex>(mtx);
        _vec.reserve(map.size());
        for (auto& _x : map) {
            _vec.push_back(_x.second);
        }
        return _vec;
    };
    std::vector<K> get_all_keys() {
        std::vector<K> _vec;
        std::shared_lock<std::shared_mutex>(mtx);
        _vec.reserve(map.size());
        for (auto& _x : map) {
            _vec.push_back(_x.first);
        }
        return _vec;
    };
    bool has(const K& _key) {
        std::shared_lock<std::shared_mutex>(mtx);
        return map.find(_key) != map.end();
    };
    void erase(const K& _key) {
        std::unique_lock<std::shared_mutex>(mtx);
        map.erase(_key);
    };
    void remove_by_value(const V& _value) {
        std::vector<K> _vec;
        std::unique_lock<std::shared_mutex>(mtx);
        _vec.reserve(map.size());
        for (auto& _x : map) {
            _vec.push_back(_x.first);
        }
        for (auto& _x : _vec) {
            map.erase(_x);
        }
    };

};

#endif // !TS_U_MAP_H


