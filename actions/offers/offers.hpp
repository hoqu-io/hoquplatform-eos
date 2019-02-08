#ifndef HOQUPLATFORM_OFFERS_ACTION_HPP
#define HOQUPLATFORM_OFFERS_ACTION_HPP

#include "../../tables/offers.hpp"

ACTION offeradd(uint64_t id, uint64_t network_id, struct name merchant, string name, struct name payer);
ACTION offerupd(uint64_t id, string name, struct name payer, string status);
ACTION offtariffadd(uint64_t id, uint8_t tariff_id, string action, asset value, double percent, bool is_percent, double commission);
ACTION offtariffupd(uint64_t id, uint8_t tariff_id, string action, asset value, double percent, bool is_percent, double commission);
ACTION offtariffact(uint64_t id, uint8_t tariff_id);
ACTION offtariffoff(uint64_t id, uint8_t tariff_id);

using offer_add_action = action_wrapper<"offeradd"_n, &HOQUPlatform::offeradd>;
using offer_update_action = action_wrapper<"offerupd"_n, &HOQUPlatform::offerupd>;
using offer_tariff_add_action = action_wrapper<"offtariffadd"_n, &HOQUPlatform::offtariffadd>;
using offer_tariff_update_action = action_wrapper<"offtariffupd"_n, &HOQUPlatform::offtariffupd>;
using offer_tariff_activate_action = action_wrapper<"offtariffact"_n, &HOQUPlatform::offtariffact>;
using offer_tariff_deactivate_action = action_wrapper<"offtariffoff"_n, &HOQUPlatform::offtariffoff>;

#endif