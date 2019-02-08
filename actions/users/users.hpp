#ifndef HOQUPLATFORM_USERS_ACTION_HPP
#define HOQUPLATFORM_USERS_ACTION_HPP

#include "../../tables/users.hpp"

ACTION useradd( name account, string role);
ACTION userkycadd( name account, string report, uint8_t level );

using user_add_action = action_wrapper<"useradd"_n, &HOQUPlatform::useradd>;
using user_add_kyc_action = action_wrapper<"userkycadd"_n, &HOQUPlatform::userkycadd>;

#endif