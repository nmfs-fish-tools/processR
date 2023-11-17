#ifndef SHAREDRVECTOR_HPP
#define SHAREDRVECTOR_HPP

#include <iterator>
#include <sstream>
#include <chrono>

#include "SharedRObject.hpp"

using namespace boost::interprocess;

typedef allocator<int, managed_shared_memory::segment_manager> SharedIntAllocator;
typedef allocator<double, managed_shared_memory::segment_manager> SharedRealAllocator;
typedef allocator<char, managed_shared_memory::segment_manager> SharedCharAllocator;
typedef std::vector<int, SharedIntAllocator> SharedIntVector;
typedef std::vector<double, SharedIntAllocator> SharedRealVector;




//This allocator will allow placing containers in the segment
typedef allocator<double, managed_shared_memory::segment_manager> ShmemAllocator;

//Alias a vector that uses the previous STL-like allocator so that allocates
//its values from the segment
typedef vector<double, ShmemAllocator> REALSMVector;





/**
 * Wrapper class for std::vector types. If this file is compiled with -DTMB_MODEL,
 * conversion operators are defined for TMB vector types.
 */
class SharedVector : public SharedRObject {
  managed_shared_memory segment;
  REALSMVector* vec_m;
  
  /**
   * @brief friiend comparison operator.
   */
  friend bool operator==(const SharedVector& lhs,
                         const SharedVector& rhs);
  
  void init(const std::string name) {
    
  
    this->name = name;
    shared_memory_object::remove(name.c_str());
    
    //Create a new segment with given name and size
    segment = managed_shared_memory(create_only, name.c_str(), 65536);
    
    //Initialize shared memory STL-compatible allocator
    const ShmemAllocator alloc_inst(segment.get_segment_manager());
    
    //Construct a vector named "MyVector" in shared memory with argument alloc_inst
    this->vec_m = segment.construct<REALSMVector>("REALSMVector")(alloc_inst);
   
      
  }
  
public:
  //Member Types
  
  typedef typename REALSMVector::value_type value_type; /*!<Member type Type>*/
  typedef typename REALSMVector::allocator_type allocator_type; /*!<Allocator for type Type>*/
  typedef typename REALSMVector::size_type size_type; /*!<Size type>*/
  typedef typename REALSMVector::difference_type difference_type; /*!<Difference type>*/
  typedef typename REALSMVector::reference reference; /*!<Reference type &Type>*/
  typedef typename REALSMVector::const_reference const_reference; /*!<Constant eference type const &Type>*/
  typedef typename REALSMVector::pointer pointer; /*!<Pointer type Type*>*/
  typedef typename REALSMVector::const_pointer const_pointer; /*!<Constant ointer type const Type*>*/
  typedef typename REALSMVector::iterator iterator; /*!<Iterator>*/
  typedef typename REALSMVector::const_iterator const_iterator; /*!<Constant iterator>*/
  typedef typename REALSMVector::reverse_iterator reverse_iterator; /*!<Reverse iterator>*/
  typedef typename REALSMVector::const_reverse_iterator const_reverse_iterator; /*!<Constant reverse iterator>*/
  
  /**
   * Default constructor.
   */
  SharedVector() {
    // std::stringstream ss;
    // 
    // std::time_t t = std::time(0);
    // ss << "REALSMVector_" << t;
    // this->init(ss.str().c_str());
  }
  
  /**
   * @brief Constructs the container of size copies of elements with value value.
   */
  SharedVector(size_t size, const double& value = double()) {
    this->vec_m->resize(size, value);
  }
  
  /**
   * @brief Copy constructor.
   */
  SharedVector(const SharedVector& other) :
    /*segment(other.segment),*/ vec_m(other.vec_m) {
    this->name = other.name;
  }
  
  // /**
  //  * @brief Copy constructor.
  //  */
  // SharedVector(const SharedVector& other) {
  //   this->
  //   this->vec_m = other.vec_m;//->resize(other.size());
  //   // for (size_t i = 0; i < this->vec_m->size(); i++) {
  //   //   this->vec_m->at(i) = other[i];
  //   // }
  // }
  
  /**
   * @brief Initialization constructor with std::vector type..
   */
  SharedVector(const std::vector<double>& other) {
    this->vec_m->resize(other.size());
    for (size_t i = 0; i < this->vec_m->size(); i++) {
      this->vec_m->at(i) = other[i];
    }
  }
  
  void create(const std::string& name) {
    this->init(name);
  }
  
  void open(const std::string& name) {
    //Open the managed segment
      this->name = name;
    this->segment = managed_shared_memory(open_only, name.c_str());
    
    //Find the vector using the c-string name
    this->vec_m = segment.find<REALSMVector>("REALSMVector").first;
  }
  
  void destroy(const std::string& name) {
    this->segment.destroy<REALSMVector>(name.c_str());
  }
  
  /**
   * @brief Returns a reference to the element at specified location pos. No bounds checking is performed.
   */
  inline double& operator[](size_t pos) {
    return (*this->vec_m)[pos];
  }
  
  /**
   * @brief Returns a constant  reference to the element at specified location pos. No bounds checking is performed.
   */
  inline const double& operator[](size_t n) const {
    return (*this->vec_m)[n];
  }
  
  /**
   * @brief Returns a reference to the element at specified location pos. Bounds checking is performed.
   */
  inline double& at(size_t n) {
    return this->vec_m->at(n);
  }
  
  /**
   * @brief Returns a constant reference to the element at specified location pos. Bounds checking is performed.
   */
  inline const double& at(size_t n) const {
    return this->vec_m->at(n);
  }
  
    /**
     * @brief Set an element at specified location pos.
     */
    inline void set(size_t pos, double value) {
      (*this->vec_m)[pos] = value;
    }

    /**
     * @brief Set an element at specified location pos.
     */
    inline double get(size_t pos) {
      return this->vec_m->at(pos);
    }
  
  /**
   * @brief  Returns a reference to the first element in the container.
   */
  reference front() {
    return this->vec_m->front();
  }
  
  /**
   * @brief  Returns a constant reference to the first element in the container.
   */
  const_reference front() const {
    return this->vec_m->front();
  }
  
  /**
   * @brief  Returns a reference to the last element in the container.
   */
  reference back() {
    return this->vec_m->back();
  }
  
  /**
   * @brief  Returns a constant reference to the last element in the container.
   */
  const_reference back() const {
    return this->vec_m->back();
  }
  
  /**
   * @brief Returns a pointer to the underlying data array.
   */
  pointer data() {
    return this->vec_m->data();
  }
  
  /**
   * @brief Returns a constant pointer to the underlying data array.
   */
  
  const_pointer data() const {
    return this->vec_m->data();
  }
  
  //iterators
  
  /**
   * @brief Returns an iterator to the first element of the vector.
   */
  iterator begin() {
    return this->vec_m->begin();
  }
  
  /**
   * @brief Returns an iterator to the element following the last element of the vector.
   */
  iterator end() {
    return this->vec_m->end();
  }
  
  /**
   * @brief Returns an constant iterator to the first element of the vector.
   */
  const_iterator begin() const {
    return this->vec_m->begin();
  }
  
  /**
   * @brief Returns an const iterator to the element following the last element of the vector.
   */
  const_iterator end() const {
    return this->vec_m->end();
  }
  
  /**
   * @brief Returns a reverse iterator to the first element of the reversed vector. It corresponds to the last element of the non-reversed vector.
   */
  reverse_iterator rbegin() {
    return this->vec_m->rbegin();
  }
  
  /**
   * @brief Returns a reverse iterator to the element following the last element of the reversed vector. It corresponds to the element preceding the first element of the non-reversed vector.
   */
  reverse_iterator rend() {
    return this->vec_m->rend();
  }
  
  /**
   * @brief Returns a constant reverse iterator to the first element of the reversed vector. It corresponds to the last element of the non-reversed vector.
   */
  const_reverse_iterator rbegin() const {
    return this->vec_m->rbegin();
  }
  
  /**
   * @brief Returns a constant reverse iterator to the element following the last element of the reversed vector. It corresponds to the element preceding the first element of the non-reversed vector.
   */
  const_reverse_iterator rend() const {
    return this->vec_m->rend();
  }
  
  //capacity
  
  /**
   * @brief Checks whether the container is empty.
   */
  bool empty() {
    return this->vec_m->empty();
  }
  
  /**
   * @brief Returns the number of elements.
   */
  size_type size() const {
    return this->vec_m->size();
  }
  
  /**
   * @brief Returns the maximum possible number of elements.
   */
  size_type max_size() const {
    return this->vec_m->max_size();
  }
  
  /**
   * @brief Reserves storage.
   */
  void reserve(size_type cap) {
    this->vec_m->reserve(cap);
  }
  
  /**
   * @brief Returns the number of elements that can be held in currently allocated storage.
   */
  size_type capacity() {
    return this->vec_m->capacity();
  }
  
  /**
   *  @brief Reduces memory usage by freeing unused memory.
   */
  void shrink_to_fit() {
    this->vec_m->shrink_to_fit();
  }
  
  //modifiers
  
  /**
   * @brief Clears the contents.
   */
  void clear() {
    this->vec_m->clear();
  }
  
  /**
   * @brief Inserts value before pos.
   */
  iterator insert(const_iterator pos, const double& value) {
    return this->vec_m->insert(pos, value);
  }
  
  /**
   * @brief Inserts count copies of the value before pos.
   */
  iterator insert(const_iterator pos, size_type count, const double& value) {
    return this->vec_m->insert(pos, count, value);
  }
  
  /**
   * @brief Inserts elements from range [first, last) before pos.
   */
  template< class InputIt >
  iterator insert(const_iterator pos, InputIt first, InputIt last) {
    return this->vec_m->insert(pos, first, last);
  }
  
  /**
   * @brief Inserts elements from initializer list ilist before pos.
   */
  
  iterator insert(const_iterator pos, std::initializer_list<double> ilist) {
    return this->vec_m->insert(pos, ilist);
  }
  
  /**
   * @brief Constructs element in-place.
   */
  template< class... Args >
  iterator emplace(const_iterator pos, Args&&... args) {
    return this->vec_m->emplace(pos, std::forward<Args>(args)...);
  }
  
  /**
   * @brief Removes the element at pos.
   */
  iterator erase(iterator pos) {
    return this->vec_m->erase(pos);
  }
  
  /**
   * @brief Removes the elements in the range [first, last).
   */
  iterator erase(iterator first, iterator last) {
    return this->vec_m->erase(first, last);
  }
  
  /**
   * @brief Adds an element to the end.
   */
  void push_back(const double&& value) {
    this->vec_m->push_back(value);
  }
  
  /**
   * @brief Constructs an element in-place at the end.
   */
  template< class... Args >
  void emplace_back(Args&&... args) {
    this->vec_m->emplace_back(std::forward<Args>(args)...);
  }
  
  /**
   * @brief Removes the last element.
   */
  void pop_back() {
    this->vec_m->pop_back();
  }
  
  /**
   * @brief Changes the number of elements stored.
   */
  void resize(size_t s) {
    this->vec_m->resize(s);
  }
  
  // /**
  //  * @brief Changes the number of elements stored.
  //  */
  // void resize(size_t s, const value_type& value) {
  //   this->vec_m->resize(s, value);
  // }
  // 
  /**
   * @brief Swaps the contents.
   */
  void swap(SharedVector& other) {
    this->vec_m->swap((*other.vec_m));
  }
  
  
  
private:
  
}; // end fims::Vector class

/**
 * @brief Comparison operator.
 */
bool operator==(const SharedVector& lhs,
                const SharedVector& rhs) {
  return (*lhs.vec_m) == (*rhs.vec_m);
}
// 
// namespace Rcpp {
// 
// template <> SEXP wrap<SharedVector>(const SharedVector& el) {
//   Rcpp::Language call("new", Symbol("SharedVector"));
//   return call.eval();
// }
// 
// template <> SharedVector as(SEXP s) {
//   try {
//     if (TYPEOF(s) != S4SXP) {
//       ::Rf_error("supplied object is not of type SharedVector.");
//     }
//     
//     Rcpp::S4 s4obj(s);
//     if (!s4obj.is("Rcpp_SharedVector")) {
//       ::Rf_error("supplied object is not of type SharedVector.");
//     }
//     
//     Rcpp::Environment env(s4obj);
//     Rcpp::XPtr<SharedVector> xptr(env.get(".pointer"));
//     
//     // build new SharedVector object with copied data
//     
//     return SharedVector((* xptr.get()));
//   } catch (...) {
//     
//     ::Rf_error("supplied object could not be converted to SharedVector.");
//   }
// }
// 
// 
// }

SharedVector as_shared_vector(SEXP s){
  return Rcpp::as<SharedVector>(s);
}

RCPP_EXPOSED_CLASS(SharedVector)


#endif
