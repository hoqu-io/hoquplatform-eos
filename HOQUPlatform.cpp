#include "HOQUPlatform.hpp"
#include "actions/users/users.cpp"
#include "actions/networks/networks.cpp"
#include "actions/offers/offers.cpp"
#include "actions/ads/ads.cpp"
#include "actions/leads/leads.cpp"

EOSIO_DISPATCH( HOQUPlatform,
        (useradd)
        (userkycadd)
        (networkadd)
        (networkupd)
        (offeradd)
        (offerupd)
        (offtariffadd)
        (offtariffupd)
        (offtariffact)
        (offtariffoff)
        (adadd)
        (adupd)
        (leadadd)
        (leaddecline)
        (leadchtariff)
        (leadsell)
)