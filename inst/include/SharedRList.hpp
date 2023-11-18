#ifndef SHAREDRLIST_HPP
#define SHAREDRLIST_HPP

#include <map>

#include <boost/interprocess/containers/map.hpp>
#include "SharedRObject.hpp"
#include "SharedRVector.hpp"
class SharedList;

namespace Rcpp {
    template <> SEXP wrap(const SharedList&);
    template <> SharedList as<SharedList>(SEXP);

}

class SharedList : public SharedRObject {

    struct tuple {
        std::string key;
        std::string sm_name;
        SMTYPE type = SMTYPE::UNKNOWN;
        SEXP data = R_NilValue;
        double value;

        tuple() {
        }

        tuple(const tuple& other) {
            key = other.key;
            sm_name = other.sm_name;
            type = other.type;
            data = other.data;
            value = other.value;
        }

    };

    std::vector<std::shared_ptr<SharedRObject> > stash;

    typedef std::string KeyType;
    typedef tuple MappedType;
    typedef std::pair<const std::string, tuple> ValueType;

    typedef bip::allocator<ValueType, bip::managed_shared_memory::segment_manager> ShmemAllocator3;
    typedef bip::map<KeyType, MappedType, std::less<KeyType>, ShmemAllocator3> SMLIST;

    bip::managed_shared_memory segment;
    SMLIST* tlist_m;

    void init(const std::string name) {
        this->name = name;
        bip::shared_memory_object::remove(name.c_str());

        //Create a new segment with given name and size
        segment = bip::managed_shared_memory(bip::create_only, name.c_str(), 65536);

        //Initialize shared memory STL-compatible allocator
        const ShmemAllocator3 alloc_inst(segment.get_segment_manager());

        //Construct a vector named "MyVector" in shared memory with argument alloc_inst
        this->tlist_m = segment.construct<SMLIST>("SMLIST")(std::less<KeyType>(), alloc_inst);
        std::map<std::string, std::shared_ptr<SharedRObject> > local_map;
    }

    SEXP create_shared_vector(std::string sm_name) {
        std::shared_ptr<SharedVector> v = std::make_shared<SharedVector>();
        v->open(sm_name);
        stash.push_back(v);
        return Rcpp::wrap(*v);
    }

    SEXP create_shared_list(std::string sm_name) {
        std::shared_ptr<SharedList> l = std::make_shared<SharedList>();
        l->open(sm_name);
        stash.push_back(l);
        return Rcpp::wrap(*l);
    }

public:

    typedef typename SMLIST::key_type key_type;
    typedef typename SMLIST::key_type mapped_type;
    typedef typename SMLIST::value_type value_type;
    typedef typename SMLIST::size_type size_type;
    typedef typename SMLIST::difference_type difference_type;
    typedef typename SMLIST::key_compare key_compare;
    typedef typename SMLIST::allocator_type allocator_type;
    typedef typename SMLIST::reference reference;
    typedef typename SMLIST::const_reference const_reference;
    typedef typename SMLIST::pointer pointer;
    typedef typename SMLIST::const_pointer const_pointer;
    typedef typename SMLIST::iterator iterator;
    typedef typename SMLIST::const_iterator const_iterator;
    typedef typename SMLIST::reverse_iterator reverse_iterator;
    typedef typename SMLIST::node_type node_type;

    SharedList() {
    }

    SharedList(const SharedList& other) :
    tlist_m(other.tlist_m) {
        this->name = other.name;
        // this->open(other.segment.named_begin()->name());
    }

    void create(const std::string& name) {
        this->init(name);
    }

    void open(const std::string& name) {
        //Open the managed segment
        this->name = name;
        this->segment = bip::managed_shared_memory(bip::open_only, name.c_str());

        //Find the vector using the c-string name
        this->tlist_m = segment.find<SMLIST>("SMLIST").first;

        typename SMLIST::iterator it;

        for (it = tlist_m->begin(); it != tlist_m->end(); ++it) {
            SMTYPE sm_type = (*it).second.type;
            //  
            switch (sm_type) {
                case SMTYPE::SMVECTOR:
                    (*it).second.data = create_shared_vector((*it).second.sm_name);
                    break;
                case SMTYPE::SMLIST:
                    (*it).second.data = create_shared_list((*it).second.sm_name);
                    break;
            }
        }
    }

    void destroy(const std::string & name) {
        this->segment.destroy<SMLIST>(name.c_str());
    }

    void set(const std::string& key, SEXP object) {

        if (this->is_SharedRObject(object)) {
            SMTYPE type = this->getSharedRObjectType(object);
            tuple t;


            if (type == SMTYPE::SMVECTOR) {
                SharedVector v(Rcpp::as<SharedVector>(object));

                t.key = key;
                t.sm_name = v.name;
                t.type = SMTYPE::SMVECTOR;
                t.data = object;
                (*this->tlist_m)[key] = t;

            } else if (type == SMTYPE::SMLIST) {
                SharedList v(Rcpp::as<SharedList>(object));

                t.sm_name = v.name;
                t.type = SMTYPE::SMLIST;
                t.data = object;
                (*this->tlist_m)[key] = t;

            } else if (type == SMTYPE::SMNUMERIC) {

                t.sm_name = "na";
                t.type = SMTYPE::SMNUMERIC;
                t.data = object;
                t.value = Rcpp::as<double>(object);
                (*this->tlist_m)[key] = t;

            } else {
                Rcpp::Rcout << "Attempting to set a unknown SharedRObject into SharedList.";
            }
        }


    }

    SEXP get(const std::string& key) {


        if ((*this->tlist_m)[key].type == SMTYPE::SMNUMERIC) {
            return Rcpp::wrap((*this->tlist_m)[key].value);
        }

        SEXP s = (*this->tlist_m)[key].data;
        return s;
        //        SMTYPE type = this->getSharedRObjectType(s);
        //
        //        if (type == SMTYPE::SMVECTOR) {
        //            Rcpp::Rcout<<"Is SMVECTOR type!\n";
        //            return Rcpp::wrap(Rcpp::as<SharedVector>(s));
        //        }
        //        Rcpp::Rcout<<"Is not SMVECTOR type!\n";
        //
        //        //    if(this->local_map.size()){
        //        //      return Rcpp::wrap(*this->local_map[key].get());
        //        //    }
        //        //    return static_cast<SEXP> (R_ExternalPtrAddr((*this->tlist_m)[key].data));
    }

    size_t size() {
        return this->tlist_m->size();
    }

};


namespace Rcpp {
    //    template <> SEXP wrap(const SharedList&);
    //    template <> SharedList as<SharedList>(SEXP) throw (not_compatible);

    template <> SEXP wrap<SharedList>(const SharedList& el) {
        Rcpp::Rcout << __LINE__ << std::endl;
        Rcpp::Language call("new", Symbol("SharedList"));
        return call.eval();
    }

    template <> SharedList as(SEXP s) {
        try {
            if (TYPEOF(s) != S4SXP) {
                Rcpp::Rcout << __LINE__ << std::endl;
                ::Rf_error("supplied object is not of type TestClass.");
            }

            Rcpp::S4 s4obj(s);
            if (!s4obj.is("Rcpp_SharedList")) {
                Rcpp::Rcout << __LINE__ << std::endl;
                ::Rf_error("supplied object is not of type SharedList.");
            }

            Rcpp::Environment env(s4obj);
            Rcpp::XPtr<SharedList> xptr(env.get(".pointer"));

            // build new TestClass object with copied data

            return SharedList((* xptr.get()));
        } catch (...) {
            Rcpp::Rcout << __LINE__ << std::endl;
            ::Rf_error("supplied object could not be converted to SharedList.");
        }
    }


}

RCPP_EXPOSED_CLASS(SharedList)


SharedList sm_list() {
    return SharedList();
}


#endif
