#include <cassert>
#include <memory>

#include "can/utils/unique_owner_ptr.hpp"

class test_a {
   public:
    test_a(int a) : a_(a) {}

    [[nodiscard]] int get_a() const {
        return a_;
    }

    void set_a(int value) {
        a_ = value;
    }

   private:
    int a_;
};

class test_b {
   public:
    test_b(bool b) : b_(b) {}

    [[nodiscard]] bool get_b() const {
        return b_;
    }

    void set_b(bool value) {
        b_ = value;
    }

   private:
    bool b_;
};

class test_class : public test_a, public test_b {
   public:
    test_class(int a, bool b) : test_a(a), test_b(b) {}
};

int main() {
    auto ptr = std::make_shared<test_class>(10, false);
    assert(ptr.use_count() == 1);

    /* first owner (constructor) */
    auto c1 = can::utils::unique_owner_ptr<test_class>(ptr);
    assert(c1.get() == ptr.get());
    assert(ptr.use_count() == 2);
    assert(ptr->get_a() == c1->get_a());
    assert(ptr->get_b() == c1->get_b());
    assert(ptr->get_a() == (*c1).get_a());
    assert(ptr->get_b() == (*c1).get_b());

    /* change a value in first owner */
    c1->set_a(2);
    assert(ptr->get_a() == 2);

    /* shouldn't compile (copy constructor) */
    /* auto u2 = can::utils::unique_owner_ptr<test_class>(u1); */

    /* shouldn't compile (copy assigment) */
    /* auto u2 = u1; */

    /* second owner (move constructor) */
    auto c2 = can::utils::unique_owner_ptr<test_class>(std::move(c1));
    assert(c1.get() == nullptr);
    assert(c2.get() == ptr.get());
    assert(ptr.use_count() == 2);
    assert(ptr->get_a() == c2->get_a());
    assert(ptr->get_b() == c2->get_b());
    assert(ptr->get_a() == (*c2).get_a());
    assert(ptr->get_b() == (*c2).get_b());

    /* change a value in first owner */
    c2.get()->set_b(true);
    assert(ptr->get_b() == true);

    /* third owner (move assignment) */
    auto c3 = std::move(c2);
    assert(c1.get() == nullptr);
    assert(c2.get() == nullptr);
    assert(c3.get() == ptr.get());
    assert(ptr.use_count() == 2);
    assert(ptr->get_a() == c3->get_a());
    assert(ptr->get_b() == c3->get_b());
    assert(ptr->get_a() == (*c3).get_a());
    assert(ptr->get_b() == (*c3).get_b());

    /* fourth owner (inherited type) */
    auto a1 = can::utils::unique_owner_ptr<test_a>(std::dynamic_pointer_cast<test_a>(ptr));
    assert(a1.get() == ptr.get());
    assert(ptr.use_count() == 3);
    assert(ptr->get_a() == a1->get_a());
    assert(ptr->get_a() == (*a1).get_a());

    /* fifth owner (move assignment) */
    auto a2 = std::move(a1);
    assert(a1.get() == nullptr);
    assert(a2.get() == ptr.get());
    assert(ptr.use_count() == 3);
    assert(ptr->get_a() == a2->get_a());
    assert(ptr->get_a() == (*a2).get_a());

    /* empty owner (move assignment) */
    auto a3 = std::move(a1);
    assert(a1.get() == nullptr);
    assert(a2.get() == ptr.get());
    assert(a3.get() == nullptr);
    assert(ptr.use_count() == 3);

    /* sixth owner (inherited type) */
    auto b1 = can::utils::unique_owner_pointer_cast<test_b>(c3);
    assert(c1.get() == nullptr);
    assert(b1.get() == ptr.get());
    assert(ptr.use_count() == 3);
    assert(ptr->get_b() == b1->get_b());
    assert(ptr->get_b() == (*b1).get_b());

    return 0;
}