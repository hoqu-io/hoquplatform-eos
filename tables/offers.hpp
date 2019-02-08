//
// Created by Artem Benois on 2019-01-18.
//

#ifndef HOQUPLATFORM_OFFERS_TABLE_HPP
#define HOQUPLATFORM_OFFERS_TABLE_HPP

map<string, uint8_t> offer_statuses = {
        {"created",1},
        {"pending",2},
        {"active",3},
        {"paused",4},
        {"declined",5},
        {"deleted",6}
};

map<string, uint8_t> tariff_statuses = {
        {"created",1},
        {"active",2},
        {"stopped",3}
};


struct offer_tariff {
    string action;
    asset value;
    double percent = 0;
    double commission = 0;
    bool is_percent;
    uint8_t status = 1;
};

//@abi table offers i64
TABLE offer {
    uint64_t id;
    uint64_t network_id;
    uint32_t created_at = current_time();
    name merchant;
    name payer;
    string name;
    uint8_t status = 1;
    map<uint8_t, offer_tariff> tariffs;

    uint64_t primary_key() const { return id; }
    uint64_t by_merchant() const {return merchant.value; }
    uint64_t by_network() const {return network_id; }

    EOSLIB_SERIALIZE(offer, (id)(network_id)(created_at)(merchant)(payer)(name)(status)(tariffs));
};

typedef eosio::multi_index<"offers"_n, offer,
                            indexed_by<"merchant"_n, const_mem_fun<offer, uint64_t, &offer::by_merchant>>,
                            indexed_by<"network"_n, const_mem_fun<offer, uint64_t, &offer::by_network>>> offers_index;

offers_index offers_table;
#endif //HOQUPLATFORM_OFFERS_TABLE_HPP
