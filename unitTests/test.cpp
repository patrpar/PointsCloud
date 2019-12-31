#include "pch.h"
#include <example/BankAccount.h>


TEST(TestCaseName, givenAWhenBThenSuccessIsReturned) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

TEST(bankAccountTest, BankAccountStartEmpy)
{
    BankAccount bankAccount;
    EXPECT_EQ(0, bankAccount.balance);
}


struct BankAccountFixture : testing::Test {

    std::unique_ptr<BankAccount> bankAccount;
    int startValue = 100;


    BankAccountFixture()
    {
        bankAccount = std::make_unique<BankAccount>(startValue);
    }

    ~BankAccountFixture(){}

};


using BankAccountTest = BankAccountFixture;


TEST_F(BankAccountTest, givenDefaultvalueWhenBankAccountIsContructedThenCorrectValueIsSet)
{
    EXPECT_NE(0, bankAccount->balance);
    int moneyBegin = bankAccount->balance;

    int extraMoney = 200;
    bankAccount->addMoney(extraMoney);

    EXPECT_EQ(moneyBegin + extraMoney, bankAccount->balance);
}

struct account_state
{
    int initial_balance;
    int extra_money;
    int final_balance;
};


struct BankAccountParams : BankAccountFixture, testing::WithParamInterface<account_state>
{
    BankAccountParams() {
        bankAccount->balance = GetParam().initial_balance;
    }
};

using BankAccountParamsTest = BankAccountParams;

TEST_P(BankAccountParamsTest, givenSomeValuesWhenAddMoneyIsCalledThenFinalBalanceIsCorrect)
{
    auto params = GetParam();
    bankAccount->addMoney(params.extra_money);

    EXPECT_EQ(params.final_balance, bankAccount->balance);

}

INSTANTIATE_TEST_CASE_P(Default, BankAccountParamsTest,
    testing::Values(
        account_state{ 200,400,600 },
        account_state{ 100,100,200 },
        account_state{ 60,120,180 }
    )
);


