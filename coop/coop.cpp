/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include <coop/coop.hpp> /// defines transfer struct (abi)
namespace coop {
   struct impl {
      static eosio::asset price() {
         return eosio::asset(10, S(4, GOLD));
      }

      static void on( const addqty& act ) {
         require_auth( addqty::get_account() );
         require_auth( act.farmer );
         auto farmer = farmers::get_or_create( act.farmer );
         farmer.quantity += act.quantity;
         eosio::print("farmer=", act.farmer, ", quantity=", farmer.quantity, "\n");
         farmers::set( farmer );
      }

      static void on( const purchase& act ) {
         require_auth( purchase::get_account() );
         require_auth( act.trader );
         assert (act.max_price.symbol == price().symbol, "max_price has incorrect symbol");
         assert (act.max_price.amount >= price().amount, "max_price below selling price");

         farmer_data farmer;
         eosio::print("trader=", act.trader, ", quantity=", act.quantity, "\n");
         assert(front_i64(N(coop),N(coop),N(farmers), &farmer, sizeof(farmer_data)), "no farmers in coop");
         eosio::print("farmer=", farmer.farmer, ", quantity=", farmer.quantity, "\n");
         while (farmer.quantity < act.quantity)
            assert(next_i64(N(coop),N(coop),N(farmers), &farmer, sizeof(farmer_data)), "no farmers with enough quantity");
         eosio::print("selected farmer=", farmer.farmer, ", quantity=", farmer.quantity, "\n");

         farmer.quantity -= act.quantity;
         farmers::set(farmer);

         // simplifying code and not worrying about repeat entries
         auto purchase = purchases::get_or_create(act.trader);
         purchase.farmer = farmer.farmer;
         purchase.quantity = act.quantity;
         purchase.filled = 0;
         eosio::print("purchased farmer=", purchase.farmer, ", quantity=", purchase.quantity, ", trader=", act.trader, "\n");
         require_recipient(purchase.farmer);
      }

      static void on( const fill& act ) {
         require_auth( fill::get_account() );
         require_auth( act.trader );
         auto purchase = purchases::get(act.trader);
         assert (!purchase.filled && purchase.farmer == act.farmer && purchase.quantity == act.quantity, "no trade found");
         eosio::print("purchased farmer=", purchase.farmer, ", quantity=", purchase.quantity, ", trader=", act.trader, "\n");
         purchase.filled = 1;
         purchases::set(purchase);
      }
   };
}

extern "C" {
    /// The apply method implements the dispatch of events to this contract
    void apply( uint64_t code, uint64_t action ) {
       if (!eosio::dispatch<coop::impl, coop::addqty, coop::purchase, coop::fill>(code, action)) {
          if ( !eosio::dispatch<coop::gold, coop::gold::transfer_memo, coop::gold::issue>( code, action ) ) {
             assert( false, "received unexpected action" );
          }
       }
    }
}
