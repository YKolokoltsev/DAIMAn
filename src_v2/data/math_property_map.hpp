//
// Created by morrigan on 29/07/17.
//

#ifndef DAIMAN_MATH_PROPERTY_MAP_H
#define DAIMAN_MATH_PROPERTY_MAP_H

/*
 *
 * Given a mathematical object, for example a critical point it is possible to
 * calculate different properties of it, each one by probably a number of
 * different methods. Sometimes these properties are not needed permanently,
 * and on the other hand - their complete number can change during development.
 *
 * Except of some basic definition of a mathematical object, these floating
 * properties is convenient to keep in dynamical memory and manage with the help of
 * MathPropertyMap class. On the lo level each property is stored inside
 * shared_ptr container, so it's deletion is automated.
 *
 * Each property has it's own data type, and shell be identifyed somehow.
 * We selected here a semi-static (compile-time) management. Each property inherit
 * a common minimal base class, MathBase. This class defines a requirement that each
 * property type implements it's own "origin" function, to keep track of ANY possible
 * origin of the "same" property computed by different algorithms.
 *
 * In the case when an origin is static (say calculated by a well defined method without
 * any additional parameters), in a pure dynamic case there will be a lots of redundant
 * information stored (for example - same origin for 300 CPs). However, it is possible to
 * put this and the other info into a property type functions. This saves space,
 * because any static data data is stored just once in the implementation of
 * it's class vtable, for example in the an 'origin' function.
 *
 * In the case when origin info is parametrized, but remains the same for a set of
 * computations, it is possible to store dynamic info in a shared_ptr,
 * and share this same pointer among all properties with the same origin property. Say - return
 * to dynamic case when necesarry.
 *
 * If we inherit property types one from another, there is just a little work left
 * (thanks to dynamic_pointer_cast), to manage a "castability" dynamically.
 *
 * The next interface extension is defined in a template Property. This template defines
 * a property value and it's type reference. Property template can be used to define
 * any standard as well as non-standard properties required. A standard property, is
 * a property which type is known in any point of the program and is declared
 * incide of a standard header, and accessed by upcasting. A non-standard property can
 * be defined anywhere in the computational routine, attached than to the MathPropertyMap
 * dynamically and removed after the computation is complete.
 *
 * Also a Property templete defines a requirement that each property data type
 * has a non-empty constructor (non-initialized properties shell not be stored).
 *
 * It is possible to define a NOT VIRTUAL "compare" function in a class three,
 * so that properties from different computations could be compared in a transparent manner.
 *
 * In the case when two properties has the same standard base class, there happens
 * a problem of choice of the value after upcast. The solution is simple - there
 * shell be an "active" flag be stored. When this flag is changed, from outside it
 * fills like the property was just recalculated, so all dependencies shell be
 * also recalculated.
 * Todo: The "active" flag or something else - this is unclear for the momet
 *
 *
 * Advantages and use-cases:
 *  - comparison of different computational methods
 *  - selection of an active property among any possibly similar
 *  - compile time property standardization
 *  - space and code save on unnecessary properties
 *  - no speed loss at repetitive property access by pointer
 *  - use of MathPropertyMap object for any temporary data storage
 *  - check if necessary data is present
 *  - check for algorithm applicability to the object with complete/incomplete/redundant props.
 *
 *  Disadvantages:
 *  - time used to find a pointer onto the property
 *  - each property require 15 bytes + type_index overhead
 *  - definition of property types shell be completed outside this class,
 *    it was decided to require the definition of "origin" requirement each time.
 *
 * Typical usage:
 *
 * //specify property origin whithin a type
 * struct aim_coord : public CP::coord{
 *   wfx_coord(CP::coord::valtype val) : CP::coord(val) {};
 *   string origin(){return string("AIMAll calc");}
 * };
 *
 * //create CP object
 * CP cp(CP::BCP);
 *
 * //create property object
 * aim_coord c{{1,2,3}};
 *
 * //copy 'c' into 'cp'
 * cp.add_prop(c);
 *
 * //get shared pointer to aim_coord (stores x,y,z)
 * auto p = cp.get_prop<aim_coord>();
 *
 * //if aim_coord declaration is unknown we can access x,y,z by its parent
 * cout << "z-coord is: " << cp.get_prop<CP::coord>()->val[2] << endl;
 *
 * //remove property from CP
 * cp.del_prop<CP::coord>();
 *
 * //data is alive until the last shared_ptr object deleted (it is in stack now)
 * cout << p->val[0] << endl;
 *
 * //the property is completely removed now
 * p.release();
 */

/*
 * Basic Class for mathematical objects, the main thing here - is to store
 * an origin of the mathematical object. Same objects can be obtained
 * with the help of different algorithms and than compared automatically.
 */

#include <string>
#include <memory>
#include <typeindex>
#include <map>

//todo: remove namespace in header
using namespace std;

struct MathBase{
    virtual std::string origin() = 0;
    virtual std::string name(){return "NA";}
    virtual std::string description(){return "NA";}
};

/*
 * A generic property base class designed for MathPropertyMap.
 */
template<typename T>
struct MathProperty : public MathBase{
    typedef T valtype;
    MathProperty(T val):val{val} {};
    valtype val;
};

class MathPropertyMap{

public:
    template<typename T>
    void add_prop(const T& val){
        static_assert(std::is_base_of<MathBase,T>::value, "not inherited from MathBase");
        type_map[typeid(T)] = std::shared_ptr<T>(new T(val));
    };

    template<typename T>
    std::shared_ptr<T> get_prop(){
        static_assert(std::is_base_of<MathBase,T>::value, "not inherited from MathBase");
        auto it = type_map.find(typeid(T));
        //todo: return all list for given T?
        //exact coincidence
        if(it != type_map.end()) return dynamic_pointer_cast<T>(it->second);
        //check that T is a base type of any
        for(auto& it : type_map){
            if(std::shared_ptr<T> p = dynamic_pointer_cast<T>(it.second))
                return p;
        }
        throw runtime_error("CP: requested property not found");
    }

    //todo: delete all by type, or delete by pointer
    template<typename T>
    void del_prop(){
        static_assert(std::is_base_of<MathBase,T>::value, "not inherited from MathBase");
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
        static_assert(std::is_base_of<MathBase,T>::value, "not inherited from MathBase");
        try{
            get_prop<T>();
            return true;
        }catch(...){
            return false;
        }
    }

private:
    std::map<std::type_index, std::shared_ptr<MathBase>> type_map;
};

#endif //DAIMAN_MATH_PROPERTY_MAP_H
