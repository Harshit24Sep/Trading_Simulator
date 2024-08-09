#include<string>

enum class OrderType {Market, Limit};

class Order
{
    public:
        bool isBuy;
        OrderType type;
        double price;
        std::string symbol;
        int quantity;
        int id;

        Order() 
        : isBuy(false), type(OrderType::Limit), symbol(""), id(0), quantity(0), price(0.0) {}

        // constructor for Limit type order
        Order(bool isBuy, std::string symbol, int id, int quantity, double price)
        : isBuy(isBuy), type(OrderType::Limit), symbol(symbol), id(id), quantity(quantity), price(price) {}

        // constructor for market type order
        Order(bool isBuy, std::string symbol, int id, int quantity)
        : isBuy(isBuy), type(OrderType::Market), symbol(symbol), id(id), quantity(quantity) {}
};


