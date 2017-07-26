#include <iostream>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>

struct Doc{
public:
    static std::shared_ptr<Doc> inst();
private:
    static Doc* instance;
    static std::mutex mtx;
};

Doc* Doc::instance = nullptr;
std::mutex Doc::mtx;

std::shared_ptr<Doc> Doc::inst() {

    //std::lock_guard<std::mutex> lock(mtx);
    mtx.lock();
    std::cout << "mutex locked" << std::endl;
    if(Doc::instance == nullptr){
        Doc::instance = new Doc();
    }
    return std::shared_ptr<Doc>(Doc::instance,[](Doc* p){
        Doc::mtx.unlock();
        std::cout << "mutex released" << std::endl;
    });
}

void f(int i){
    auto a = Doc::inst();
    std::cout << "thread " << i << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

int main(){
    std::thread th1(f,1);
    std::thread th2(f,2);
    th1.join();
    th2.join();
    std::cout << "exit" << std::endl;
}
