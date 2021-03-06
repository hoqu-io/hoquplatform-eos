/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include "HOQUTokenHQX.hpp"

namespace eosio
{
    void HOQUTokenHQX::create(name issuer,
                            asset maximum_supply)
    {
        require_auth(_self);

        auto sym = maximum_supply.symbol;
        eosio_assert(sym.is_valid(), "invalid symbol name");
        eosio_assert(maximum_supply.is_valid(), "invalid supply");
        eosio_assert(maximum_supply.amount > 0, "max-supply must be positive");
        eosio_assert(maximum_supply.symbol.precision() == 9, "precision must be 9");

        stats statstable(_self, sym.code().raw());
        auto existing = statstable.find(sym.code().raw());
        eosio_assert(existing == statstable.end(), "token with symbol already exists");

        statstable.emplace(_self, [&](auto &s) {
            s.supply.symbol = maximum_supply.symbol;
            s.max_supply = maximum_supply;
            s.issuer = issuer;
        });
    }

    void HOQUTokenHQX::issue(name to, asset quantity, string memo)
    {
        auto sym = quantity.symbol;
        eosio_assert(sym.is_valid(), "invalid symbol name");
        eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

        stats statstable(_self, sym.code().raw());
        auto existing = statstable.find(sym.code().raw());
        eosio_assert(existing != statstable.end(), "token with symbol does not exist, create token before issue");
        const auto &st = *existing;

        require_auth(st.issuer);
        eosio_assert(quantity.is_valid(), "invalid quantity");
        eosio_assert(quantity.amount > 0, "must issue positive quantity");

        eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
        eosio_assert(quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply");

        statstable.modify(st, same_payer, [&](auto &s) {
            s.supply += quantity;
        });

        add_balance(st.issuer, quantity, st.issuer);

        if (to != st.issuer)
        {
            SEND_INLINE_ACTION(*this, transfer, {{st.issuer, "active"_n}},
                               {st.issuer, to, quantity, memo});
        }
    }

    void HOQUTokenHQX::swap(name to, asset quantity, string memo)
    {
        auto sym = quantity.symbol;
        eosio_assert(sym.is_valid(), "invalid symbol name");
        eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

        stats statstable(_self, sym.code().raw());
        auto existing = statstable.find(sym.code().raw());
        eosio_assert(existing != statstable.end(), "token with symbol does not exist, create token before issue");
        const auto &st = *existing;

        require_auth(st.issuer);

        SEND_INLINE_ACTION(*this, issue, {{st.issuer, "active"_n}},
                           {to, quantity, memo});

        holders holderstable(_self, st.issuer.value);
        auto it = holderstable.find( to.value );
        if (it == holderstable.end()) {
            holderstable.emplace(st.issuer, [&]( auto& a ){
                a.balance = quantity;
                a.account = to;
            });
        } else {
            holderstable.modify(it, st.issuer, [&](auto &s) {
                s.balance += quantity;
            });
        }
    }

    void HOQUTokenHQX::retire(asset quantity, string memo)
    {
        auto sym = quantity.symbol;
        eosio_assert(sym.is_valid(), "invalid symbol name");
        eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

        stats statstable(_self, sym.code().raw());
        auto existing = statstable.find(sym.code().raw());
        eosio_assert(existing != statstable.end(), "token with symbol does not exist");
        const auto &st = *existing;

        require_auth(st.issuer);
        eosio_assert(quantity.is_valid(), "invalid quantity");
        eosio_assert(quantity.amount > 0, "must retire positive quantity");

        eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");

        statstable.modify(st, same_payer, [&](auto &s) {
            s.supply -= quantity;
        });

        sub_balance(st.issuer, quantity);
    }

    void HOQUTokenHQX::transfer(name from,
                              name to,
                              asset quantity,
                              string memo)
    {
        eosio_assert(from != to, "cannot transfer to self");
        require_auth(from);
        eosio_assert(is_account(to), "to account does not exist");

        auto sym = quantity.symbol.code();
        stats statstable(_self, sym.raw());
        const auto &st = statstable.get(sym.raw());

        holders holderstable(_self, st.issuer.value);
        bool from_isholder = false;
        bool to_isholder = false;

        auto it_from = holderstable.find( from.value );
        if (it_from != holderstable.end()) {
            from_isholder = true;
        }

        auto it_to = holderstable.find( to.value );
        if (it_to != holderstable.end()) {
            to_isholder = true;
        }

        eosio_assert(from == st.issuer || ((to_isholder && from_isholder) || (!from_isholder && !to_isholder)), "transfer restricted");

        require_recipient(from);
        require_recipient(to);

        eosio_assert(quantity.is_valid(), "invalid quantity");
        eosio_assert(quantity.amount > 0, "must transfer positive quantity");
        eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
        eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

        auto payer = has_auth(to) ? to : from;

        sub_balance(from, quantity);
        add_balance(to, quantity, payer);
    }

    void HOQUTokenHQX::transferfrom(name from,
                                  name to,
                                  name spender,
                                  asset quantity,
                                  string memo)
    {
        eosio_assert(from != to, "cannot transfer to self");
        eosio_assert(is_account(from), "from account does not exist");
        eosio_assert(is_account(to), "to account does not exist");

        auto sym = quantity.symbol.code();
        stats statstable(_self, sym.raw());
        const auto &st = statstable.get(sym.raw());

        // Notify both the sender and receiver upon action completion
        require_recipient(from);
        require_recipient(to);
        require_recipient(spender);

        eosio_assert(quantity.is_valid(), "invalid quantity");
        eosio_assert(quantity.amount > 0, "must transfer positive quantity");
        eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
        eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

        allowed allowedtable(_self, from.value);
        auto existing = allowedtable.find(spender.value + sym.raw()); //Find returns an iterator pointing to the found object
        eosio_assert(existing != allowedtable.end(), "spender not allowed");
        const auto &at = *existing;

        require_auth(at.spender);
        eosio_assert(at.quantity.is_valid(), "invalid allowed quantity");
        eosio_assert(at.quantity.amount > 0, "allowed must be a positive quantity");
        eosio_assert(at.quantity.symbol == st.supply.symbol, "symbol precision mismatch");
        eosio_assert(at.quantity.amount >= quantity.amount, "Allowed quantity < Transfer Quantity");

        auto payer = has_auth(to) ? to : spender;

        sub_balancefrom(from, at.spender, quantity);
        add_balance(to, quantity, payer);
        allowedtable.modify(at, at.spender, [&](auto &a) {
            a.quantity -= quantity;
        });

        holders holderstable(_self, st.issuer.value);
        auto it_from = holderstable.find( from.value );
        if (it_from != holderstable.end()) {
            if (it_from->balance <= quantity) {
                holderstable.erase(it_from);
            } else {
                holderstable.modify(it_from, st.issuer, [&](auto &s) {
                    s.balance -= quantity;
                });
            }

        }
    }

    void HOQUTokenHQX::approve(name owner,
                             name spender,
                             asset quantity)
    {
        eosio_assert(owner != spender, "cannot allow self");

        require_auth(owner);
        eosio_assert(is_account(spender), "spender account does not exist");

        auto sym = quantity.symbol.code();
        stats statstable(_self, sym.raw());
        const auto &st = statstable.get(sym.raw());

        holders holderstable(_self, st.issuer.value);
        bool from_isholder = false;
        auto it_from = holderstable.find( owner.value );
        if (it_from != holderstable.end()) {
            from_isholder = true;
            eosio_assert(spender == st.issuer, "holders can approve only to issuer");
        }

        // Notify both the sender and receiver upon action completion
        require_recipient(owner);
        require_recipient(spender);

        eosio_assert(quantity.is_valid(), "invalid quantity");
        eosio_assert(quantity.amount > 0, "must transfer positive quantity");
        eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");

        // Making changes to allowed in owner scope
        allowed allowedtable(_self, owner.value);
        auto existing = allowedtable.find(spender.value + sym.raw()); //Find returns an iterator pointing to the found object
        if (existing == allowedtable.end())
        {
            allowedtable.emplace(owner, [&](auto &a) {
                a.key = spender.value + sym.raw();
                a.spender = spender;
                a.quantity = quantity;
            });
        }
        else
        {
            const auto &at = *existing;
            allowedtable.modify(at, owner, [&](auto &a) {
                a.quantity = quantity;
            });
        }
    }

    void HOQUTokenHQX::sub_balance(name owner, asset value)
    {
        accounts from_acnts(_self, owner.value);

        const auto &from = from_acnts.get(value.symbol.code().raw(), "no balance object found");
        eosio_assert(from.balance.amount >= value.amount, "overdrawn balance");

        from_acnts.modify(from, owner, [&](auto &a) {
            a.balance -= value;
        });
    }

    void HOQUTokenHQX::sub_balancefrom(name owner, name spender, asset value)
    {
        accounts from_acnts(_self, owner.value);

        const auto &from = from_acnts.get(value.symbol.code().raw(), "no balance object found");
        eosio_assert(from.balance.amount >= value.amount, "overdrawn balance");

        from_acnts.modify(from, spender, [&](auto &a) {
            a.balance -= value;
        });
    }

    void HOQUTokenHQX::add_balance(name owner, asset value, name ram_payer)
    {
        accounts to_acnts(_self, owner.value);
        auto to = to_acnts.find(value.symbol.code().raw());
        if (to == to_acnts.end())
        {
            to_acnts.emplace(ram_payer, [&](auto &a) {
                a.balance = value;
            });
        }
        else
        {
            to_acnts.modify(to, same_payer, [&](auto &a) {
                a.balance += value;
            });
        }
    }

    void HOQUTokenHQX::open( name owner, const symbol& symbol, name ram_payer )
    {
        require_auth( ram_payer );

        auto sym_code_raw = symbol.code().raw();

        stats statstable( _self, sym_code_raw );
        const auto& st = statstable.get( sym_code_raw, "symbol does not exist" );
        eosio_assert( st.supply.symbol == symbol, "symbol precision mismatch" );

        accounts acnts( _self, owner.value );
        auto it = acnts.find( sym_code_raw );
        if( it == acnts.end() ) {
            acnts.emplace( ram_payer, [&]( auto& a ){
                a.balance = asset{0, symbol};
            });
        }
    }

    void HOQUTokenHQX::close( name owner, const symbol& symbol )
    {
        require_auth( owner );
        accounts acnts( _self, owner.value );
        auto it = acnts.find( symbol.code().raw() );
        eosio_assert( it != acnts.end(), "Balance row already deleted or never existed. Action won't have any effect." );
        eosio_assert( it->balance.amount == 0, "Cannot close because the balance is not zero." );
        acnts.erase( it );
    }

} // namespace eosio

EOSIO_DISPATCH(eosio::HOQUTokenHQX, (create)(issue)(swap)(transfer)(approve)(transferfrom)(open)(close)(retire))