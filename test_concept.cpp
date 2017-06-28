#include <iostream>
#include <exception>
#include <memory>

using namespace std;

template <typename TRef>
struct DerefTrait{
    using T = TRef::element_type;

    int foo(std::shared_ptr<T> ptr){
        cout << "shared" << endl;
    }

    int foo(std::weak_ptr<T> ptr){
        cout << "weak" << endl;
    }
};


int main(){
    using t_shared = std::shared_ptr<int>;
    using t_weak = std::weak_ptr<int>;

    t_shared s_a(new int(10));
    t_weak w_a(s_a);
    s_a.reset();

    if(w_a.use_count() == 0){
        std::cout << "weak is null" << std::endl;
    }else{
        std::enable_if<std::is_base_of<t_weak, typeof(w_a)>::value>
        std::cout << "weak is not null,  val = " << *w_a.lock() << std::endl;
    }
}