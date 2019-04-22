#ifndef HOQUPLATFORM_LEADS_ACTION_HPP
#define HOQUPLATFORM_LEADS_ACTION_HPP

#include "../../tables/leads.hpp"

ACTION leadadd(uint64_t id, uint64_t ad_id, uint8_t tariff_id,  asset price, string meta, string data, uint32_t timestamp);
ACTION leadchtariff(uint64_t id, uint8_t tariff_id, asset price, string memo);
ACTION leaddecline(uint64_t id, string memo);
ACTION leadsell(uint64_t id);

using lead_add_action = action_wrapper<"leadadd"_n, &HOQUPlatform::leadadd>;
using lead_update_action = action_wrapper<"leaddecline"_n, &HOQUPlatform::leadchtariff>;
using lead_decline_action = action_wrapper<"leaddecline"_n, &HOQUPlatform::leaddecline>;
using lead_sell_action = action_wrapper<"leadsell"_n, &HOQUPlatform::leadsell>;

#endif