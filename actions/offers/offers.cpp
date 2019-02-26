#ifndef HOQUPLATFORM_OFFERS_ACTION
#define HOQUPLATFORM_OFFERS_ACTION

ACTION HOQUPlatform::offeradd(uint64_t id, uint64_t network_id, struct name merchant, string name, struct name payer) {
    require_auth(_code);
    eosio_assert(is_account(merchant), "Unknown merchant account");
    if (merchant != payer) {
        eosio_assert(is_account(payer), "Unknown payer account");
    }

    auto offer_itr = offers_table.find(id);
    if ( offer_itr == offers_table.end() ) {
        auto network_itr = networks_table.find(network_id);

        if ( network_itr == networks_table.end() ) {
            eosio_assert(false, "Network doesn`t exist");
        }

        auto itr = users_table.find(merchant.value);

        if ( itr == users_table.end() ) {
            eosio_assert(false, "User doesn`t exist");
        } else {
            eosio_assert(itr->status == user_statuses["active"], "Merchant user must be active");
            eosio_assert(itr->role == user_roles["merchant"], "Merchant user must be role merchant");

            offers_table.emplace( _self, [&]( auto& n ) {
                n.id = id;
                n.network_id = network_id;
                n.merchant = merchant;
                n.payer = payer;
                n.name = name;
            });
        }
    } else {
        eosio_assert(false, "Duplicate id");
    }
}

ACTION HOQUPlatform::offerupd(uint64_t id, string name, struct name payer, string status) {
    require_auth(_code);
    eosio_assert(is_account(payer), "Unknown payer account");

    eosio_assert(offer_statuses.find(status) != offer_statuses.end(), "Unknown status");

    auto itr = offers_table.find(id);

    if (status != "active") {
        auto offer_idx = ads_table.get_index<"byactoffer"_n>();
        auto itr = offer_idx.find(id);
        while (itr != offer_idx.end()) {
            ads_table.modify(ads_table.get(itr->id),_self, [&]( auto& ad ) {
                ad.status = ad_statuses["inactive"];
            });
            itr = offer_idx.find(id);
        }
    }

    if ( itr == offers_table.end() ) {
        eosio_assert(false, "Offer doesn`t exist");
    } else {
        offers_table.modify(itr, _self, [&]( auto& o ) {
            o.name = name;
            o.payer = payer;
            o.status = offer_statuses[status];
        });
    }
}

ACTION HOQUPlatform::offtariffadd(uint64_t id, uint8_t tariff_id, string action, asset value, double percent, bool is_percent, double commission) {
    require_auth(_code);
    eosio_assert(value.symbol.precision() == 9 && value.symbol.code().to_string() == "HQX", "Wrong symbol or precision");
    eosio_assert(commission <= 99.5, "Commission must be less then 99.5%");

    auto itr = offers_table.find(id);

    if ( itr != offers_table.end() ) {
        eosio_assert(itr->tariffs.find(tariff_id) == itr->tariffs.end(), "Use method 'offtariffupd' to update existing tariff");
        offer_tariff tariff;
        tariff.action = action;
        tariff.value = value;
        tariff.percent = percent;
        tariff.is_percent = is_percent;
        tariff.commission = commission;

        offers_table.modify( itr, _self, [&]( auto& ot ) {
            ot.tariffs[tariff_id] = tariff;
        });
    }
}

ACTION HOQUPlatform::offtariffupd(uint64_t id, uint8_t tariff_id, string action, asset value, double percent, bool is_percent, double commission) {
    require_auth(_code);
    eosio_assert(value.symbol.precision() == 9 && value.symbol.code().to_string() == "HQX", "Wrong symbol or precision");
    eosio_assert(commission <= 99.5, "Commission must be less then 99.5%");

    auto itr = offers_table.find(id);

    if ( itr != offers_table.end() ) {
        eosio_assert(itr->tariffs.find(tariff_id) != itr->tariffs.end(), "Offer tariff doesn`t exist");
        eosio_assert(itr->tariffs.at(tariff_id).status == tariff_statuses["created"], "Can`t update active or stopped tariff");
        offer_tariff tariff;
        tariff.action = action;
        tariff.value = value;
        tariff.percent = percent;
        tariff.is_percent = is_percent;
        tariff.commission = commission;

        offers_table.modify( itr, _self, [&]( auto& ot ) {
            ot.tariffs[tariff_id] = tariff;
        });
    }
}

ACTION HOQUPlatform::offtariffact(uint64_t id, uint8_t tariff_id) {
    require_auth(_code);

    auto itr = offers_table.find(id);

    if ( itr != offers_table.end() ) {
        eosio_assert(itr->tariffs.find(tariff_id) != itr->tariffs.end(), "Offer tariff doesn`t exist");
        eosio_assert(itr->tariffs.at(tariff_id).status == tariff_statuses["created"], "Can`t activate tariff");
        offers_table.modify( itr, _self, [&]( auto& ot ) {
            ot.tariffs[tariff_id].status = tariff_statuses["active"];
        });
    }
}

ACTION HOQUPlatform::offtariffoff(uint64_t id, uint8_t tariff_id) {
    require_auth(_code);

    auto itr = offers_table.find(id);

    if ( itr != offers_table.end() ) {
        eosio_assert(itr->tariffs.find(tariff_id) != itr->tariffs.end(), "Offer tariff doesn`t exist");
        offers_table.modify( itr, _self, [&]( auto& ot ) {
            ot.tariffs[tariff_id].status = tariff_statuses["stopped"];
        });
    }
}

#endif