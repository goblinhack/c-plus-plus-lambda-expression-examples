Modern C++ (C++11 ... C++20) examples
=====================================

Are you looking for a collection of short snippets of modern C++ code
covering core features in C++11 and beyond? Of course you are!

As I'm constantly learning C++ myself, I wanted to create some simple examples 
of each of the main features of the language. You can build each example
individually or as a whole.

Let me know if this is useful to anyone or if there are any areas you want
covered. This is (probably forever) a work in progress.

[How to use range based for loops](range_based_for_loop/README.md)

[How to use range based for loops with your own iterator](range_based_for_loop_custom_begin_end/README.md)

[How to use std::bind with simple example](std_bind/README.md)

[How to use std::bind with methods or proxy functions](std_bind_with_a_method/README.md)

[How to use std::bind with callback inside a class](std_bind_with_a_class_callback/README.md)

[How to use std::move to avoid the cost of temporaries](std_move/README.md)

[How to use std::forward to invoke the exactly correct function through a template](std_forward/README.md)

[How to create and use std::unique_ptr](std_unique_ptr/README.md)

[How to create and use std::shared_ptr](std_shared_ptr/README.md)

[How to make a wrapper around std::shared_ptr](std_shared_ptr_wrapper/README.md)

To build all the examples, just do:
```C++
   make 
```
To build all the examples and the documentation and run every examle, do:
```C++
   sh ./RUNME
```
How to use a range based for loop
=================================

A nice extension in C++ 11 is range based for loops. They are both
readable and safer as you do not need to manually specify the boundary
conditions.

There are a few ways to walk such containers. One is via a constant iterator e.g.:

By now you will have seen the following range based for loop e.g.:
```C++
    for (auto const i : container) { }
```
Another, if you wish to modify the data as you go is:
```C++
    for (auto &i : container) { }
```
However it may be 'simpler' to always do the following and use a forward reference:
```C++
    for (auto &&i : container) { }
```
This handles corner cases for things like vectors of bitfields that you cannot have
a reference to. Here is such a corner case. The iterator provided is actually a thing
called a proxy iterator, vector<bool>::reference. So, this will not compile:
```C++
    #include <vector>
    int main() {
        std::vector<bool> v(8);
        for (auto& e : v) { e = true; }
    }
```
But this will!
```C++
    #include <vector>
    int main() {
        std::vector<bool> v(8);
        for (auto&& e : v) { e = true; }
    }
```
Here is a full example:
```C++
#include <iostream>
#include <algorithm>
#include <vector>

int main() {
    // Create a vector of strings:
    std::initializer_list< std::string > init1 = {"elem1", "elem1"};
    std::vector< std::string > vec1(init1);

    // Range based for loop iterator with a const:
    for (const auto i : vec1) {
        std::cout << "vec1: walk " << i << std::endl;
    }

    // Range based for loop iterator with a modifiable reference:
    for (auto &i : vec1) {
        i += "+ stuff";
        std::cout << "vec1: walk " << i << std::endl;
    }

    // Range based for loop iterator with forward reference:
    for (auto &&i : vec1) {
        i += "+ more stuff";
        std::cout << "vec1: walk " << i << std::endl;
    }
}
```
To build:
<pre>
cd range_based_for_loop
rm -f *.o example
c++ -std=c++2a -Werror -g -ggdb3 -Wall -c -o main.o main.cpp
c++ main.o  -o example
./example
</pre>
Expected output:
<pre>

# Create a vector of strings:

# Range based for loop iterator with a const:
vec1: walk elem1
vec1: walk elem1

# Range based for loop iterator with a modifiable reference:
vec1: walk elem1+ stuff
vec1: walk elem1+ stuff

# Range based for loop iterator with forward reference:
vec1: walk elem1+ stuff+ more stuff
vec1: walk elem1+ stuff+ more stuff
</pre>
How to use a range based for loop with your own class iterator
==============================================================

By now you will have seen the following range based for loop e.g.:
```C++
    for (i : container) { }
```
Now it would be nice to apply that to our own classes too. To do this,
you need to define something that can hold where we are in the processing
loop, called an iterator e.g.:
```C++
    class Iterator {
    public:
        Iterator(T* ptr) : ptr(ptr) {}
        Iterator operator++() { ++ptr; return *this; }
        bool operator!= (const Iterator& o) const {
            return ptr != o.ptr;
        }
        const T& operator*() const { return *ptr; }
   private:
       T* ptr;
   };
public:
   Iterator begin() const { return Iterator(data); }
   Iterator end() const { return Iterator(data + len); }
```
Notice that the iterator has some private data that we increment as
we walk.

Here is a full example:
```C++
#include <iostream>
#include <sstream>
#include <algorithm>

template<class T> class MyVector {
private:
    T *data;
    size_t maxlen;
    size_t currlen;
public:
    MyVector<T> () : data (nullptr), maxlen(0), currlen(0) {
        std::cout << "default constructor " << to_string() << std::endl;
    }
    MyVector<T> (int maxlen) : data (new T [maxlen]),
                               maxlen(maxlen),
                               currlen(0) {
        std::cout << "new " << to_string() << std::endl;
    }
    MyVector<T> (const MyVector& o) {
        std::cout << "copy constructor called for " << o.to_string() << std::endl;
        data = new T [o.maxlen];
        maxlen = o.maxlen;
        currlen = o.currlen;
        std::copy(o.data, o.data + o.maxlen, data);
        std::cout << "copy constructor result is  " << to_string() << std::endl;
    }
    MyVector<T> (MyVector<T>&& o) {
        std::cout << "std::move called for " << o.to_string() << std::endl;
        data = o.data;
        maxlen = o.maxlen;
        currlen = o.currlen;
        o.data = nullptr;
        o.maxlen = 0;
        o.currlen = 0;
        std::cout << "std::move result is  " << to_string() << std::endl;
    }
    ~MyVector<T> () {
        std::cout << "delete " << to_string() << std::endl;
    }
    void push_back (const T& i) {
        if (currlen >= maxlen) {
            maxlen *= 2;
            auto newdata = new T [maxlen];
            std::copy(data, data + currlen, newdata);
            if (data) {
                delete[] data;
            }
            data = newdata;
        }
        data[currlen++] = i;
        std::cout << "push_back called " << to_string() << std::endl;
    }
    friend std::ostream& operator<<(std::ostream &os, const MyVector<T>& o) {
        auto s = o.data;
        auto e = o.data + o.currlen;;
        while (s < e) {
            os << "[" << *s << "]";
            s++;
        }
        return os;
    }
    std::string to_string (void) const {
        auto address = static_cast<const void*>(this);
        std::stringstream ss;
        ss << address;

        std::string elems;
        auto s = data;
        auto e = data + currlen;;
        while (s < e) {
            elems += std::to_string(*s);
            s++;
            if (s < e) {
                elems += ",";
            }
        }

        return "MyVector(" + ss.str() +
                         ", currlen=" + std::to_string(currlen) +
                         ", maxlen=" + std::to_string(maxlen) +
                         " elems=[" + elems + "])";
    }
    class Iterator {
    public:
        Iterator(T* ptr) : ptr(ptr) {}
        Iterator operator++() { ++ptr; return *this; }
        bool operator!= (const Iterator& o) const {
            return ptr != o.ptr;
        }
        const T& operator*() const { return *ptr; }
   private:
       T* ptr;
   };
public:
   Iterator begin() const {
       return Iterator(data);
   }
   Iterator end() const {
       return Iterator(data + currlen);
   }
};

int main() {
    // Create a custom vector class:
    auto vec1 = MyVector<int>(1);
    vec1.push_back(10);
    vec1.push_back(11);
    vec1.push_back(12);

    // Walk the custom vector with our iterator:
    for (auto i : vec1) {
        std::cout << "vec1: walk " << i << std::endl;
    }

    // End, expect vec1 destroy:
}
```
To build:
<pre>
cd range_based_for_loop_custom_begin_end
rm -f *.o example
c++ -std=c++2a -Werror -g -ggdb3 -Wall -c -o main.o main.cpp
c++ main.o  -o example
./example
</pre>
Expected output:
<pre>

# Create a custom vector class:
new MyVector(0x7ffee8d42730, currlen=0, maxlen=1 elems=[])
push_back called MyVector(0x7ffee8d42730, currlen=1, maxlen=1 elems=[10])
push_back called MyVector(0x7ffee8d42730, currlen=2, maxlen=2 elems=[10,11])
push_back called MyVector(0x7ffee8d42730, currlen=3, maxlen=4 elems=[10,11,12])

# Walk the custom vector with our iterator:
vec1: walk 10
vec1: walk 11
vec1: walk 12

# End, expect vec1 destroy:
delete MyVector(0x7ffee8d42730, currlen=3, maxlen=4 elems=[10,11,12])
</pre>
How to use std::bind, the basic stuff
=====================================

std::bind is a useful construct that allows you to associate some context
(i.e. a classes "this" pointer) with a function. It is primarily of use
with invoking callback functionaliy, but to get started let's focus on a
some simple cases:
```C++
    auto bind_fn = bind(bind_test, _1, _2, _3);
```
The funny syntax, _1 refers to an argument number is is fully called 
std::placeholders::_1 but for obvious reasons people prefer to use:
```C++
    using namespace std::placeholders; // for _1, _2, _3...
```
to make things readable. Once a function is bound, you can invoke it
like:
```C++
    bind_fn(1, 2, 3);
```
and bind_test with be called with (1, 2, 3). Nice.

It can also be used in std::for_each loops e.g.:
```C++
    static void do_stuff (const std::string& arg1) {
        std::cout << arg1 << std::endl;
    }

    std::list<std::string> l;
    auto bind_fn = bind(do_stuff, _1);
    std::for_each(l.begin(), l.end(), bind_fn);
```
And with std::transform (where you show intent to modify the list):
```C++
    static std::string do_stuff (std::string& arg1) {
        return arg1 + " is modified";
    }

    std::list<std::string> l;
    auto bind_fn = bind(do_stuff, _1);
    std::transform(l.begin(), l.end(), l.begin(), bind_fn);
```
Here is the full example:
```C++
#include <algorithm>
#include <functional> // for _1, _2
#include <iostream>
#include <list>
#include <string>
#include <utility>

using namespace std::placeholders; // for _1, _2, _3...

static void bind_fn1 (const std::string& arg1,
                      const std::string& arg2,
                      const std::string& arg3)
{
    std::cout << "argument 1 is " << arg1 << std::endl;
    std::cout << "argument 2 is " << arg2 << std::endl;
    std::cout << "argument 3 is " << arg3 << std::endl;
}

//
// You could cheat here and use &arg1, but that makes a mockery of 
// std::for_each
//
static void bind_try_to_modify_string (std::string arg1)
{
    arg1 = arg1 + " and is not modified and this will be ignored";
}

static std::string bind_really_modify_string (const std::string& arg1)
{
    return arg1 + " and is modified";
}

int main(int, char**)
{
    const std::string a = "cat";
    const std::string b = "dog";
    const std::string c = "ork";

    auto fn1 = bind(bind_fn1, _1, _2, _3);
    auto fn2 = bind(bind_fn1, _2, _1, _3);
    auto fn3 = bind(bind_fn1, _3, _1, _2);

    // call bind_fn1(_1, _2, _3)
    fn1(a, b, c);

    // call bind_fn1(_2, _1, _3)
    fn2(a, b, c);

    // call bind_fn1(_3, _2, _1)
    fn3(a, b, c);

    //
    // Use std::bind with std::for_each
    //
    std::initializer_list< std::string > il = { a, b, c };
    std::list< std::string > l1(il);

    // call our bind function via std::for_each to modify a list of strings
    auto fn4 = bind(bind_try_to_modify_string, _1);
    std::for_each(l1.begin(), l1.end(), fn4);
    for (auto e : l1) { std::cout << "l1 list-elem is now " << e << std::endl; }

    //
    // Use std::bind with std::transform
    //
    std::initializer_list< std::string > i2 = { a, b, c };
    std::list< std::string > l2(i2);

    // call our bind function via std::transform to modify a list of strings
    auto fn5 = bind(bind_really_modify_string, _1);
    std::transform(l2.begin(), l2.end(), l2.begin(), fn5);
    for (auto e : l2) { std::cout << "l2 list-elem is now " << e << std::endl; }
}
```
To build:
<pre>
cd std_bind
rm -f *.o example
c++ -std=c++2a -Werror -g -ggdb3 -Wall -c -o main.o main.cpp
c++ main.o  -o example
./example
</pre>
Expected output:
<pre>

# call bind_fn1(_1, _2, _3)
argument 1 is cat
argument 2 is dog
argument 3 is ork

# call bind_fn1(_2, _1, _3)
argument 1 is dog
argument 2 is cat
argument 3 is ork

# call bind_fn1(_3, _2, _1)
argument 1 is ork
argument 2 is cat
argument 3 is dog

# call our bind function via std::for_each to modify a list of strings
l1 list-elem is now cat
l1 list-elem is now dog
l1 list-elem is now ork

# call our bind function via std::transform to modify a list of strings
l2 list-elem is now cat and is modified
l2 list-elem is now dog and is modified
l2 list-elem is now ork and is modified
</pre>
How to use std::bind with methods or proxy functions
====================================================

Sometimes we want to be able to bind to a method so we can call it directly,
or more likely pass that function around. There are a few ways to do this.
One is via a proxy function like:
```C++
    void proxy_function (int cash, BankAccount<int> &b)
    {
        b.deposit(cash);
    }

    auto f1 = std::bind(proxy_function, _1, account1);
    f1(100);
```
Or without the proxy:
```C++
    auto f2 = std::bind(&BankAccount<int>::deposit, &account1, _1);
    f2(100);
```
Here is the full example:
```C++
#include <algorithm>  // for std::move
#include <functional> // for _1, _2
#include <iostream>
#include <memory>
#include <sstream>    // for std::stringstream
#include <string>
#include <utility>

using namespace std::placeholders;  // for _1, _2, _3...

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

void proxy_function (int cash, BankAccount<int> &b)
{
    b.deposit(cash);
}

int main(int, char**)
{
    try {
        // create account1 and try to deposit into it
        auto account1 = BankAccount<int>(0);

        // invoke a bind to a method via proxy
        auto f1 = std::bind(proxy_function, _1, account1);
        f1(100);

        //
        // std::ref is not normally needed unless it is something, like
        // say a bitfield that cannot have a direct reference and needs
        // to be wrapped by std::reference
        //
        // invoke a bind to a method via proxy with std::ref
        auto f2 = std::bind(proxy_function, _1, std::ref(account1));
        f2(100);

        // invoke a bind to a method directly
        auto f3 = std::bind(&BankAccount<int>::deposit, &account1, _1);
        f3(100);

        // check the deposits succeeded
        account1.check_balance(300);

        // end
    } catch (const std::string &e) {
        std::cerr << "FAILED: account1 deposit failed!: " << e << std::endl;
    }
}
```
To build:
<pre>
cd std_bind_with_a_method
rm -f *.o example
c++ -std=c++2a -Werror -g -ggdb3 -Wall -c -o main.o main.cpp
c++ main.o  -o example
./example
</pre>
Expected output:
<pre>

# create account1 and try to deposit into it
new cash BankAccount(0x7ffee2a90f68, cash $0)

# invoke a bind to a method via proxy
copy cash constructor called for BankAccount(0x7ffee2a90f68, cash $0)
copy cash constructor result is  BankAccount(0x7ffee2a90f60, cash $0)
deposit cash called BankAccount(0x7ffee2a90f60, cash $100)

# invoke a bind to a method via proxy with std::ref
deposit cash called BankAccount(0x7ffee2a90f68, cash $100)

# invoke a bind to a method directly
deposit cash called BankAccount(0x7ffee2a90f68, cash $200)

# check the deposits succeeded
delete account BankAccount(0x7ffee2a90f60, cash $100)
delete account BankAccount(0x7ffee2a90f68, cash $200)
# FAILED: account1 deposit failed!: account has different funds BankAccount(0x7ffee2a90f68, cash $200) than expected 300
</pre>
How to use std::bind with a class callback
==========================================

Sometimes we want to be able to invoke callbacks from our class. This can be
difficult in C++ as we need to have the context (i.e. the class object) somehow
associated with the function.

Thankfully, std::bind, can do this for us. Here we bind "account1" to one of
its methods.
```C++
    auto method = std::bind(&BankAccount<int>::deposit, &account1, _1);
    method(100);
```
This is identical to:
```C++
    account1.deposit(100);
```
So not too earth shattering. However, suppose we want to call a method in
our class and have it call another method within the same class (or potentially
another class) i.e. a callback.

Here we define the ability for "check_transaction" to accept a callack
function:
```C++
    using CheckTransactionCallback = std::function<void(T)>;
    int check_transaction (int cash, CheckTransactionCallback fn) {
        if (cash < 100) {
            throw std::string("transaction is too small for Mr Money Bags");
        } else {
            fn(cash);
        }
        return cash;
    }
```
And to invoke it:
```C++
    auto callback = std::bind(&BankAccount<int>::deposit, &account1, _1);
    account1.check_transaction(99, callback);
```
Here is the full example:
```C++
#include <algorithm>  // for std::move
#include <functional> // for _1, _2
#include <iostream>
#include <memory>
#include <sstream>    // for std::stringstream
#include <string>
#include <utility>

using namespace std::placeholders;  // for _1, _2, _3...

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
    try {
        // create account1 and try to deposit into it
        auto account1 = BankAccount<int>(0);

        auto deposit_method = std::bind(&BankAccount<int>::deposit, &account1, _1);
        account1.check_transaction(100, deposit_method);
        account1.check_transaction(100, deposit_method);
        account1.check_balance(200);
        std::cout << "SUCCESS: account1 1st deposit succeeded!" << std::endl;

        //
        // This will fail as we catch the 'small' transaction
        //
        account1.check_transaction(99, deposit_method);
        std::cout << "SUCCESS: account1 2nd deposit succeeded!" << std::endl;

        // end
    } catch (const std::string &e) {
        std::cerr << "FAILED: account1 deposit failed!: " << e << std::endl;
    }
}
```
To build:
<pre>
cd std_bind_with_a_class_callback
rm -f *.o example
c++ -std=c++2a -Werror -g -ggdb3 -Wall -c -o main.o main.cpp
c++ main.o  -o example
./example
</pre>
Expected output:
<pre>

# create account1 and try to deposit into it
new cash BankAccount(0x7ffeeda69540, cash $0)
deposit cash called BankAccount(0x7ffeeda69540, cash $100)
deposit cash called BankAccount(0x7ffeeda69540, cash $200)
# SUCCESS: account1 1st deposit succeeded!
delete account BankAccount(0x7ffeeda69540, cash $200)
# FAILED: account1 deposit failed!: transaction is too small for Mr Money Bags
</pre>
How to use std::move to avoid the cost of temporaries
=====================================================

std::move is a nifty way to avoid the use of temporaries when doing copies
of data. For example if you do:

```C++
template<class T> void simpleswap(T& a, T& b) {
    T tmp = b;
    b = a;
    a = b;
}
```

You will see the copy constructor invoked. Not a huge deal for small classes,
but if you are dealing with a vector, that could be very expensive.

A better approach is to use std::move e.g.

```C++
template<class T> void simpleswap(T& a, T& b) {
    T tmp { std::move(a) };
    a = std::move(b);
    b = std::move(tmp);
}
```

Now the question is, how do we implement std::move on our own classes.
To use this you will see a new form of constructor with the "&&" syntax.
The example below will create our own vector class and then implement
the move constructor to transfer elements across. The old vector will
be left empty.

Note that std::vector already does this approach; we're just applying
the same ideas to a custom class.
```C++
#include <iostream>
#include <string>
#include <sstream> // std::stringstream
#include <algorithm> // std::move

template<class T> class MyVector {
private:
    T *data;
    size_t maxlen;
    size_t currlen;
public:
    MyVector<T> () : data (nullptr), maxlen(0), currlen(0) {
        std::cout << "default constructor " << to_string() << std::endl;
    }
    MyVector<T> (int maxlen) : data (new T [maxlen]),
                               maxlen(maxlen),
                               currlen(0) {
        std::cout << "new " << to_string() << std::endl;
    }
    MyVector<T> (const MyVector& o) {
        std::cout << "copy constructor called for " << o.to_string() << std::endl;
        data = new T [o.maxlen];
        maxlen = o.maxlen;
        currlen = o.currlen;
        std::copy(o.data, o.data + o.maxlen, data);
        std::cout << "copy constructor result is  " << to_string() << std::endl;
    }
    MyVector<T> (MyVector<T>&& o) {
        std::cout << "std::move called for " << o.to_string() << std::endl;
        data = o.data;
        maxlen = o.maxlen;
        currlen = o.currlen;
        o.data = nullptr;
        o.maxlen = 0;
        o.currlen = 0;
        std::cout << "std::move result is  " << to_string() << std::endl;
    }
    ~MyVector<T> () {
        std::cout << "delete " << to_string() << std::endl;
    }
    void push_back (const T& i) {
        if (currlen >= maxlen) {
            maxlen *= 2;
            auto newdata = new T [maxlen];
            std::copy(data, data + currlen, newdata);
            if (data) {
                delete[] data;
            }
            data = newdata;
        }
        data[currlen++] = i;
        std::cout << "push_back called " << to_string() << std::endl;
    }
    friend std::ostream& operator<<(std::ostream &os, const MyVector<T>& o) {
        auto s = o.data;
        auto e = o.data + o.currlen;;
        while (s < e) {
            os << "[" << *s << "]";
            s++;
        }
        return os;
    }
    std::string to_string (void) const {
        auto address = static_cast<const void*>(this);
        std::stringstream ss;
        ss << address;

        std::string elems;
        auto s = data;
        auto e = data + currlen;;
        while (s < e) {
            elems += std::to_string(*s);
            s++;
            if (s < e) {
                elems += ",";
            }
        }

        return "MyVector(" + ss.str() +
                         ", currlen=" + std::to_string(currlen) +
                         ", maxlen=" + std::to_string(maxlen) +
                         " elems=[" + elems + "])";
    }
};

int main() {
    // Create a custom vector class:
    auto vec1 = new MyVector<int>(1);
    vec1->push_back(10);
    vec1->push_back(11);
    std::cout << "vec1: " << *vec1 << std::endl;

    // Create a new copy of vec1, vec2 via copy constructor (&):
    auto vec2 = *vec1;
    std::cout << "vec2: " << vec2 << std::endl;

    // Check we can append onto the copied vector:
    vec2.push_back(12);
    vec2.push_back(13);
    std::cout << "vec2: " << vec2 << std::endl;

    // Create a new vector from vec1, vec3 via the move constructor (&&):
    auto vec3 = std::move(*vec1);
    std::cout << "vec3: " << vec3 << std::endl;

    // Check we can append onto the std:move'd vector:
    vec3.push_back(14);
    vec3.push_back(15);
    std::cout << "vec3: " << vec3 << std::endl;

    // Destroy the old vector, vec1. It has no invalid elems:
    delete vec1;

    // End, expect vec2 and vec3 destroy:
}
```
To build:
<pre>
cd std_move
rm -f *.o example
c++ -std=c++2a -Werror -g -ggdb3 -Wall -c -o main.o main.cpp
c++ main.o  -o example
./example
</pre>
Expected output:
<pre>

# Create a custom vector class:
new MyVector(0x7fdd2cd02870, currlen=0, maxlen=1 elems=[])
push_back called MyVector(0x7fdd2cd02870, currlen=1, maxlen=1 elems=[10])
push_back called MyVector(0x7fdd2cd02870, currlen=2, maxlen=2 elems=[10,11])
vec1: [10][11]

# Create a new copy of vec1, vec2 via copy constructor (&):
copy constructor called for MyVector(0x7fdd2cd02870, currlen=2, maxlen=2 elems=[10,11])
copy constructor result is  MyVector(0x7ffee1b0e698, currlen=2, maxlen=2 elems=[10,11])
vec2: [10][11]

# Check we can append onto the copied vector:
push_back called MyVector(0x7ffee1b0e698, currlen=3, maxlen=4 elems=[10,11,12])
push_back called MyVector(0x7ffee1b0e698, currlen=4, maxlen=4 elems=[10,11,12,13])
vec2: [10][11][12][13]

# Create a new vector from vec1, vec3 via the move constructor (&&):
std::move called for MyVector(0x7fdd2cd02870, currlen=2, maxlen=2 elems=[10,11])
std::move result is  MyVector(0x7ffee1b0e678, currlen=2, maxlen=2 elems=[10,11])
vec3: [10][11]

# Check we can append onto the std:move'd vector:
push_back called MyVector(0x7ffee1b0e678, currlen=3, maxlen=4 elems=[10,11,14])
push_back called MyVector(0x7ffee1b0e678, currlen=4, maxlen=4 elems=[10,11,14,15])
vec3: [10][11][14][15]

# Destroy the old vector, vec1. It has no invalid elems:
delete MyVector(0x7fdd2cd02870, currlen=0, maxlen=0 elems=[])

# End, expect vec2 and vec3 destroy:
delete MyVector(0x7ffee1b0e678, currlen=4, maxlen=4 elems=[10,11,14,15])
delete MyVector(0x7ffee1b0e698, currlen=4, maxlen=4 elems=[10,11,12,13])
</pre>
How to use std::forward
=======================

std::forward's aim is to make it easier to pass along the exact type of function
prototype that is given. For example consider before std::forward you would have
to define all combinations of a template e.g.:
```C++
template <typename A, typename B< void f(A& a, B& b) { }
template <typename A, typename B< void f(const A& a, B& b) { }
template <typename A, typename B< void f(A& a, const B& b) { }
template <typename A, typename B< void f(A& a, B& b) { }
template <typename A, typename B< void f(const const A& a, const B& b) { }
```
This is tedious and does not scale. Now with std::forward we need to define only one template:
```C++
template<typename A, typename Account> void f(A&& a, B&& b) {
    f(std::forward<Account>(a), std::forward<Account>(b));
}
```
I struggled to find a useful non generic example of std::forward, but hit upon an
example of a bank account that we pass along the cash to be deposited as an argument.

So if we have a const version of an account we should expect when we pass it to our
deposit template<> that the const function is called; and this then throws an exception 
(the idea being this was a locked account!)

If we have a non const account then we should be able to modify the account.

As a final aside, std::forward is more or less eqivalent to static_cast<U&&>
althought there are apparently some corner cases where this is not true; but
I'm not aware of them, so please let me know :)

Here is the full example:
```C++
#include <iostream>
#include <string>
#include <sstream> // std::stringstream
#include <algorithm> // std::move
#include <utility>
#include <iostream>
#include <functional>

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
    friend int deposit (int cash, const BankAccount<int> &&account) {
        throw std::string("tried to write to a locked (const) account");
    }
    friend int deposit (int cash, const BankAccount<int> &account) {
        throw std::string("tried to write to a locked (const) account");
    }
    friend int deposit (int cash, BankAccount<int> &account) {
        account.deposit(cash);
        return account.cash;
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

template<typename T, typename Account>
int process_deposit(T cash, Account&& b) {
    return deposit(cash, std::forward<Account>(b));
}

int main(int, char**)
{
    try {
        // create account1 and try to deposit into it
        auto account1 = BankAccount<int>(0);
        process_deposit<int>(100, account1);
        std::cout << account1.to_string() << std::endl;
        std::cout << "SUCCESS: account1 deposit succeeded!" << std::endl;
    } catch (const std::string &e) {
        std::cerr << "FAILED: account1 deposit failed!: " << e << std::endl;
    }

    try {
        // create locked account2 and try to deposit into it; this should fail
        const auto account2 = BankAccount<int>(0);
        process_deposit<int>(100, account2);
        std::cout << account2.to_string() << std::endl;
        std::cout << "SUCCESS: account2 deposit succeeded!" << std::endl;
    } catch (const std::string &e) {
        std::cerr << "FAILED: account2 deposit failed!: " << e << std::endl;
    }

    try {
        // create locked account3 and try to deposit into it; this should fail
        auto account3 = BankAccount<int>(0);
        process_deposit<int>(100, std::move(account3));
        std::cout << account3.to_string() << std::endl;
        std::cout << "SUCCESS: account3 deposit succeeded!" << std::endl;
    } catch (const std::string &e) {
        std::cerr << "FAILED: account3 deposit failed!: " << e << std::endl;
    }
}
```
To build:
<pre>
cd std_forward
rm -f *.o example
c++ -std=c++2a -Werror -g -ggdb3 -Wall -c -o main.o main.cpp
c++ main.o  -o example
./example
</pre>
Expected output:
<pre>

# create account1 and try to deposit into it
new cash BankAccount(0x7ffee503f6b0, cash $0)
deposit cash called BankAccount(0x7ffee503f6b0, cash $100)
BankAccount(0x7ffee503f6b0, cash $100)
# SUCCESS: account1 deposit succeeded!
delete account BankAccount(0x7ffee503f6b0, cash $100)

# create locked account2 and try to deposit into it; this should fail
new cash BankAccount(0x7ffee503f670, cash $0)
delete account BankAccount(0x7ffee503f670, cash $0)
# FAILED: account2 deposit failed!: tried to write to a locked (const) account

# create locked account3 and try to deposit into it; this should fail
new cash BankAccount(0x7ffee503f630, cash $0)
delete account BankAccount(0x7ffee503f630, cash $0)
# FAILED: account3 deposit failed!: tried to write to a locked (const) account
</pre>
How to use std::unique_ptr
==========================

Unique pointers are just that. They cannot be shared. If you try to share
them you will find that an assign happens instead and the ownership of the
pointer moves, leaving the old unique pointer looking at nullptr.

To create a unique pointer you can either use the following which is a bit
long winded
```C++
    std::unique_ptr<class Foo> uptr(new Foo("foo2"));
```
Or, the following which hides the use of "new" but also invokes the copy
constructor. If you don't want that, use the above.
```C++
    auto uptr = std::make_unique< class Foo >(Foo("foo1"));
```
Note: To change the ownership of a unique pointer, use std::move

Example:
```C++
#include <memory>
#include <iostream>
#include <sstream>

class Foo {
private:
    std::string data; // local data for foo for debugging
public:
    Foo(std::string data) : data(data) {
        std::cout << "new " << to_string() << std::endl;
    }
    Foo(const Foo& other) {
        std::cout << "copy constructor " << to_string() << std::endl;
        data = other.data;
    }
    ~Foo() {
        // Note, other.reset will be called for us
        std::cout << "delete " << to_string() << std::endl;
    }
    std::string to_string (void) {
        auto address = static_cast<const void*>(this);
        std::stringstream ss;
        ss << address;
        return "Foo(" + ss.str() + ", data=" + data + ")";
    }
};

int main (void)
{
    // NOTE: make_unique creates a new ptr and will invoke foo1's copy constructor:
    auto uptr1 = std::make_unique< class Foo >(Foo("foo1"));

    // NOTE: to avoid the copy, do this:
    std::unique_ptr<class Foo> uptr2(new Foo("foo2"));

    // As you cannot copy unique pointers, reassign it with move
    std::unique_ptr<class Foo> uptr3;
    uptr3 = std::move(uptr2);

    // Let's print all the unique ptrs now
    std::cout << "uptr1 = " << (uptr1.get() ? uptr1->to_string() : "nullptr") << std::endl;
    std::cout << "uptr2 = " << (uptr2.get() ? uptr2->to_string() : "nullptr") << std::endl;
    std::cout << "uptr3 = " << (uptr3.get() ? uptr3->to_string() : "nullptr") << std::endl;

    // Expect the unique ptr data to be destroyed now
}
```
To build:
<pre>
cd std_unique_ptr
rm -f *.o example
c++ -std=c++2a -Werror -g -ggdb3 -Wall -c -o main.o main.cpp
c++ main.o  -o example
./example
</pre>
Expected output:
<pre>

# NOTE: make_unique creates a new ptr and will invoke foo1's copy constructor:
new Foo(0x7ffeeac2d088, data=foo1)
copy constructor Foo(0x7f82454029e0, data=)
delete Foo(0x7ffeeac2d088, data=foo1)

# NOTE: to avoid the copy, do this:
new Foo(0x7f8245402a00, data=foo2)

# As you cannot copy unique pointers, reassign it with move

# Let's print all the unique ptrs now
uptr1 = Foo(0x7f82454029e0, data=foo1)
uptr2 = nullptr
uptr3 = Foo(0x7f8245402a00, data=foo2)

# Expect the unique ptr data to be destroyed now
delete Foo(0x7f8245402a00, data=foo2)
delete Foo(0x7f82454029e0, data=foo1)
</pre>
How to use std::shared_ptr
==========================

Here we create two instances of a std::shared_ptr and then make them point to
each other. This creates a deadlock which can only be resolved by calling
reset() upon which the pointers can then be destructed.

To create a shared pointer you can either use the following which is a bit
long winded
```C++
    std::shared_ptr<class Foo> sptr(new Foo("foo2"));
```
Or, the following which hides the use of "new" but also invokes the copy
constructor. If you don't want that, use the above.
```C++
    auto sptr = std::make_shared< class Foo >(Foo("foo1"));
```
Example:
```C++
#include <memory>
#include <iostream>
#include <sstream>

typedef std::shared_ptr< class Foo > Foop;

class Foo {
private:
    std::string data; // local data for foo for debugging
    Foop other;       // pointer to other Foo's so we can make a deadlock
public:
    Foo(std::string data) : data(data) {
        std::cout << "new " << to_string() << std::endl;
    }
    Foo(const Foo& o) {
        std::cout << "copy constructor " << to_string() << std::endl;
        data = o.data;
        other = o.other;
    }
    ~Foo() {
        // Note, other.reset will be called for us
        std::cout << "delete " << to_string() << std::endl;
    }
    void addref (Foop other) {
        other = other;
        std::cout << "other use_count now " << other.use_count() << std::endl;
    }
    void delref (void) {
        other.reset();
        std::cout << "other use_count now " << other.use_count() << std::endl;
    }
    std::string to_string (void) {
        auto address = static_cast<const void*>(this);
        std::stringstream ss;
        ss << address;
        return "Foo(" + ss.str() + ", data=" + data + ")";
    }
};

int main (void)
{
    // Create a copy constructed class and share it between two pointers:
    auto sptr1 = std::make_shared< class Foo >(Foo("foo1"));
    std::cout << "sptr1 ref count now " << sptr1.use_count() << std::endl;
    auto sptr2 = sptr1;
    std::cout << "sptr2 ref count now " << sptr2.use_count() << std::endl;

    // Try to create a deadlock:
    sptr1->addref(sptr2);
    std::cout << "sptr1 ref count now " << sptr1.use_count() << std::endl;
    sptr2->addref(sptr1);
    std::cout << "sptr2 ref count now " << sptr2.use_count() << std::endl;

    // Undo the 'deadlock':
    sptr1->delref();
    std::cout << "sptr1 ref count now " << sptr1.use_count() << std::endl;
    sptr2->delref();
    std::cout << "sptr2 ref count now " << sptr2.use_count() << std::endl;

    // Release the shared sptrs, expect foo1 to be destroyed:
    sptr1.reset();
    std::cout << "sptr1 ref count now " << sptr1.use_count() << std::endl;
    sptr2.reset();
    std::cout << "sptr2 ref count now " << sptr2.use_count() << std::endl;

    // You can also create shared pointers WITHOUT copy constructor overhead
    std::shared_ptr<class Foo> sptr0(new Foo("foo0"));
    std::cout << "sptr0 = " << (sptr0.get() ? sptr0->to_string() : "nullptr") << std::endl;
}
```
To build:
<pre>
cd std_shared_ptr
rm -f *.o example
c++ -std=c++2a -Werror -g -ggdb3 -Wall -c -o main.o main.cpp
c++ main.o  -o example
./example
</pre>
Expected output:
<pre>

# Create a copy constructed class and share it between two pointers:
new Foo(0x7ffee1a80220, data=foo1)
copy constructor Foo(0x7fa605c029f8, data=)
delete Foo(0x7ffee1a80220, data=foo1)
sptr1 ref count now 1
sptr2 ref count now 2

# Try to create a deadlock:
other use_count now 3
sptr1 ref count now 2
other use_count now 3
sptr2 ref count now 2

# Undo the 'deadlock':
other use_count now 0
sptr1 ref count now 2
other use_count now 0
sptr2 ref count now 2

# Release the shared sptrs, expect foo1 to be destroyed:
sptr1 ref count now 0
delete Foo(0x7fa605c029f8, data=foo1)
sptr2 ref count now 0

# You can also create shared pointers WITHOUT copy constructor overhead
new Foo(0x7fa605c029b0, data=foo0)
sptr0 = Foo(0x7fa605c029b0, data=foo0)
delete Foo(0x7fa605c029b0, data=foo0)
</pre>
How to make your own wrapper around std::shared_ptr
===================================================

Here we create a wrapper around std::shared_ptr. Why? Mostly educational,
but you could use this to keep track of memory allocation and frees as part
of a basic leak detector.

```C++
#include <memory>
#include <sstream>
#include <string>
#include <iostream>

template <typename T> class MySharedPtr {
private:
    std::shared_ptr<T> sptr;
    std::string name {"nullptr"};

    void debug (const std::string &what) {
        std::cout << name << ": " << what << " " << to_string() << std::endl;
    }

    std::string to_string (void) {
        auto address = static_cast<const void*>(this);
        std::stringstream ss;
        ss << address;
        if (sptr) {
            return "MySharedPtr(" + ss.str() + "," + sptr->to_string() + ")";
        } else {
            return "MySharedPtr(" + ss.str() + ")";
        }
    }
public:
    // explicit means constructor must match exactly
    template <typename ...ARGS> explicit
      MySharedPtr(const std::string &name, ARGS... a) : name(name) {
        sptr = std::make_shared<T>(a...);
        debug("MySharedPtr::make_shared");
    }

    explicit MySharedPtr(const std::string &name) : name(name) {
        sptr = std::make_shared<T>();
        debug("MySharedPtr::make_shared");
    }

    explicit MySharedPtr(void) {
        debug("MySharedPtr::default constructor");
    }

    ~MySharedPtr() {
        debug("MySharedPtr::delete");
    }

    T* const operator->() {
        debug("MySharedPtr::-> dereference");
        return sptr.operator->();
    }

    T* get() const {
        debug("MySharedPtr::get ptr");
        return sptr.get();
    }

    T& operator*() {
        debug("MySharedPtr::* ptr");
        return *sptr;
    }

    const T& operator*() const {
        debug("MySharedPtr::const * ptr");
        return *sptr;
    }

    operator bool() const {
        debug("MySharedPtr::bool");
        return (bool)sptr;
    }

    size_t use_count(void) const {
        return sptr.use_count();
    }

    void reset() {
        debug("MySharedPtr::reset");
        sptr.reset();
    }
};

typedef MySharedPtr< class Foo > Foop;

class Foo {
private:
    std::string data;
    void debug (const std::string &what) {
        std::cout << what << " " << to_string() << std::endl;
    }
public:
    Foo(std::string data) : data(data) {
        debug("new");
    }
    ~Foo() {
        debug("delete");
    }
    std::string to_string (void) {
        auto address = static_cast<const void*>(this);
        std::stringstream ss;
        ss << address;
        return "Foo(" + ss.str() + ", data=" + data + ")";
    }
};

int main (void)
{
    // create a class and share it between two pointers:
    auto sptr1 = MySharedPtr< class Foo >("[foo1]", Foo("foo1-data"));
    std::cout << "sptr1 ref count now " << sptr1.use_count() << std::endl;
    auto sptr2 = sptr1;
    std::cout << "sptr2 ref count now " << sptr2.use_count() << std::endl;

    // release the shared sptrs, expect foo1 to be destroyed:
    sptr1.reset();
    std::cout << "sptr1 ref count now " << sptr1.use_count() << std::endl;
    sptr2.reset();
    std::cout << "sptr2 ref count now " << sptr2.use_count() << std::endl;
}
```
To build:
<pre>
cd std_shared_ptr_wrapper
rm -f *.o example
c++ -std=c++2a -Werror -g -ggdb3 -Wall -c -o main.o main.cpp
c++ main.o  -o example
./example
</pre>
Expected output:
<pre>

# create a class and share it between two pointers:
new Foo(0x7ffedfd66658, data=foo1-data)
[foo1]: MySharedPtr::make_shared MySharedPtr(0x7ffedfd66688,Foo(0x7faa164029c8, data=foo1-data))
delete Foo(0x7ffedfd66658, data=foo1-data)
sptr1 ref count now 1
sptr2 ref count now 2

# release the shared sptrs, expect foo1 to be destroyed:
[foo1]: MySharedPtr::reset MySharedPtr(0x7ffedfd66688,Foo(0x7faa164029c8, data=foo1-data))
sptr1 ref count now 0
[foo1]: MySharedPtr::reset MySharedPtr(0x7ffedfd66608,Foo(0x7faa164029c8, data=foo1-data))
delete Foo(0x7faa164029c8, data=foo1-data)
sptr2 ref count now 0
[foo1]: MySharedPtr::delete MySharedPtr(0x7ffedfd66608)
[foo1]: MySharedPtr::delete MySharedPtr(0x7ffedfd66688)
</pre>
