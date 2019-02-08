#pragma once
#include <eosiolib/eosio.hpp>
#include <eosiolib/transaction.hpp>
#include <eosiolib/chain.h>
#include <eosiolib/crypto.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/fixedpoint.hpp>
#include <string>
#include <vector>
#include <map>

using namespace eosio;
using std::string;
using std::vector;
using std::map;

capi_checksum256 get_trx_id( bool assert_only_one_action = true ) {
    capi_checksum256 trx_id;
    std::vector<char> trx_bytes;
    size_t trx_size = transaction_size();
    trx_bytes.resize(trx_size);
    read_transaction(trx_bytes.data(), trx_size);
    sha256( trx_bytes.data(), trx_size, &trx_id );

    if ( assert_only_one_action ) {
        auto trx = unpack<transaction>(trx_bytes.data(), trx_size);
        eosio_assert( trx.actions.size() == 1, "transaction contains more then one action");
    }

    return trx_id;
}

string checksum256_to_string(const capi_checksum256 c)
{
    char hexstr[64];
    for (int i = 0; i < 32; i++)
    {
        sprintf(hexstr + i * 2, "%02x", c.hash[i]);
    }
    string c_str = std::string(hexstr);
    return c_str;
}

string uint64_t2hex(uint64_t num)
{
    char hexstr[16];

    sprintf(hexstr, "%016llX\n", num);

    string c_str = string(hexstr);
    return "0x" + c_str;
}

asset calc_percent(asset value, double percent)
{
    value.amount = value.amount * (int)(percent * 1000) / 100000;
    return value;
}

CONTRACT HOQUPlatform : public contract {
    public:
        using contract::contract;
        HOQUPlatform( name receiver, name code, datastream<const char*> ds )
            : contract(receiver, code, ds)
            , users_table(receiver, receiver.value)
            , networks_table(receiver, receiver.value)
            , offers_table(receiver, receiver.value)
            , ads_table(receiver, receiver.value)
            , leads_table(receiver, receiver.value)

            {}

        #include "actions/users/users.hpp"
        #include "actions/networks/networks.hpp"
        #include "actions/offers/offers.hpp"
        #include "actions/ads/ads.hpp"
        #include "actions/leads/leads.hpp"
};
