#pragma once

class BankAccount
{
public:
    int balance = 0;
    BankAccount() {}
    BankAccount(int _balance):balance(_balance){}

    void addMoney(int money) {
        balance += money;
    }
};
