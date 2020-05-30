#include <algorithm>
#include <iostream>
#include <list>
#include <string>
#include <sstream>
#include <map>
#include "../common/common.h"

class AccountNumber {
private:
    int val {};
public:
    AccountNumber (void) { }
    AccountNumber (int val) : val(val) { }
    bool operator< (const AccountNumber& rhs) const {
        return (val < rhs.val); 
    }
    std::string to_string (void) const {
        return "AccountNumber(" + std::to_string(val) + ")";
    }
    friend std::ostream& operator<<(std::ostream &os, const AccountNumber& o) {
        os << o.to_string();
        return os;
    }
};

template<class T> class BankAccount;

template<class T> class BankAccount {
private:
    const T no_cash {};
    T cash {};
public:
    BankAccount<T> () {
        std::cout << "default constructor " << to_string() << std::endl;
    }
    BankAccount<T> (T cash) : cash (cash) {
        std::cout << "new cash " << to_string() << std::endl;
    }
    BankAccount<T> (const BankAccount& o) {
        std::cout << "copy cash constructor called for " << o.to_string() << std::endl;
        cash = o.cash;
        std::cout << "copy cash constructor result is  " << to_string() << std::endl;
    }
    // Transfer of funds?
    BankAccount<T> (BankAccount<T>&& o) {
        std::cout << "move cash called for " << o.to_string() << std::endl;
        cash = o.cash;
        o.cash = no_cash;
        std::cout << "move cash result is  " << to_string() << std::endl;
    }
    ~BankAccount<T> () {
        std::cout << "delete account " << to_string() << std::endl;
    }
    void deposit (const T& deposit) {
        cash += deposit;
        std::cout << "deposit cash called " << to_string() << std::endl;
    }
    using CheckTransactionCallback = std::function<void(T)>;
    int check_transaction (int cash, CheckTransactionCallback fn) {
        if (cash < 100) {
            throw std::string("transaction is too small for Mr Money Bags");
        } else {
            fn(cash);
        }
        return cash;
    }
    T balance (void) const {
        return cash;
    }
    bool check_balance (T expected) const {
        if (cash == expected) {
            return true;
        } else {
            throw std::string("account has different funds " +
                              to_string() + " than expected " +
                              std::to_string(expected));
        }
    }
    friend std::ostream& operator<<(std::ostream &os, const BankAccount<T>& o) {
        os << "$" << std::to_string(o.cash);
        return os;
    }
    std::string to_string (void) const {
        auto address = static_cast<const void*>(this);
        std::stringstream ss;
        ss << address;
        return "BankAccount(" + ss.str() + ", cash $" + std::to_string(cash) + ")";
    }
};

int main(int, char**)
{
    DOC("Create a std::map of AccountNumber -> Account");
    using Account = BankAccount<int>;
    using Bank = std::map< AccountNumber, Account >;
    Bank thebank;

    //
    // We're going to be fancy here and create a lambda we can use
    // to print the accounts.
    //
    auto show_all_bank_accounts = ([](const Bank &b)
        {
            DOC("Show all bank accounts");
            for (auto const &acc : b) {
                std::cout << acc.first << " " << acc.second << std::endl;
            }
        } );

    DOC("Create some accounts");
    AccountNumber account1(101);
    Account       balance1(10000);

    AccountNumber account2(102);
    Account       balance2(20000);

    AccountNumber account3(104);
    Account       balance3(30000);

    DOC("Add some bank accounts");
    thebank.insert(std::make_tuple(account1, balance1));
    thebank.insert(std::make_tuple(account2, balance2));
    show_all_bank_accounts(thebank);

    DOC("Does account1 exist?");
    if (thebank.find(account1) == thebank.end()) {
        DOC("No");
    } else {
        DOC("Yes");
    }

    DOC("Does account2 exist?");
    if (thebank.find(account2) == thebank.end()) {
        DOC("No");
    } else {
        DOC("Yes");
    }

    DOC("Does account3 exist?");
    if (thebank.find(account3) == thebank.end()) {
        DOC("No. Add it quick!");
        thebank.insert(std::make_tuple(account3, balance3));
    } else {
        DOC("Yes");
    }
    show_all_bank_accounts(thebank);

    DOC("Remove account2");
    thebank.erase(account2);
    show_all_bank_accounts(thebank);

    DOC("Modify account3");
    thebank[account3].deposit(100);
    show_all_bank_accounts(thebank);

    DOC("Close the bank");
    thebank.clear();

    DOC("End");
}
