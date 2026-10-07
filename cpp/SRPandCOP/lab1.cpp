#include <iostream>
#include <string>
//PAYMENT SYSTEM
//Сущности: Клиент, Счет, Кредитная карта (КК), Заказ, Администратор.
//Дей-я и инкапсуляция:
// - Клиент: оплатить заказ, сделать перевод на другой счет;
// - Администратор: заблокировать КК;
// - КК: списать деньги, может быть заблокированной;
// - Счет: списать деньги;
// - Заказ: оплатить;
//ОСР: "оплатить Заказ" != "сделать платеж на другой Счет" -> Требуется гибкость при добавлении разныз видов оплат.
class Order;
class IPaymentMethod; 
class BankAccount;

class Client{
    private:
    std::string name;

    public:
    static int total_clients; //МЕСТО ВЫДЕЛИТЬ НЕ ЗАБУДЬ ПЖ

    Client() : name("Client_"+std::to_string(total_clients)) {
        total_clients++;
    }
    Client(std::string nameClient) : name(nameClient) {
        total_clients++;
    }

    std::string getName() const { return name; }
}
