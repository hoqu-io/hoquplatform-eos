#ifndef HOQUPLATFORM_ADS_ACTION_HPP
#define HOQUPLATFORM_ADS_ACTION_HPP

#include "../../tables/ads.hpp"

ACTION adadd(uint64_t id, uint64_t offer_id, name affiliate);
ACTION adupd(uint64_t id, string status);

using ad_add_action = action_wrapper<"adadd"_n, &HOQUPlatform::adadd>;
using ad_update_action = action_wrapper<"adupdate"_n, &HOQUPlatform::adupd>;

#endif