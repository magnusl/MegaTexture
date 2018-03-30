#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <vector>
#include <map>
#include <list>

namespace mt {

template<class K>
class LRUCache
{
public:
    LRUCache(size_t Size)
    {
        _freeSlots.resize(Size);
        for(size_t i = 0; i < Size; ++i) {
            _freeSlots[i] = i;
        }
    }

    // add a key to the LRU cache and possible evict another key
    size_t Put(K Key, K & Evicted)
    {
        Evicted = static_cast<size_t>(-1);
        std::map<K, size_t>::iterator keyIt = _keyToSlot.find(Key);
        if (keyIt != _keyToSlot.end()) {
            // key already has a slot
            std::list<K>::iterator it = std::find(_history.begin(), _history.end(), Key);
            if (it != _history.end()) {
                _history.erase(it);
            }
            _history.push_back(Key);
            // return the slot
            return keyIt->second;
        }
        else if (!_freeSlots.empty()) {
            // we have empty slots, take one
            size_t slot = _freeSlots.back();
            _freeSlots.pop_back();
            // map key to the slot
            _keyToSlot[Key] = slot;
            // add to history
            _history.push_back(Key);
            // return the slot
            return slot;
        } else {
            // cache is full, evict something
            if (!_history.empty()) {
                // remove the oldest entry
                Evicted = _history.front();
                _history.pop_front();
                // take the slot
                size_t slot = _keyToSlot[Evicted];
                // erase old item
                _keyToSlot.erase(Evicted);
                // return the new slot
                _keyToSlot[Key] = slot;
                return slot;
            } else {
                // should never happen
                return static_cast<size_t>(-1);
            }
        }
    }

protected:
    std::vector<size_t> _freeSlots;
    std::map<K, size_t> _keyToSlot;
    std::list<K>        _history;
};

} // namespace mt

#endif
