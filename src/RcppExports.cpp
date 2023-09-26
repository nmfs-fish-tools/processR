// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

#ifdef RCPP_USE_GLOBAL_ROSTREAM
Rcpp::Rostream<true>&  Rcpp::Rcout = Rcpp::Rcpp_cout_get();
Rcpp::Rostream<false>& Rcpp::Rcerr = Rcpp::Rcpp_cerr_get();
#endif

// copyEnvironment
Rcpp::Environment copyEnvironment(const Rcpp::Environment& sourceEnv);
RcppExport SEXP _processR_copyEnvironment(SEXP sourceEnvSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const Rcpp::Environment& >::type sourceEnv(sourceEnvSEXP);
    rcpp_result_gen = Rcpp::wrap(copyEnvironment(sourceEnv));
    return rcpp_result_gen;
END_RCPP
}
// readEnvironmentFromSharedMemory
Rcpp::Environment readEnvironmentFromSharedMemory(const std::string& shared_memory_name);
RcppExport SEXP _processR_readEnvironmentFromSharedMemory(SEXP shared_memory_nameSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const std::string& >::type shared_memory_name(shared_memory_nameSEXP);
    rcpp_result_gen = Rcpp::wrap(readEnvironmentFromSharedMemory(shared_memory_name));
    return rcpp_result_gen;
END_RCPP
}
// RunProcess
void RunProcess(Rcpp::Function fun, Rcpp::Environment env);
RcppExport SEXP _processR_RunProcess(SEXP funSEXP, SEXP envSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::Function >::type fun(funSEXP);
    Rcpp::traits::input_parameter< Rcpp::Environment >::type env(envSEXP);
    RunProcess(fun, env);
    return R_NilValue;
END_RCPP
}
// HardwareConcurrency
size_t HardwareConcurrency();
RcppExport SEXP _processR_HardwareConcurrency() {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    rcpp_result_gen = Rcpp::wrap(HardwareConcurrency());
    return rcpp_result_gen;
END_RCPP
}
// CreateProcessPool
Rcpp::List CreateProcessPool(int size);
RcppExport SEXP _processR_CreateProcessPool(SEXP sizeSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< int >::type size(sizeSEXP);
    rcpp_result_gen = Rcpp::wrap(CreateProcessPool(size));
    return rcpp_result_gen;
END_RCPP
}

RcppExport SEXP _rcpp_module_boot_processR();

static const R_CallMethodDef CallEntries[] = {
    {"_processR_copyEnvironment", (DL_FUNC) &_processR_copyEnvironment, 1},
    {"_processR_readEnvironmentFromSharedMemory", (DL_FUNC) &_processR_readEnvironmentFromSharedMemory, 1},
    {"_processR_RunProcess", (DL_FUNC) &_processR_RunProcess, 2},
    {"_processR_HardwareConcurrency", (DL_FUNC) &_processR_HardwareConcurrency, 0},
    {"_processR_CreateProcessPool", (DL_FUNC) &_processR_CreateProcessPool, 1},
    {"_rcpp_module_boot_processR", (DL_FUNC) &_rcpp_module_boot_processR, 0},
    {NULL, NULL, 0}
};

RcppExport void R_init_processR(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
