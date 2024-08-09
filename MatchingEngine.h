#include "OrderBook.h"
#include <fstream>
#include <sstream>

class MatchingEngine
{
    private:
        OrderBook orderBook;
        std::unordered_map< int, Order> allOrders;
        std::vector<std::thread> threads;
        std::mutex gLock;

        void matchMarketOrder(const Order &order);
        void matchMarketBuyOrder(const Order &order);
        void matchMarketSellOrder(const Order &order);
        void matchLimitOrder(const Order &order);
        void matchLimitSellOrder(const Order &order);
        void matchLimitBuyOrder(const Order &order);

    public:
        void addOrder(const Order &order);
        void cancelOrder(int orderId);
        void modifyOrder(int orderId, int newQuantity, double newPrice);
        void printOrderBook();
        void waitForAllOrdersToComplete();
        void addDummyOrder();

};


void MatchingEngine::matchMarketOrder(const Order &order)
{
    std::lock_guard<std::mutex> lock(gLock);
    if(order.isBuy)
        matchMarketBuyOrder(order);
    else
        matchMarketSellOrder(order);
}

void MatchingEngine::matchMarketBuyOrder(const Order &order)
{
    
    int remainingQuantity = order.quantity;
    if(orderBook.sellOrders.find(order.symbol)==orderBook.sellOrders.end() || orderBook.sellOrders[order.symbol].empty())
    {
        orderBook.addOrder(order);
        std::cout << order.symbol << " No sell Market Order found : Quantity " << remainingQuantity << "\n";
        return;
    }

    std::sort(orderBook.sellOrders[order.symbol].begin(), orderBook.sellOrders[order.symbol].end(), 
    [](Order a, Order b){
        return a.price < b.price;
    });
    
    for(auto orders = orderBook.sellOrders[order.symbol].begin();orders<orderBook.sellOrders[order.symbol].end();orders++)
    {
        if(remainingQuantity>=orders->quantity)
        {
            remainingQuantity -= orders->quantity;
            std::cout << "Matched: Buy Order ID " << order.id << " with Sell Order ID " << orders->id << " at Price " << orders->price << ", Quantity " << orders->quantity << "\n";
            orderBook.sellOrders[order.symbol].erase(orders);
        }
        else
        {
            orders->quantity -= remainingQuantity;
            std::cout << "Matched: Buy Order ID " << order.id << " with Sell Order ID " << orders->id << " at Price " << orders->price << ", Quantity " << orders->quantity << "\n";
            remainingQuantity = 0;
        }
    }

    if (remainingQuantity > 0)
        std::cout << order.symbol << " Unfilled Buy Market Order: Quantity " << remainingQuantity << "\n";
}

void MatchingEngine::matchMarketSellOrder(const Order &order)
{

    int remainingQuantity = order.quantity;
    if(orderBook.buyOrders.find(order.symbol)==orderBook.buyOrders.end() || orderBook.buyOrders[order.symbol].empty())
    {
        orderBook.addOrder(order);
        std::cout << order.symbol << " No Buy Market Order found : Quantity " << remainingQuantity << "\n";
        return;
    }

    std::sort(orderBook.buyOrders[order.symbol].begin(), orderBook.buyOrders[order.symbol].end(), 
    [](Order a, Order b){
        return a.price > b.price;
    });
    
    for(auto orders = orderBook.buyOrders[order.symbol].begin();orders<orderBook.buyOrders[order.symbol].end();orders++)
    {
        if(remainingQuantity>=orders->quantity)
        {
            remainingQuantity -= orders->quantity;
            std::cout << "Matched: Market Sell Order ID " << order.id << " with Buy Order ID " << orders->id << " at Price " << orders->price << ", Quantity " << orders->quantity << "\n";
            orderBook.buyOrders[order.symbol].erase(orders);
        }
        else
        {
            orders->quantity -= remainingQuantity;
            std::cout << "Matched: Market Sell Order ID " << order.id << " with Buy Order ID " << orders->id << " at Price " << orders->price << ", Quantity " << orders->quantity << "\n";
            remainingQuantity = 0;
        }
    }

    if (remainingQuantity > 0)
        std::cout << order.symbol << " Unfilled Sell Market Order: Quantity " << remainingQuantity << "\n";
}

void MatchingEngine::matchLimitOrder(const Order &order)
{
    std::lock_guard<std::mutex> lock(gLock);
    if (order.isBuy) 
        matchLimitBuyOrder(order);
    else
        matchLimitSellOrder(order);

}

void MatchingEngine::matchLimitBuyOrder(const Order &order)
{

    int remainingQuantity = order.quantity;

    if(orderBook.sellOrders.find(order.symbol)==orderBook.sellOrders.end() || orderBook.sellOrders[order.symbol].empty())
    {
        orderBook.addOrder(order);
        std::cout << order.symbol << " No limit sell Order found : Quantity " << remainingQuantity << "\n";
        return;
    }

    std::sort(orderBook.sellOrders[order.symbol].begin(), orderBook.sellOrders[order.symbol].end(), 
    [](Order a, Order b){
        return a.price < b.price;
    });

    bool flag = false;
    for(auto orders = orderBook.sellOrders[order.symbol].begin();orders<orderBook.sellOrders[order.symbol].end();orders++)
    {
        if(orders->price <= order.price)
        {
            flag = true;
            if(remainingQuantity>=orders->quantity)
            {
                remainingQuantity -= orders->quantity;
                std::cout << "Matched: Limit Buy Order ID " << order.id << " with Sell Order ID " << orders->id << " at Price " << orders->price << ", Quantity " << orders->quantity << "\n";
                orderBook.sellOrders[order.symbol].erase(orders);
            }
            else
            {
                orders->quantity -= remainingQuantity;
                std::cout << "Matched: Limit Buy Order ID " << order.id << " with Sell Order ID " << orders->id << " at Price " << orders->price << ", Quantity " << orders->quantity << "\n";
                remainingQuantity = 0;
            }
        }
    }

    if (remainingQuantity > 0 && flag) 
    {
        Order unfilledOrder = order;
        unfilledOrder.quantity = remainingQuantity;
        orderBook.addOrder(unfilledOrder);
    }
}

void MatchingEngine::matchLimitSellOrder(const Order &order)
{

    int remainingQuantity = order.quantity;
    if(orderBook.buyOrders.find(order.symbol)==orderBook.buyOrders.end() || orderBook.buyOrders[order.symbol].empty())
    {
        orderBook.addOrder(order);
        std::cout << order.symbol << " No Buy Market Order found : Quantity " << remainingQuantity << "\n";
        return;
    }

    std::sort(orderBook.buyOrders[order.symbol].begin(), orderBook.buyOrders[order.symbol].end(), 
    [](Order a, Order b){
        return a.price > b.price;
    });
    
    bool flag = false;
    for(auto orders = orderBook.buyOrders[order.symbol].begin();orders<orderBook.buyOrders[order.symbol].end();orders++)
    {
        if(orders->price >= order.price)
        {
            flag = true;
            if(remainingQuantity>=orders->quantity)
            {
                remainingQuantity -= orders->quantity;
                std::cout << "Matched: Limit Sell Order ID " << order.id << " with Buy Order ID " << orders->id << " at Price " << orders->price << ", Quantity " << orders->quantity << "\n";
                orderBook.buyOrders[order.symbol].erase(orders);
            }
            else
            {
                orders->quantity -= remainingQuantity;
                std::cout << "Matched: Limit Sell Order ID " << order.id << " with Buy Order ID " << orders->id << " at Price " << orders->price << ", Quantity " << remainingQuantity << "\n";
                remainingQuantity = 0;
            }
        }
    }

    if (remainingQuantity > 0 && flag) 
    {
        Order unfilledOrder = order;
        unfilledOrder.quantity = remainingQuantity;
        orderBook.addOrder(unfilledOrder);
    }
}

void MatchingEngine::addOrder(const Order &order)
{
    std::lock_guard<std::mutex> lock(gLock);
    allOrders[order.id] = order;
    if(order.type==OrderType::Market)
        threads.emplace_back(&MatchingEngine::matchMarketOrder, this, order);
    else
        threads.emplace_back(&MatchingEngine::matchLimitOrder, this, order);
}

void MatchingEngine::cancelOrder(int orderId)
{
    std::lock_guard<std::mutex> lock(gLock);
    auto it = allOrders.find(orderId);
    if(it!=allOrders.end())
    {
        orderBook.removeOrder(it->second);
        allOrders.erase(it);
    }
}

void MatchingEngine::modifyOrder(int orderId, int newQuantity, double newPrice)
{
    std::lock_guard<std::mutex> lock(gLock);
    auto it = allOrders.find(orderId);
    if(it!=allOrders.end())
    {
        Order modifiedOrder = it->second;
        cancelOrder(orderId);
        modifiedOrder.quantity = newQuantity;
        
        if(newPrice>0)
            modifiedOrder.price = newPrice;
        
        addOrder(modifiedOrder);
    }
}

void MatchingEngine::printOrderBook()
{
    std::lock_guard<std::mutex> lock(gLock);
    orderBook.printOrderBook();
}

void MatchingEngine::waitForAllOrdersToComplete()
{
    for (auto &thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    threads.clear();
}

void MatchingEngine::addDummyOrder()
{
    std::ifstream file("DummyOrder.yaml");
    std::string line;
    while(std::getline(file, line))
    {
        std::istringstream is(line);
        bool isBuy;
        std::string typeStr;
        std::string symbol;
        int id;
        int quantity;
        double price;

        is >> isBuy >> typeStr >> symbol >> id >> quantity >> price;
        Order o1;
        o1.symbol =symbol;
        o1.isBuy = isBuy;
        o1.id = id;
        o1.quantity = quantity;
        o1.price = price;
        o1.type = (typeStr == "LIMIT") ? OrderType::Limit : OrderType::Market;
        orderBook.addOrder(o1);
    }
}