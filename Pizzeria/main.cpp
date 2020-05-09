//
//  main.cpp
//  Pizzeria
//
//  Created by Eduard Dzhumagaliev on 4/11/20.
//  Copyright © 2020 Eduard Dzhumagaliev. All rights reserved.
//

#include <iostream>
#include <vector>
#include <numeric>
#include <map>

using namespace std;

struct Ingredient {
    string name;
    size_t cost;
    
    friend bool operator<(const Ingredient & lhs, const Ingredient & rhs) {
        return lhs.cost < rhs.cost;
    };
};

class Pizza {
public:
    enum Size {
        small,
        medium,
        large,
        xlarge
    };
        
protected:
    map<Ingredient, size_t> ingredients;
    Size size;
    
    Pizza(Size size): size(size) {}; // Finally we've found the application for protected constructors
                                     // that is, if we want to implement the constructor, but want the
                                     // class to stay semi-abstract, so that it can't be initialized directly
                                     // even though it has logics for this
    
public:
    void add(Ingredient & ingredient) {
        ingredients[ingredient] += 1;
    };
    
    string getIngredientsList() {
        string ingredientsList;
        for (auto [ingredient, quantity]: ingredients) {
            ingredientsList += ingredient.name + ": " + to_string(quantity) + " / ";
        }
        return ingredientsList;
    };
    
    string getSize() {
        switch (size) {
            case small:
                return "SM";
                break;
            case medium:
                return "MD";
                break;
            case large:
                return "LG";
                break;
            case xlarge:
                return "XL";
                break;
                
            default:
                return "UNKNOWN";
                break;
        }
    }
    
    string getPizzaDetails() {
        return getSize() + " / " + getIngredientsList() + to_string(getCost());
    };
    
    virtual string getDescription() = 0;
    virtual size_t getBaseCost() = 0;
    virtual double getCostMultiplier() = 0;
    
    size_t getCost() {
        size_t baseCost = getBaseCost();
        size_t ingredientsCost = getCostMultiplier() * reduce(ingredients.begin(), ingredients.end(),
                                                              0, [](size_t lhs, pair<Ingredient, size_t> rhs) -> size_t {
            return lhs + rhs.first.cost * rhs.second;
        });
        return baseCost * (size + 1) + ingredientsCost;
    };
};

class VeganPizza: public Pizza {
public:
    VeganPizza(Size size): Pizza(size) {};
    
    string getDescription() override {
        return "Vegan Pizza 🥦 / " + getPizzaDetails();
    };
    
    size_t getBaseCost() override {
        return 350;
    };
    
    double getCostMultiplier() override {
        return 0.8;
    };
};

class HawaiianPizza: public Pizza {
public:
    HawaiianPizza(Size size): Pizza(size) {};
    
    string getDescription() override {
        return "Hawaiian Pizza 🍍 / " + getPizzaDetails();
    };
    
    size_t getBaseCost() override {
        return 450;
    };
    
    double getCostMultiplier() override {
        return 1.2;
    };
};

class PepperoniPizza: public Pizza {
public:
    PepperoniPizza(Size size): Pizza(size) {};
    
    string getDescription() override {
        return "Pepperoni Pizza 🍕 / " + getPizzaDetails();
    };
    
    size_t getBaseCost() override {
        return 480;
    };
    
    double getCostMultiplier() override {
        return 1.5;
    };
};

class PizzaBuilder {
protected:
    Pizza * product;
    
public:
    virtual void with(Ingredient & ingredient, size_t times) = 0;
    virtual void with(Ingredient & ingredient) = 0;
    
    Pizza * getProduct() {
        return product;
    };
};

class VeganBuilder: public PizzaBuilder {
public:
    VeganBuilder(Pizza::Size size) {
        product = new VeganPizza(size);
    };
    
    void with(Ingredient &ingredient, size_t times) override {
        for (size_t i = 0; i < times; i++) product->add(ingredient);
    };
    void with(Ingredient &ingredient) override {
        with(ingredient, 1);
    };
};

class HawaiianBuilder: public PizzaBuilder {
public:
    HawaiianBuilder(Pizza::Size size) {
        product = new HawaiianPizza(size);
    };
    
    void with(Ingredient &ingredient, size_t times) override {
        for (size_t i = 0; i < times; i++) product->add(ingredient);
    };
    void with(Ingredient &ingredient) override {
        with(ingredient, 1);
    };
};

class PepperoniBuilder: public PizzaBuilder {
public:
    PepperoniBuilder(Pizza::Size size) {
        product = new PepperoniPizza(size);
    };
    
    void with(Ingredient &ingredient, size_t times) override {
        for (size_t i = 0; i < times; i++) product->add(ingredient);
    };
    void with(Ingredient &ingredient) override {
        with(ingredient, 1);
    };
};

class Order {
private:
    vector<Ingredient> availableIngredients;
    vector<Pizza*> pizzaListing;
    
    Pizza::Size inputPizzaSize() {
        cout << "Choose your pizza size: " << endl;
        while (true) {
            cout << "0: Small (25 cm)" << endl;
            cout << "1: Medium (30 cm)" << endl;
            cout << "2: Large (35 cm)" << endl;
            cout << "3: XLarge (40 cm)" << endl;
            uint choice;
            cin >> choice;
            if (choice >= Pizza::Size::small && choice <= Pizza::Size::xlarge) return (Pizza::Size)choice;
        }
    };
    
    // key, value : ingredient, quantity
    typedef map<size_t, size_t> IngredientsChoice;
    IngredientsChoice inputIngredients() {
        cout << "### Choose Ingredients ###" << endl;
        IngredientsChoice ingredientsChoice;
        while(true) {
            if (!ingredientsChoice.empty()) {
                cout << "#### Chosen ingredients ####" << endl;
                for (auto [key, quantity]: ingredientsChoice) {
                    cout << availableIngredients[key].name << " (" << quantity << ") " << endl;
                };
            }
            
            cout << "#### Available Ingredients ####" << endl;
            for (size_t i = 0; i < availableIngredients.size(); i++) {
                cout << i << ": " << availableIngredients[i].name << " (" << availableIngredients[i].cost << ") " << endl;
            }
            cout << "Type `-1` to exit the ingredients dialog" << endl;
            
            int choice;
            cin >> choice;
            
            if (choice >= 0 && choice < availableIngredients.size()) {
                ingredientsChoice[choice] += 1;
            } else if (choice == -1) {
                return ingredientsChoice;
            }
        }
    };
    
    void printOrder() {
        if (!pizzaListing.empty()) {
            cout << "## Your order ##" << endl;
            for (auto pizza: pizzaListing) {
                cout << pizza->getDescription() << endl;;
            }
        }
    };
    
    void printTotal() {
        if (!pizzaListing.empty()) {
            cout << "$$ ORDER TOTAL $$" << endl;
            size_t total = 0;
            for (auto pizza: pizzaListing) {
                total += pizza->getCost();
            }
            cout << total << " USD + " << total * 0.05 << " cashback 🤫" << endl;
        }
    }
    
    void orderVeganPizza() {
        cout << "## Order Vegan Pizza ##" << endl;
        auto pizzaSize = inputPizzaSize();
        auto veganBuilder = VeganBuilder(pizzaSize);
        auto ingredientsChoice = inputIngredients();
        for (auto [key, quantity]: ingredientsChoice) {
            veganBuilder.with(availableIngredients[key], quantity);
        }
        pizzaListing.push_back(veganBuilder.getProduct());
    };
    
    void orderHaiwaiianPizza() {
        cout << "## Order Hawaiian Pizza ##" << endl;
        auto pizzaSize = inputPizzaSize();
        auto hawaiianBuilder = HawaiianBuilder(pizzaSize);
        auto ingredientsChoice = inputIngredients();
        for (auto [key, quantity]: ingredientsChoice) {
            hawaiianBuilder.with(availableIngredients[key], quantity);
        }
        pizzaListing.push_back(hawaiianBuilder.getProduct());
    };
    
    void orderPepperoniPizza() {
        cout << "## Order Pepperoni Pizza ##" << endl;
        auto pizzaSize = inputPizzaSize();
        auto pepperoniBuilder = PepperoniBuilder(pizzaSize);
        auto ingredientsChoice = inputIngredients();
        for (auto [key, quantity]: ingredientsChoice) {
            pepperoniBuilder.with(availableIngredients[key], quantity);
        }
        pizzaListing.push_back(pepperoniBuilder.getProduct());
    };
    
    bool orderPizza() {
        cout << "1/2/3/-1: Vegan/Hawaiian/Pepperoni Pizza/quit" << endl;
        int choice;
        cin >> choice;
        if (choice == -1) {
            return false;
        } else {
            // Should do this OOP way (so that this is scalable)
            // But builders get in the way 🤫
            switch (choice) {
                case 1:
                    orderVeganPizza();
                    break;
                case 2:
                    orderHaiwaiianPizza();
                    break;
                case 3:
                    orderPepperoniPizza();
                    break;
                    
                default:
                    break;
            }
            return true;
        }
    }
    
public:

    void startOrderLoop() {
        cout << "# Pizza Order #" << endl;
        bool needsOrder = true;
        while (needsOrder) {
            printOrder();
            needsOrder = orderPizza();
        }
        printTotal();
    };
    
    Order() {
        auto salt = (Ingredient){.name = "Salt", .cost = 5};
        auto pepper = (Ingredient){.name = "Pepper", .cost = 3};
        availableIngredients.push_back(salt);
        availableIngredients.push_back(pepper);
    }
};

int main(int argc, const char * argv[]) {
    Order order;
    order.startOrderLoop();
    
    return 0;
}
