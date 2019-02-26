#ifndef HOQUPLATFORM_USERS_ACTION_HPP
#define HOQUPLATFORM_USERS_ACTION_HPP

#include "../../tables/users.hpp"

ACTION userlogin(name account, uint64_t secret );
ACTION useradd( name account, string role );
ACTION userkycadd( name account, string report, uint8_t level, uint32_t timestamp );
ACTION userrating( name account, uint8_t rating );
ACTION useractivate( name account );
ACTION userblock( name account );
ACTION userdelete( name account );

using user_login_action = action_wrapper<"userlogin"_n, &HOQUPlatform::userlogin>;
using user_add_action = action_wrapper<"useradd"_n, &HOQUPlatform::useradd>;
using user_add_kyc_action = action_wrapper<"userkycadd"_n, &HOQUPlatform::userkycadd>;
using user_set_rating_action = action_wrapper<"userkycadd"_n, &HOQUPlatform::userrating>;
using user_activate_action = action_wrapper<"userkycadd"_n, &HOQUPlatform::useractivate>;
using user_block_action = action_wrapper<"userkycadd"_n, &HOQUPlatform::userblock>;
using user_delete_action = action_wrapper<"userkycadd"_n, &HOQUPlatform::userdelete>;
#endif