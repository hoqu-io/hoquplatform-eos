#ifndef HOQUPLATFORM_AD_ACTION
#define HOQUPLATFORM_AD_ACTION

ACTION HOQUPlatform::adadd(uint64_t id, uint64_t offer_id, name affiliate) {
    require_auth(_code);

    auto itr = ads_table.find(id);

    if ( itr == ads_table.end() ) {
        //eosio_assert(is_account(affiliate), "Unknown account");

        auto itr = users_table.find(affiliate.value);

        if ( itr == users_table.end() ) {
            eosio_assert(false, "User doesn`t exist");
        } else {
            eosio_assert(itr->status == user_statuses["active"], "User must be active");
            eosio_assert(itr->role == user_roles["affiliate"], "User must be role affiliate");
            auto offer = offers_table.find(offer_id);
            eosio_assert(offer != offers_table.end(), "Offer does`t exist");
            eosio_assert(offer->status == offer_statuses["active"], "Offer is not active");

            ads_table.emplace( _self, [&]( auto& n ) {
                n.id = id;
                n.offer_id = offer_id;
                n.affiliate = affiliate;
            });
        }
    } else {
        eosio_assert(false, "Duplicate id");
    }
}

ACTION HOQUPlatform::adupd(uint64_t id, string status ) {
    require_auth(_code);

    eosio_assert(ad_statuses.find(status) != ad_statuses.end(), "Unknown status");

    auto itr = ads_table.find(id);

    if ( itr == ads_table.end() ) {
        eosio_assert(false, "Ad doesn`t exist");
    } else {
        ads_table.modify( itr, _self, [&]( auto& n ) {
            n.status = ad_statuses[status];
        });
    }
}

#endif