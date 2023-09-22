#include <filesystem>
#include <thread>
#include <Rcpp.h>
#include <sstream>
#include <chrono>
#include <boost/process.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>


namespace bp = boost::process;
namespace bip = boost::interprocess;



std::string getRLibraryPath() {
  Rcpp::Function SysGetenv("Sys.getenv");
  Rcpp::CharacterVector result = SysGetenv("R_LIBS_USER");
  if (result.size() == 1) {
    return Rcpp::as<std::string>(result[0]);
  } else {
    // Handle the case where the environment variable is not found or there are multiple paths
    return ""; // You can return an appropriate default value or handle the error as needed
  }
}

int check(std::string ss_env_name, std::string ss_fun_name) {
  bp::ipstream parent_input;
  bp::opstream parent_output;
  std::cout << "check....\n";
  std::cout << ss_env_name << "\n";
  
  
  bip::shared_memory_object env_shm(bip::open_only, ss_env_name.c_str(), bip::read_only);
  bip::shared_memory_object fun_shm(bip::open_only, ss_fun_name.c_str(), bip::read_only);
  
  bip::mapped_region env_region(env_shm, bip::read_only);
  bip::mapped_region fun_region(fun_shm, bip::read_only);
  
  const unsigned char* sharedCharArray_env = static_cast<const unsigned char*> (env_region.get_address());
  Rcpp::RawVector serialized_env(env_region.get_size());
  std::memcpy(serialized_env.begin(), sharedCharArray_env, env_region.get_size());
  
  const unsigned char* sharedCharArray_fun = static_cast<const unsigned char*> (fun_region.get_address());
  Rcpp::RawVector serialized_fun(fun_region.get_size());
  std::memcpy(serialized_fun.begin(), sharedCharArray_fun, fun_region.get_size());
  
  
  SEXP arg;
  PROTECT(arg = Rf_allocVector(RAWSXP, env_region.get_size()));
  std::memcpy(RAW(arg), &serialized_env[0], serialized_env.size() * sizeof (unsigned char));
  
  
  SEXP unser;
  PROTECT(unser = Rf_lang2(Rf_install("unserialize"), arg));
  
  int errorOccurred;
  SEXP ret = R_tryEval(unser, R_GlobalEnv, &errorOccurred);
  if (errorOccurred) {
    std::cout << "Error occurred unserializing environment." << std::endl;
  }
  
  
  Rcpp::Environment environment = Rcpp::as<Rcpp::Environment>(ret);
  
  
  // 
  SEXP arg2;
  PROTECT(arg2 = Rf_allocVector(RAWSXP, serialized_fun.size()));
  memcpy(RAW(arg2), &serialized_fun[0], serialized_fun.size() * sizeof (unsigned char));
  
  SEXP unser2;
  PROTECT(unser2 = Rf_lang2(Rf_install("unserialize"), arg2));
  
  int errorOccurred2;
  SEXP ret2 = R_tryEval(unser2, R_GlobalEnv, &errorOccurred2);
  if (errorOccurred2) {
    std::cout << "Error occurred unserializing function." << std::endl;
  }
  
  Rcpp::Function function = Rcpp::as<Rcpp::Function>(ret2);
  
  Rcpp::List l = Rcpp::as<Rcpp::List>(environment.ls(true));
  
  std::cout << "print env->\n";
  for (int i = 0; i < l.size(); i++) {
    std::cout << Rcpp::as<std::string>(l[i]) << "\n";
  }
  std::cout << "->done\n" << std::flush;
  function();
  
  std::cout << "done check.\n\n\n" << std::flush;
  
  
  
  return 0;
}

// [[Rcpp::export]]
Rcpp::Environment copyEnvironment(const Rcpp::Environment& sourceEnv) {
  Rcpp::Environment newEnv = Rcpp::new_env();
  
  Rcpp::CharacterVector names = sourceEnv.ls(true);
  
  for (int i = 0; i < names.size(); ++i) {
    std::string name = Rcpp::as<std::string>(names[i]);
    SEXP value = sourceEnv[name];
    
    // Assign the symbol and its value to the new environment
    newEnv.assign(name, value);
  }
  
  
  
  return newEnv;
}

void writeToSharedMemory(const std::string& shared_memory_name, const Rcpp::RawVector& serialized_env) {
  try {
    // Create or open the shared memory segment
    boost::interprocess::shared_memory_object shm(
        boost::interprocess::open_or_create, shared_memory_name.c_str(), boost::interprocess::read_write
    );
    
    // Set the size of the shared memory segment
    shm.truncate(serialized_env.size());
    
    // Map the shared memory segment into this process's address space
    boost::interprocess::mapped_region region(shm, boost::interprocess::read_write);
    
    // Get a pointer to the shared memory
    unsigned char* shared_memory_ptr = static_cast<unsigned char*> (region.get_address());
    
    std::copy(serialized_env.begin(), serialized_env.end(), shared_memory_ptr);
    // Copy the serialized data to shared memory
    // std::memcpy(shared_memory_ptr, serialized_env.begin(), serialized_env.size());
    
    // Note: Add error handling as needed
  } catch (const std::exception& e) {
    // Handle exceptions
    Rcpp::Rcout << e.what() << std::endl;
  }
}




class Process {
  std::shared_ptr<boost::process::child> child_m;
  
public:
  int rank;
  Rcpp::RawVector fun; //serialized function
  Rcpp::RawVector env; //serialized environment
  boost::process::ipstream child_output;
  boost::process::opstream child_input;
  std::thread thread_object;
  std::stringstream message;
  
  Process(const Process& other) :
    child_m((other.child_m)), rank(other.rank), fun(other.fun), env(other.env) {
  }
  
  
  Process() {
    
  }
  
  void start(const Rcpp::Function& fun, const Rcpp::Environment& env, const int& rank) {
    
    //Copy environment to shared memory
    Rcpp::Environment E = copyEnvironment(env);
    E.assign("process_rank", rank);
    std::string path = "";
    std::stringstream ss_path;
    
    ss_path<<getRLibraryPath()<<"/processR/bin";
    if (std::filesystem::exists(ss_path.str()) && std::filesystem::is_directory(ss_path.str())) {
      ss_path<<"/rchild";
      path = ss_path.str();
    } else {
      std::cout << "Libaray directory \""<<ss_path.str()<<" does not exist.\"" << std::endl;
    }
 
    std::stringstream sm_name_env;
    std::stringstream sm_name_fun;
    std::time_t t = std::time(0);
    sm_name_env << "processR_sm_env_" << t<<"_"<<rank;
    sm_name_fun << "processR_sm_fun_" << t<<"_"<<rank;
    // std::cout << sm_name_env.str() << "\n\n";
 
    
    // this->fun = fun;
    // this->env = E;
    
    Rcpp::Environment baseEnv("package:base");
    Rcpp::Function serializeFunc = baseEnv["serialize"];
    Rcpp::RawVector serialized_env = serializeFunc(E, R_NilValue);
    Rcpp::RawVector serialized_fun = serializeFunc(fun, R_NilValue);
    
    writeToSharedMemory(sm_name_env.str(), serialized_env);
    writeToSharedMemory(sm_name_fun.str(), serialized_fun);
    
    // Launch the child process

    this->child_m = std::make_shared<boost::process::child>(path,
                                                            boost::process::std_in < this->child_input,
                                                          boost::process::std_out > this->child_output
    );
    
    child_input << rank;
    child_input.flush();
    
    child_input << sm_name_env.str() << std::endl;
    child_input.flush();
    
    child_input << sm_name_fun.str() << std::endl;
    child_input.flush();
    
  }
  
  void join() {
    this->collect();
    this->child_m->join();
  }
  
  void wait() {
    this->collect();
    this->child_m->wait();
  }
  
  void terminate() {
    this->collect();
    this->child_m->terminate();
  }
  
  void collect(){
    std::string line;
    while (this->child_output && std::getline(this->child_output, line) && !line.empty()) {
      this->message << line << std::endl;
    }
  }
  
  std::string get_message(){
    return this->message.str();;
  }
  
  void print(){
    Rcpp::Rcout<<this->message.str();
  }
  
};

// [[Rcpp::export]]
void RunProcess(Rcpp::Function fun, Rcpp::Environment env) {
  
  std::string path = "";
  std::stringstream ss_path;
  
  ss_path<<getRLibraryPath()<<"/processR/bin";
  if (std::filesystem::exists(ss_path.str()) && std::filesystem::is_directory(ss_path.str())) {
    ss_path<<"/rchild";
    path = ss_path.str();
  } else {
    std::cout << "Libaray directory \""<<ss_path.str()<<" does not exist.\"" << std::endl;
  }
  
  
  std::stringstream sm_name_env;
  std::stringstream sm_name_fun;
  std::time_t t = std::time(0);
  sm_name_env << "processR_sm_env_" << t;
  sm_name_fun << "processR_sm_fun_" << t;
  std::cout << sm_name_env.str() << "\n\n";
  
  //Copy environment to shared memory
  Rcpp::Environment E = copyEnvironment(env);
  
  
  Rcpp::Environment baseEnv("package:base");
  Rcpp::Function serializeFunc = baseEnv["serialize"];
  Rcpp::RawVector serialized_env = serializeFunc(E, R_NilValue);
  Rcpp::RawVector serialized_fun = serializeFunc(fun, R_NilValue);
  
  writeToSharedMemory(sm_name_env.str(), serialized_env);
  writeToSharedMemory(sm_name_fun.str(), serialized_fun);
  
  // Launch the child process
  boost::process::ipstream child_output;
  boost::process::opstream child_input;
  boost::process::child childProcess(path,
                                     boost::process::std_in < child_input,
                                                              boost::process::std_out > child_output
  );
  
  int rank = 0;
  child_input << rank;
  child_input.flush();
  
  child_input << sm_name_env.str() << std::endl;
  child_input.flush();
  
  child_input << sm_name_fun.str() << std::endl;
  child_input.flush();
  
  std::string line;
  while (child_output && std::getline(child_output, line) && !line.empty()) {
    std::cout << line << std::endl;
  }
  
  // // Wait for the child process to finish
  childProcess.wait();
  
  
}

RCPP_EXPOSED_CLASS(Process)
  
  RCPP_MODULE(processR) {
    Rcpp::class_<Process>("Process")
    .constructor()
    .method("start", &Process::start)
    .method("join", &Process::join)
    .method("wait", &Process::wait)
    .method("terminate", &Process::terminate)
    .method("get_message", &Process::get_message)
    .method("print", &Process::print)
    .field("rank", &Process::rank);
  }

// // [[Rcpp::export]]
//Process CreateProcess(){
//  return Process();
//}


/**
 * Returns a list of children.
 */

//[[Rcpp::export]]
size_t HardwareConcurency(){
  return std::thread::hardware_concurrency();
}

// [[Rcpp::export]]
Rcpp::List CreateProcessPool(int size) {
  
  Rcpp::List l(size);
  
  for (int i = 0; i < l.size(); i++) {
    
    l.at(i) = Rcpp::wrap(Process());
    // Rcpp::as<Process>(l.at(i)).rank = i;
  }
  
  return l;
}

