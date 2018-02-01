/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include <eosiolib/eos.hpp>
#include <eosiolib/token.hpp>
#include <eosiolib/db.hpp>
#include <eosiolib/generic_currency.hpp>

namespace coop {
   typedef eosio::generic_currency< eosio::token<N(coop),S(4,GOLD)> > gold;

   ACTION( N(coop), addqty ) {
      typedef action_meta<N(coop),N(addqty)> meta;
      account_name farmer;
      uint64_t     quantity;

      EOSLIB_SERIALIZE( addqty, (farmer)(quantity) )
   };

   ACTION( N(coop), purchase ) {
      typedef action_meta<N(coop),N(purchase)> meta;
      account_name trader;
      uint64_t     quantity;
      eosio::asset max_price;

      EOSLIB_SERIALIZE( purchase, (trader)(quantity)(max_price) )
   };

   ACTION( N(coop), fill ) {
      typedef action_meta<N(coop),N(fill)> meta;
      account_name trader;
      account_name farmer;
      uint64_t     quantity;

      EOSLIB_SERIALIZE( fill, (trader)(farmer)(quantity) )
   };

   struct farmer_data {
      account_name farmer;
      uint64_t     quantity;

      EOSLIB_SERIALIZE( farmer_data, (farmer)(quantity) )
   };
   typedef eosio::table64<N(coop), N(farmers), farmer_data>      farmers;

   struct purchase_data {
      account_name trader;
      account_name farmer;
      uint64_t     quantity;
      uint64_t     filled;

      EOSLIB_SERIALIZE( purchase_data, (trader)(farmer)(quantity)(filled) )
   };
   typedef eosio::table64<N(coop), N(purchases), purchase_data>      purchases;
}
