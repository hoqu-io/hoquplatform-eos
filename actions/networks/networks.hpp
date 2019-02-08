#ifndef HOQUPLATFORM_NETWORKS_ACTION_HPP
#define HOQUPLATFORM_NETWORKS_ACTION_HPP

#include "../../tables/networks.hpp"

ACTION networkadd(uint64_t id, name owner, string name);
ACTION networkupd(uint64_t id, string name, string status);

using network_add_action = action_wrapper<"networkadd"_n, &HOQUPlatform::networkadd>;
using network_update_action = action_wrapper<"networkupd"_n, &HOQUPlatform::networkupd>;

#endif