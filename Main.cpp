#include "MatchingEngine.h"
#include <iostream>



int main()
{
    MatchingEngine engine;
    engine.addDummyOrder();
    engine.printOrderBook();
    // while using order always make sure that you keep ID value more than 50.

    // Adding Limit Order
    //engine.addOrder(Order(true,"RVNL",51,10,460.10)); //buy limit order
    //engine.addOrder(Order(false,"RVNL",52,10,460.10)); //sell limit order
    
    
    //adding market order
    //engine.addOrder(Order(false,"RVNL",53,10)); //sell market order
    //engine.waitForAllOrdersToComplete();
    //engine.addOrder(Order(true,"TATASTEEL",1,20,1001.10));
    //engine.printOrderBook();
    

    
    return 0;
}