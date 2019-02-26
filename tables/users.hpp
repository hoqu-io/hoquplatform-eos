//
// Created by Artem Benois on 2019-01-18.
//

#ifndef HOQUPLATFORM_USERS_TABLE_HPP
#define HOQUPLATFORM_USERS_TABLE_HPP

map<string, uint8_t> user_roles = {
        {"affiliate", 1},
        {"merchant", 2},
        {"network", 3},
        {"admin", 4}
};

map<string, uint8_t> user_statuses = {
        {"created",1},
        {"active", 2},
        {"blocked",3},
        {"deleted",4}
};

struct kyc_report {
    capi_checksum256 transaction_id;
    uint8_t level;
};

TABLE user {
    name account;
    uint32_t created_at = now();
    uint32_t rating = 0;
    uint8_t role;
    uint8_t kyc_level = 1;
    uint8_t status = 1;
    vector<kyc_report> reports;

    uint64_t primary_key() const { return account.value; }


    EOSLIB_SERIALIZE(user, (account)(created_at)(rating)(role)(kyc_level)(status)(reports))
};

//typedef eosio::multi_index<"users"_n, user, indexed_by<"account"_n, const_mem_fun< user, name, &user::by_account>>> users_index;
typedef eosio::multi_index<"users"_n, user> users_index;

users_index users_table;
#endif //HOQUPLATFORM_USERS_TABLE_HPP
