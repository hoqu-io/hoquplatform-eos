#ifndef HOQUPLATFORM_NETWORK_ACTION
#define HOQUPLATFORM_NETWORK_ACTION

ACTION HOQUPlatform::networkadd(uint64_t id, name owner, string name) {
    require_auth(_code);

    auto itr = networks_table.find(id);

    if ( itr == networks_table.end() ) {
        eosio_assert(is_account(owner), "Unknown account");

        auto itr = users_table.find(owner.value);

        if ( itr == users_table.end() ) {
            eosio_assert(false, "User doesn`t exist");
        } else {
            eosio_assert(itr->role == user_roles["network"], "User must be role network");

            networks_table.emplace( _self, [&]( auto& n ) {
                n.id = id;
                n.owner = owner;
                n.name = name;
            });
        }
    } else {
        eosio_assert(false, "Duplicate id");
    }
}

ACTION HOQUPlatform::networkupd(uint64_t id, string name, string status ) {
    require_auth(_code);

    eosio_assert(network_statuses.find(status) != network_statuses.end(), "Unknown status");

    auto itr = networks_table.find(id);

    if ( itr == networks_table.end() ) {
        eosio_assert(false, "Network doesn`t exist");
    } else {
        networks_table.modify( itr, _self, [&]( auto& n ) {
            n.name = name;
            n.status = network_statuses[status];
        });
    }
}

#endif