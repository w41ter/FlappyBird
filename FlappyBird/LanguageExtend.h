#pragma once

//
// defend heritable.
//
template <typename OnlyFriend>
class Uninheritable
{
	friend OnlyFriend;
	Uninheritable(void) {}
};

template <typename _Init, typename _Pr>
inline void Map(_Init beg, _Init end_, _Pr pred) {
    for (; beg != end_; ++beg)
        pred(*beg);
}