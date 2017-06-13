#include <iostream>
#include <vector>
#include <typeindex>
#include <map>
#include <memory>

using namespace std;

/*
 * At any critical point it is possible to calculate different properties
 * by different methods. These properties are not needed permanently, so
 * the computation and storage of unneeded data is redundant.
 *
 * It was decided here to keep only those properties that was already calculated.
 * In a simplest case, there are just one data that is obligatory - it is a type of a CP.
 * It becomes known when a CP is found and it is constant. All the other properties
 * are maintained in a dynamical memory that is managed automatically with the help
 * of shared_ptr container (is deleted automatically with the last container).
 *
 * Each property has it's own data type. Therefore for a uniform access to the pointers
 * it is necessary to have a common minimal base class, CP_BASE_PROP. This class also
 * defines a requirement that each property type implements it's "origin" function,
 * to keep track of ANY possible origins.
 *
 * In the case when an origin is static (say calculated by a well defined method without
 * any additional parameters) - it is possible to implement an "origin" without necessity
 * to store any additional information inside a property object. This saves space,
 * because in this case origin data is stored just once in the implementation of its
 * vtable. However, if an origin info is parametrized it is better to store original info in a
 * shared_ptr, and share this pointer among all CPs that have the same property with the
 * same origin.
 *
 * The next interface extension is defined in a template CP_PROP. This template defines
 * a property value and it's type reference. CP_PROP is public, and can be used to define
 * any non-standard properties required for possible calculations. Also here is defined
 * a requirement that each property data type has a non-empty constructor (empty properties
 * shell not be needed).
 *
 *
 * Advantages and use-cases:
 *  - comparison of different computational methods
 *  - selection of an active property among any possibly similar
 *  - property standardization
 *  - space and code save on unnecessary properties
 *  - no speed loss at repetitive property access
 *  - use of CP object for any temporary data storage
 *  - check if necessary data is present
 *
 *  Disadvantages:
 *  - time used to find a pointer onto the property
 *  - each property require ~40 bytes overhead
 *  - definition of even standard properties shell be done outside this class
 *
 * Typical usage:
 *
 * struct aim_coord : public CP::coord{
 *   wfx_coord(CP::coord::valtype val) : CP::coord(val) {};
 *   string origin(){return string("AIMAll calc");}
 * };
 *
 * CP cp(CP::BCP);
 * wfx_coord c{{1,2,3}};
 * cp.add_prop(c);
 * cp.get_prop<wfx_coord>()->val[2] /-z-coord-/
 *    /-or somewhere else, where wfx_coord is unknown-/
 * cp.get_prop<CP::coord>()->val[2] /-z-coord-/
 */


class CP{
public:

    enum TYPE{
        NACP,  //Nuclear Attractor Critical Point
        NNACP, //Non-Nuclear Attractor Critical Point
        BCP,   //Bond Critical Point
        RCP,   //Ring Critical Point
        CCP};  //Cage Critical Point

private:
    struct BASE_PROP{
        virtual string origin() = 0;
        virtual string name(){return "NA";}
        virtual string description(){return "NA";}
    };

public:
    template<typename T>
    struct PROP : public BASE_PROP{
        typedef T valtype;
        PROP(T val):val{val} {};
        valtype val;
    };

    struct coord : public PROP<array<double, 3>>{
        coord(coord::valtype val) : PROP<array<double, 3>>(val){};
        virtual string name(){return "coord";}
        virtual string description(){return "cartesian coordinates";}
    };

    struct rho : public PROP<double>{
        rho(rho::valtype val) : PROP<double>(val){};
        virtual string name(){return "rho";}
        virtual string description(){return "electron density at CP";}
    };

    /*
     * define here any other standard properties that shell be known in
     * different places of an application
     */

public:
    CP(TYPE cp_type) : cp_type{cp_type} {};

    template<typename T>
    void add_prop(const T& val){
        static_assert(is_base_of<BASE_PROP,T>::value,
                      "any CP property shell be inherited from CP_BASE_PROP");
        type_map[typeid(T)] = shared_ptr<T>(new T(val));
    };

    template<typename T>
    shared_ptr<T> get_prop(){
        auto it = type_map.find(typeid(T));
        //exact coincidence
        if(it != type_map.end()) return dynamic_pointer_cast<T>(it->second);
        //check that T is a base type of any
        for(auto& it : type_map){
            if(shared_ptr<T> p = dynamic_pointer_cast<T>(it.second))
                return p;
        }
        throw runtime_error("CP: requested property not present");
    }

    template<typename T>
    void del_prop(){
        for(auto it = type_map.begin(); it != type_map.end();){
            if(dynamic_pointer_cast<T>(it->second)){
                type_map.erase(it++);
            }else{
                ++it;
            }
        }
    }

    template<typename T>
    bool check_prop(){
        try{
            get_prop<T>();
            return true;
        }catch(...){
            return false;
        }
    }

public:
    const TYPE cp_type;
private:
    std::map<std::type_index, shared_ptr<BASE_PROP>> type_map;
};

struct wfx_coord : public CP::coord{
    wfx_coord(CP::coord::valtype val) : CP::coord(val) {};
    string origin(){return string("coord origin");}
};

struct wfx_rho : public CP::rho{
    wfx_rho(CP::rho::valtype val) : CP::rho(val) {};
    string origin(){return string("rho origin");}
};


int main(){

    CP cp(CP::BCP);
    wfx_coord wc{{1,2,3}};
    wfx_rho rho{3.14};

    cp.add_prop(wc);
    cp.add_prop(rho);

    auto p = cp.get_prop<wfx_coord>();
    cp.del_prop<CP::coord>();

    cout << sizeof(shared_ptr<CP::PROP<char>>) << " " << sizeof(CP::PROP<char>) << " " << sizeof(type_index) << endl;

}