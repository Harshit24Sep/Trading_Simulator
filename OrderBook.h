#include "Order.h"
#include<iostream>
#include<vector>
#include<mutex>
#include<thread>
#include<unordered_map>
#include<algorithm>


class OrderBook
{
    public:
        std::unordered_map< std::string, std::vector<Order> > sellOrders;
        std::unordered_map< std::string, std::vector<Order> > buyOrders;
        std:: mutex gLock;

        void addOrder(const Order &order);
        void removeOrder(const Order &order);
        void printOrderBook();
};

void OrderBook::addOrder(const Order &order)
{
    std::lock_guard<std::mutex> lock(gLock);
    //critical section code start
    if(order.isBuy)
        buyOrders[order.symbol].push_back(order);
    else
        sellOrders[order.symbol].push_back(order);
    //critical section code end
}

void OrderBook::removeOrder(const Order &order)
{
    std::lock_guard<std::mutex> lock(gLock);
    //critical section code start
    if(order.isBuy)
    {
        for(auto it = buyOrders[order.symbol].begin(); it!=buyOrders[order.symbol].end();it++)
        {
            if(it->id==order.id)
                buyOrders[order.symbol].erase(it);
        }
    }
    else
    {
        for(auto it = sellOrders[order.symbol].begin(); it!=sellOrders[order.symbol].end();it++)
        {
            if(it->id==order.id)
                sellOrders[order.symbol].erase(it);
        }
    }
    //critical section code end
}
void OrderBook::printOrderBook()
{
    std::lock_guard<std::mutex> lock(gLock);
    //Printing Buy order book
    std::cout<<"Buy Orders : \n";
    for(auto it:buyOrders)
    {
        std::cout<<"Name :"<<it.first<<std::endl;
        for(auto order:it.second)
        {
            std::cout<< "ID: " << order.id << " Symbol: " << order.symbol
                      << " Price: " << order.price << " Quantity: " << order.quantity << "\n";
        }
    }

    //Printing Sell order book
    std::cout<<"\n Sell Orders : \n";
    for(auto it:sellOrders)
    {
        std::cout<<"Name :"<<it.first<<std::endl;
        for(auto order:it.second)
        {
            std::cout<< "ID: " << order.id << " Symbol: " << order.symbol
                      << " Price: " << order.price << " Quantity: " << order.quantity << "\n";
        }
    }

}