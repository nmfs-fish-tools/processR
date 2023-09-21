#ifndef PROCESSR_HPP
#define PROCESSR_HPP
// 
// #include <Rcpp.h>
// #include <boost/process.hpp>
// 
// 
// class Child{
//   boost::process::child child_m;
//   
// public:
//   int rank;
//   Rcpp::RawVector fun;//serialized function
//   Rcpp::RawVector env;//serialized environment
//   
//   
//   Child(Rcpp::Function fun, Rcpp::Environment env){
//   }
//   
//   
//   
//   void start(Rcpp::Function fun, Rcpp::Environment env){
//  
//   }
//   
//   void join(){
//     
//   }
//   
//   void Kill(){
//     
//   }
//   
// };
// 
// // [[Rcpp::export]]
// void Run(Rcpp::Function fun, Rcpp::Environment env){
//   std::string path ="/Users/mattadmin/rprojects/processR/src/RRunner.x";
//   Rcpp::RawVector serialized_fun;
//   {
//     Rcpp::Function serializeFunc("serialize");
//     serialized_fun = serializeFunc(fun);
//   }
//   
//   Rcpp::RawVector serialized_env;
//   {
//     Rcpp::Function serializeEnv("serialize");
//     serialized_env = serializeEnv(env);
//   }    
//   boost::process::ipstream child_output;
//   boost::process::opstream child_input;
//   
//   boost::process::child childProcess(path,
//          boost::process::std_in < child_input, // Redirect parent's input to child's output
//          boost::process::std_out > child_output // Redirect child's output to parent's input
//   );
//   
//   child_input << serialized_fun.size();
//   child_input<< serialized_fun;
// 
//   childProcess.join();
// }
// 
// /**
//  * Returns a list of children.
//  */
// Rcpp::List CreateFamily(int size){
//   
// }
// 


#endif