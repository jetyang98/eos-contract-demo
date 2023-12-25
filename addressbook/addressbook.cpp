#include <eosio/eosio.hpp>

using namespace eosio;

class [[eosio::contract("addressbook")]] addressbook : public contract {

public:
    addressbook(name receiver, name code, datastream<const char *> ds) : contract(receiver, code, ds) {}

    [[eosio::action]]
    void upsert(name user, std::string name, std::uint8_t age) {
        require_auth(user);
        address_index addresses(get_self(), get_first_receiver().value);
        auto iterator = addresses.find(user.value);
        if(iterator == addresses.end()) {
            addresses.emplace(user, [&](person &row) {
                row.key = user;
                row.name = name;
                row.age = age;
            });
        } else {
            addresses.modify(iterator, user, [&](person &row) {
                row.key = user;
                row.name = name;
                row.age = age;
            });
        }
    }

    [[eosio::action]]
    void erase(name user) {
        require_auth(user);

        address_index addresses(get_self(), get_first_receiver().value);

        auto iterator = addresses.find(user.value);
        check(iterator != addresses.end(), "Record does not exist");
        addresses.erase(iterator);
    }

private:
    struct [[eosio::table]] person {
        name key;
        std::string name;
        std::uint8_t age;
        uint64_t primary_key() const { return key.value; }
    };

    using address_index = multi_index<"people"_n, person>;
};