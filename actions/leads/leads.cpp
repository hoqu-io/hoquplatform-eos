#ifndef HOQUPLATFORM_LEAD_ACTION
#define HOQUPLATFORM_LEAD_ACTION

ACTION HOQUPlatform::leadadd(uint64_t id, uint64_t ad_id, uint8_t tariff_id, asset price) {
    require_auth(_code);
    eosio_assert(price.symbol.precision() == 9 && price.symbol.code().to_string() == "HQX", "Wrong symbol or precision");

    auto lead = leads_table.find(id);
    if ( lead == leads_table.end() ) {
        auto ad = ads_table.find(ad_id);

        eosio_assert(ad != ads_table.end(), "AdCampaign doesn`t exist");
        eosio_assert(ad->status == ad_statuses["active"], "AdCampaign is not active");

        auto offer = offers_table.get(ad->offer_id);

        eosio_assert( offer.tariffs.find(tariff_id) != offer.tariffs.end(), "Tariff doesn`t exist");

        auto tariff = offer.tariffs.at(tariff_id);
        eosio_assert( tariff.status == tariff_statuses["active"], "Tariff is not active");

        asset lead_price;
        if (tariff.is_percent) {
            lead_price = calc_percent(price, tariff.percent);
        } else {
            lead_price = tariff.value;
        }

        leads_table.emplace( _self, [&]( auto& l ) {
            l.id = id;
            l.ad_id = ad_id;
            l.price = lead_price;
            l.tariff = tariff_id;
        });

    } else {
        eosio_assert(false, "Duplicate id");
    }
}

ACTION HOQUPlatform::leadchtariff(uint64_t id, uint8_t tariff_id, asset price, string memo) {
    require_auth(_code);
    eosio_assert(price.symbol.precision() == 9 && price.symbol.code().to_string() == "HQX", "Wrong symbol or precision");
    auto lead = leads_table.find(id);
    eosio_assert(lead != leads_table.end(), "Unknown lead id");
    auto ad = ads_table.get(lead->ad_id);
    auto offer = offers_table.get(ad.offer_id);

    if (tariff_id != lead->tariff) {
        eosio_assert( offer.tariffs.find(tariff_id) != offer.tariffs.end(), "Tariff doesn`t exist");
    }

    auto tariff = offer.tariffs.at(tariff_id);
    eosio_assert( tariff.status == tariff_statuses["active"], "Tariff is not active");

    asset lead_price;
    if (tariff.is_percent) {
        lead_price = calc_percent(price, tariff.percent);
    } else {
        lead_price = tariff.value;
    }

    leads_table.emplace( _self, [&]( auto& l ) {
        l.id = id;
        l.price = lead_price;
        l.tariff = tariff_id;
    });

}

ACTION HOQUPlatform::leaddecline(uint64_t id, string memo) {
    require_auth(_code);
    auto lead = leads_table.find(id);
    eosio_assert(lead != leads_table.end(), "Unknown lead id");
    leads_table.erase(lead);
}

ACTION HOQUPlatform::leadsell(uint64_t id) {
    require_auth(_code);
    auto lead = leads_table.find(id);
    eosio_assert(lead != leads_table.end(), "Unknown lead id");

    auto ad = ads_table.get(lead->ad_id);
    auto offer = offers_table.get(ad.offer_id);
    auto tariff = offer.tariffs.at(lead->tariff);
    auto network = networks_table.get(offer.network_id);

    asset network_price = calc_percent(lead->price, tariff.commission);
    asset hoquplatform_price = calc_percent(lead->price, 0.5);
    asset affiliate_price = lead->price - network_price - hoquplatform_price;

    action(
            permission_level{_self, "active"_n},
            "hoqutoken"_n,
            "transferfrom"_n,
            std::make_tuple(offer.payer, ad.affiliate, _self, affiliate_price, "hoquplatform lead id: " + uint64_t2hex(id))
    ).send();

    action(
            permission_level{_self, "active"_n},
            "hoqutoken"_n,
            "transferfrom"_n,
            std::make_tuple(offer.payer, network.owner, _self, network_price, "hoquplatform lead id: " + uint64_t2hex(id))
    ).send();

    action(
            permission_level{_self, "active"_n},
            "hoqutoken"_n,
            "transferfrom"_n,
            std::make_tuple(offer.payer, "hoquplatform"_n, _self, hoquplatform_price, "hoquplatform lead id: " + uint64_t2hex(id))
    ).send();

    leads_table.erase(lead);
}

#endif