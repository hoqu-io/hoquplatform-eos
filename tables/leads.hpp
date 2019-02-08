//
// Created by Artem Benois on 2019-01-18.
//

#ifndef HOQUPLATFORM_LEADS_TABLE_HPP
#define HOQUPLATFORM_LEADS_TABLE_HPP

TABLE lead {
    uint64_t id;
    uint64_t ad_id;
    uint8_t tariff;
    asset price;
    uint8_t status;

    uint64_t primary_key() const { return id; }

    EOSLIB_SERIALIZE(lead, (id)(ad_id)(tariff)(price)(status))
};

typedef eosio::multi_index<"leads"_n, lead> leads_index;

leads_index leads_table;
#endif //HOQUPLATFORM_LEADS_TABLE_HPP
