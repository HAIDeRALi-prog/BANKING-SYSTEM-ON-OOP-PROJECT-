#include <iostream>
#include <fstream>
#include <string>
using namespace std;

const int MAX_ACCOUNTS = 100;
const double INTEREST_RATE = 0.05; 

// Input validation part
int getValidatedInt(string prompt)
{
    int val;
    while (true)
    {
        cout << prompt;
        cin >> val;
        if (!cin.fail()) break;
        cout << "Invalid input. Try again.\n";
        cin.clear();
        cin.ignore(1000, '\n');
    }
    return val;
}

double getValidatedDouble(string prompt)
{
    double val;
    while (true) 
    {
        cout << prompt;
        cin >> val;
        if (!cin.fail() && val >= 0) break;
        cout << "Invalid input. Try again.\n";
        cin.clear();
        cin.ignore(1000, '\n');
    }
    return val;
}

// Abstract base class
class Account
{
public:
    virtual void createAccount(int accNum, string holder, double bal) = 0;
    virtual int getAccountNumber() const = 0;
    virtual string getHolder() const = 0;
    virtual void setHolder(string holder) = 0;
    virtual void setBalance(double bal) = 0;
    virtual void display() const = 0;
    virtual ~Account() {}
};

// Derived class 
class BankAccount : public Account 
{
private:
    int accountNumber;
    string accountHolder;
    double balance;

public:
    BankAccount() 
    {
        accountNumber = 0;
        accountHolder = "";
        balance = 0.0;
    }

    static void showWelcomeMessage()   // <--- Static member function added
    {
        cout << "Welcome to Bank Account System\n";
    }

    void createAccount(int accNum, string holder, double bal) override
    {
        accountNumber = accNum;
        accountHolder = holder;
        balance = bal;
    }

    int getAccountNumber() const override 
    {
        return accountNumber;
    }

    string getHolder() const override 
    {
        return accountHolder;
    }

    void setHolder(string holder) override
    {
        accountHolder = holder;
    }

    void setBalance(double bal) override 
    {
        balance = bal;
    }

    double getBalance() const 
    {
        return balance;
    }

    void display() const override 
    {
        cout << "Account Number: " << accountNumber << endl;
        cout << "Holder Name: " << accountHolder << endl;
        cout << "Balance: $" << balance << endl;
    }

    BankAccount operator+(double amount)
    {
        BankAccount temp = *this;
        temp.balance += amount;
        return temp;
    }

    BankAccount operator-(double amount) 
    {
        BankAccount temp = *this;
        if (amount > temp.balance) 
        {
            cout << "Insufficient balance.\n";
        } else 
        {
            temp.balance -= amount;
        }
        return temp;
    }

    friend void printSummary(const BankAccount& acc);

    void saveToFile(ofstream& out) 
    {
        out << accountNumber << "," << accountHolder << "," << balance << "\n";
    }

    void loadFromFile(int accNum, string holder, double bal) 
    {
        accountNumber = accNum;
        accountHolder = holder;
        balance = bal;
    }
};

void printSummary(const BankAccount& acc)
{
    cout << "\n--- Account Summary ---\n";
    cout << "Account: " << acc.accountNumber << "\n";
    cout << "Holder: " << acc.accountHolder << "\n";
    cout << "Balance: $" << acc.balance << "\n";
    cout << "------------------------\n";
}

BankAccount* accounts[MAX_ACCOUNTS];
int accountCount = 0;

void saveAccountsToFile()
{
    ofstream out("accounts.txt");
    for (int i = 0; i < accountCount; ++i) 
    {
        accounts[i]->saveToFile(out);
    }
    out.close();
}

void loadAccountsFromFile() 
{
    ifstream in("accounts.txt");
    accountCount = 0;
    int accNum;
    string name;
    double bal;
    string line;
    while (getline(in, line))
    {
        size_t pos1 = line.find(',');
        size_t pos2 = line.rfind(',');
        if (pos1 == string::npos || pos2 == string::npos || pos1 == pos2) continue;
        accNum = stoi(line.substr(0, pos1));
        name = line.substr(pos1 + 1, pos2 - pos1 - 1);
        bal = stod(line.substr(pos2 + 1));
        accounts[accountCount] = new BankAccount();
        accounts[accountCount]->createAccount(accNum, name, bal);
        accountCount++;
    }
    in.close();
}

// === Simple transaction logging ===
void logTransaction(int accNum, const string& type, double amount)
{
    ofstream out("transactions.txt", ios::app);
    if (!out) return;
    out << accNum << "," << type << "," << amount << "\n";
    out.close();
}

void viewTransactionHistory(int accNum)
{
    ifstream in("transactions.txt");
    if (!in) {
        cout << "No transactions found.\n";
        return;
    }
    cout << "\n--- Transaction History for Account #" << accNum << " ---\n";
    string line;
    bool found = false;
    while (getline(in, line)) {
        size_t pos1 = line.find(',');
        if (pos1 == string::npos) continue;
        int acc = stoi(line.substr(0, pos1));
        if (acc == accNum) {
            string rest = line.substr(pos1 + 1);
            cout << rest << "\n";
            found = true;
        }
    }
    if (!found) cout << "No transactions for this account.\n";
    cout << "-----------------------------------------\n";
    in.close();
}
// === End transaction logging ===

void adminMenu()
{
    int choice;
    do 
    {
        cout << "\n--- Admin Menu ---\n";
        cout << "1. Create Account" << endl;
        cout << "2. View All Accounts" << endl;
        cout << "3. Search by Account Number" << endl;
        cout << "4. Update Account" << endl;
        cout << "5. Delete Account" << endl;
        cout << "6. Back to Main Menu" << endl;
        choice = getValidatedInt("Enter choice: ");

        if (choice == 1) 
        {
            int acc = getValidatedInt("Enter Account Number: ");
            string name;
            cout << "Enter Holder Name: ";
            cin >> ws;
            getline(cin, name);
            double bal = getValidatedDouble("Enter Initial Balance: ");
            accounts[accountCount] = new BankAccount();
            accounts[accountCount]->createAccount(acc, name, bal);
            accountCount++;
            saveAccountsToFile();
            logTransaction(acc, "Account Created", bal);  // Log creation
            cout << "Account created successfully.\n";
        }
        else if (choice == 2) 
        {
            for (int i = 0; i < accountCount; ++i) accounts[i]->display();
        }
        else if (choice == 3)
        {
            int num = getValidatedInt("Enter Account Number to Search: ");
            bool found = false;
            for (int i = 0; i < accountCount; ++i) 
            {
                if (accounts[i]->getAccountNumber() == num) 
                {
                    accounts[i]->display();
                    found = true;
                }
            }
            if (!found) cout << "Account not found.\n";
        }
        else if (choice == 4) 
        {
            int num = getValidatedInt("Enter Account Number to Update: ");
            bool updated = false;
            for (int i = 0; i < accountCount; ++i) 
            {
                if (accounts[i]->getAccountNumber() == num) 
                {
                    string name;
                    cout << "Enter new name: ";
                    cin >> ws;
                    getline(cin, name);
                    accounts[i]->setHolder(name);
                    saveAccountsToFile();
                    cout << "Account updated.\n";
                    updated = true;
                    break;
                }
            }
            if (!updated) cout << "Account not found.\n";
        }
        else if (choice == 5) 
        {
            int num = getValidatedInt("Enter Account Number to Delete: ");
            bool deleted = false;
            
            for (int i = 0; i < accountCount; ++i) 
            {
                if (accounts[i]->getAccountNumber() == num) 
                {
                    delete accounts[i];
                    for (int j = i; j < accountCount - 1; ++j) 
                    {
                        accounts[j] = accounts[j + 1];
                    }
                    accountCount--;
                    saveAccountsToFile();
                    cout << "Account deleted.\n";
                    deleted = true;
                    break;
                }
            }
            if (!deleted) cout << "Account not found.\n";
        }
    }
    while (choice != 6);
}

void userMenu() 
{
    int num = getValidatedInt("Enter your Account Number: ");
    for (int i = 0; i < accountCount; ++i)
    {
        if (accounts[i]->getAccountNumber() == num) 
        {
            BankAccount* acc = dynamic_cast<BankAccount*>(accounts[i]);
            int choice;
            do 
            {
                cout << "\n--- User Menu ---\n";
                cout << "1. View Account" << endl;
                cout << "2. Deposit" << endl;
                cout << "3. Withdraw" << endl;
                cout << "4. View Summary" << endl;
                cout << "5. Apply Interest" << endl;
                cout << "6. View Transaction History" << endl; 
                cout << "7. Exit" << endl;
                choice = getValidatedInt("Enter choice: ");
                if (choice == 1) 
                {
                    acc->display();
                }
                else if (choice == 2) 
                {
                    double amt = getValidatedDouble("Enter amount to deposit: ");
                    *acc = *acc + amt;
                    logTransaction(num, "Deposit", amt);
                    saveAccountsToFile();
                }
                else if (choice == 3) 
                {
                    double amt = getValidatedDouble("Enter amount to withdraw: ");
                    if (amt <= acc->getBalance()) {
                        *acc = *acc - amt;
                        logTransaction(num, "Withdraw", amt);
                        saveAccountsToFile();
                    }
                }
                else if (choice == 4) 
                {
                    printSummary(*acc);
                }
                else if (choice == 5)
                {   // Apply interest
                    double interest = acc->getBalance() * INTEREST_RATE;
                    *acc = *acc + interest;
                    cout << "Interest applied at " << (INTEREST_RATE * 100) << "%.\n";
                    logTransaction(num, "Interest Applied", interest);
                    saveAccountsToFile();
                }
                else if (choice == 6) 
                {
                    viewTransactionHistory(num);
                }
            } 
            while (choice != 7);
            return;
        }
    }
    cout << "Account not found.\n";
}

bool authenticate(string role)
{
    string user, pass;
    cout << "Enter username: "; cin >> user;
    cout << "Enter password: "; cin >> pass;

    if (role == "admin") 
    {
        if (user == "Zain" && pass == "4365") 
        {
            return true;
        }
    } 
    else if (role == "user")
    {
        if ((user == "Haider" && pass == "4384") ||
            (user == "Rehman" && pass == "4351") ||
            (user == "Ehsan" && pass == "4358")) 
            {
            return true;
        }
    }

    cout << "Ivalid credentials.\n";
    return false;
}

int main()
{
    BankAccount::showWelcomeMessage();  // Call the static member function
    loadAccountsFromFile();
    int choice;
    do 
    {
        cout << "\n=== Online Banking System ===\n";
        cout << "1. Admin Login" << endl;
        cout << "2. User Login" << endl;
        cout << "3. Exit" << endl;

        choice = getValidatedInt("Enter your choice: ");
        if (choice == 1 && authenticate("admin")) adminMenu();
        else if (choice == 2 && authenticate("user")) userMenu();
    }
    while (choice != 3);

    for (int i = 0; i < accountCount; ++i) delete accounts[i];
    cout << "\nThank you for using the system.\n";
    return 0;
}
