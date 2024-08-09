#include "MatchingEngine.h"
#include <iostream>


int main()
{
    MatchingEngine engine;
    //Order o1(true,"RVNL",1,20,460.10);
    // Adding Limit Order
    engine.addOrder(Order(true,"RVNL",1,10,460.10)); //buy limit order
    engine.addOrder(Order(false,"RVNL",2,10,460.10)); //sell limit order
    //engine.printOrderBook();
    
    //adding market order
    //engine.addOrder(Order(false,"RVNL",3,10)); //sell market order
    engine.waitForAllOrdersToComplete();
    //engine.addOrder(Order(true,"TATASTEEL",1,20,1001.10));
    engine.printOrderBook();
    

    
    return 0;
}