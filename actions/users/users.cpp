#ifndef HOQUPLATFORM_USERS_ACTION
#define HOQUPLATFORM_USERS_ACTION

ACTION HOQUPlatform::userlogin(name account, uint64_t secret) {
    require_auth(account);
}

ACTION HOQUPlatform::useradd( name account, string role) {
    if (!has_auth(account)) {
        require_auth(_code);
    }

    eosio_assert(is_account(account), "Unknown account");

    eosio_assert(user_roles.find(role) != user_roles.end(), "Unknown role");

    auto itr = users_table.find(account.value);

    if ( itr == users_table.end() ) {
        users_table.emplace( _self, [&]( auto& u ) {
            u.account = account;
            u.role = user_roles[role];
        });
    } else {
        eosio_assert(false, "User already exist");
    }
}

ACTION HOQUPlatform::userrating( name account, uint8_t rating) {
    require_auth(_code);

    eosio_assert(is_account(account), "Unknown account");

    auto itr = users_table.find(account.value);

    if ( itr != users_table.end() ) {
        users_table.modify( itr, _self, [&]( auto& u ) {
            u.rating = rating;
        });
    } else {
        eosio_assert(false, "User doesn`t exist");
    }
}

ACTION HOQUPlatform::useractivate( name account ) {
    require_auth(_code);

    eosio_assert(is_account(account), "Unknown account");
    auto itr = users_table.find(account.value);

    if ( itr != users_table.end() ) {
        users_table.modify( itr, _self, [&]( auto& u ) {
            u.status = user_statuses["active"];
        });
    } else {
        eosio_assert(false, "User doesn`t exist");
    }
}

ACTION HOQUPlatform::userblock( name account ) {
    require_auth(_code);

    eosio_assert(is_account(account), "Unknown account");
    auto itr = users_table.find(account.value);

    if ( itr != users_table.end() ) {
        users_table.modify( itr, _self, [&]( auto& u ) {
            u.status = user_statuses["blocked"];
        });
    } else {
        eosio_assert(false, "User doesn`t exist");
    }
}

ACTION HOQUPlatform::userdelete( name account ) {
    require_auth(_code);

    eosio_assert(is_account(account), "Unknown account");
    auto itr = users_table.find(account.value);

    if ( itr != users_table.end() ) {
        users_table.modify( itr, _self, [&]( auto& u ) {
            u.status = user_statuses["deleted"];
        });
    } else {
        eosio_assert(false, "User doesn`t exist");
    }
}

ACTION HOQUPlatform::userkycadd( name account, string report, uint8_t level, uint32_t timestamp) {
    require_auth(_code);

    auto itr = users_table.find(account.value);

    if ( itr != users_table.end() ) {

        kyc_report report;
        report.transaction_id = get_trx_id();
        report.level = level;

        users_table.modify( itr, _self, [&]( auto& u ) {
            u.reports.push_back(report);
            if (u.kyc_level < level) {
                u.kyc_level = level;
            }
        });
    }
}

#endif