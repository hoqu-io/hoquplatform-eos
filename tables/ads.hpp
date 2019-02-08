//
// Created by Artem Benois on 2019-01-18.
//

#ifndef HOQUPLATFORM_ADS_TABLE_HPP
#define HOQUPLATFORM_ADS_TABLE_HPP

map<string, uint8_t> ad_statuses = {
        {"created",1},
        {"pending",2},
        {"active",3},
        {"paused",4},
        {"inactive",5},
        {"banned",6},
        {"deleted",7}
};

TABLE ad {
    uint64_t id;
    uint64_t offer_id;
    name affiliate;
    uint8_t status = 1;

    uint64_t primary_key() const { return id; }
    uint64_t by_active_offer() const { return status == 3? offer_id : 0; }
    uint64_t by_inactive_offer() const { return status == 5? offer_id : 0; }

    EOSLIB_SERIALIZE(ad, (id)(offer_id)(affiliate)(status))
};

//typedef eosio::multi_index<"ads"_n, ad, indexed_by<"offer"_n, const_mem_fun<ad, uint64_t, &ad::by_offer>>> ads_index;

typedef eosio::multi_index<"ads"_n, ad,
                            indexed_by<"byactoffer"_n, const_mem_fun<ad, uint64_t, &ad::by_active_offer>>,
                            indexed_by<"byinactoffer"_n, const_mem_fun<ad, uint64_t, &ad::by_inactive_offer>>
                          > ads_index;

ads_index ads_table;
#endif //HOQUPLATFORM_ADS_TABLE_HPP
