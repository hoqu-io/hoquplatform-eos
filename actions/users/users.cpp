#ifndef HOQUPLATFORM_USERS_ACTION
#define HOQUPLATFORM_USERS_ACTION

ACTION HOQUPlatform::useradd( name account, string role) {
    require_auth(_code);

    print_f("trx_id: %", checksum256_to_string(get_trx_id()));
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

ACTION HOQUPlatform::userkycadd( name account, string report, uint8_t level) {
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