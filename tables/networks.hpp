//
// Created by Artem Benois on 2019-01-18.
//

#ifndef HOQUPLATFORM_NETWORKS_TABLE_HPP
#define HOQUPLATFORM_NETWORKS_TABLE_HPP

map<string, uint8_t> network_statuses = {
        {"created",1},
        {"pending",2},
        {"active",3},
        {"paused",4},
        {"banned",5},
        {"deleted",6}
};


TABLE network {
    uint64_t id;
    name owner;
    uint32_t created_at = now();
    string name;
    uint8_t status = 1;

    uint64_t primary_key() const { return id; }
    uint64_t by_owner() const {return owner.value; }

    EOSLIB_SERIALIZE(network, (id)(owner)(created_at)(name)(status))
};

typedef eosio::multi_index<"networks"_n, network, indexed_by<"owner"_n, const_mem_fun< network, uint64_t, &network::by_owner>>> networks_index;

networks_index networks_table;
#endif
