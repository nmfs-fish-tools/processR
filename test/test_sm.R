library(processR)
library(Rcpp)
library(RInside)

parent <- FALSE
child <- FALSE
rank_ <- 0
sm_name<- "my_shared_memory"

begin<-c(0,5)
end<-c(5,10)

args <- commandArgs()


for (i in 1:length(args)) {
  if (args[i] == "-parent") {
    parent <<- TRUE
  }
  
  
  
  if (args[i] == "-sm" && (i + 1) <= length(args)) {
    # print(args[i + 1])

    vector_name <<- args[i + 1]
    cat("vector_name = ")
    cat(vector_name)
    cat("\n")
  }
  
  if (args[i] == "-rank" && (i + 1) <= length(args)) {
    # print(args[i + 1])
    rank_ <<- as.integer(args[i + 1])
    cat("rank = ")
    cat(rank_)
    cat("\n")
  }
  
  if (args[i] == "-child") {
    child <<- TRUE
  }
  
  
}

if (parent == TRUE) {
  
  SMV<- new(processR::SharedVector)
  SMV$create(sm_name)
  SMV$resize(10)
  
  cat("\ninitial values:\n")
  for(i in 0:(SMV$size()-1)){
    SMV$set(i,3.1459)
    cat(SMV$get(i))
    cat(" ")
  }
  cat("\n")

  process <-
    new(processR::Process,
        "Rscript test_sm.R -child -rank 1")
  
  process2 <-
    new(processR::Process,
        "Rscript test_sm.R -child -rank 2")
  
  process$wait()
  process2$wait()
  
  cat("\nchild messages:\n\n")
  cat(process$get_message())
  cat("\n")
  cat(process2$get_message())
  
  cat("\nparent after concurrent modification:\n")
  for(i in 0:(SMV$size()-1)){
    cat(SMV$get(i))
    cat(" ")
  }
  cat("\n")
  
  SMV$destroy(sm_name)
  
} else{
  
  SMV<- new(processR::SharedVector)
  SMV$open(sm_name)
  
  for(i in begin[rank_]:(end[rank_]-1)){
    SMV$set(i, SMV$get(i)*(rank_*2))
    cat(SMV$get(i))
    cat(" ")
  }
  cat("\n")
  
  
}
if(parent == TRUE){
  
q()
}
# .GlobalEnv<-processR::CreateSharedEnvironment("global")
#sm_name<-"MySharedEnvironment"
#my_env<-processR::CreateSharedEnvironment(sm_name)
# # address(my_env)
#my_env[["my_list"]]<-list()
#ls(my_env, TRUE)
#
# your_env<-processR::readSharedEnvironment(sm_name)
#ls(your_env,TRUE)
# your_env[["your_list"]]<-list()
# my_env[["test_int"]]<-1
# ls(my_env, TRUE)
# ls(your_env, TRUE)
#
# my_env[["test_interprocess_write"]]<-c(0,0,0,0)
# cat(my_env[["test_interprocess_write"]])
#
# # rm(my_env)
# # rm(your_env)
# test<-function(){
#   cat("hi")
#   env<-processR::readSharedEnvironment("test_shared_environment")
#
#   if(processR.rank == 1){
#     env[["test_interprocess_write"]][1]<-1
#     env[["test_interprocess_write"]][2]<-1
#   }else{
#     env[["test_interprocess_write"]][3]<-2
#     env[["test_interprocess_write"]][4]<-2
#   }
# }
#
#
#
# #create a pool child processes
# pool <- processR::CreateProcessPool(1)#list()
#
# for (i in 1:length(pool)){#processR::HardwareConcurrency()) {
#   #creat a new child
#   ###pool[[i]] <- new(processR::Process)
#   #pass the entry function, environment, and child rank
#   pool[[i]]$start(test, environment(), i)
#   print(pool[[i]]$pid() )
# }
#
# print("waiting...")
# for (j in 1:length(pool)) {
#   #wait for the children to finish
#   pool[[j]]$wait()
# }
# #
# # env<-processR::readSharedEnvironment("test_shared_environment")
# #
# # cat(env[["test_interprocess_write"]])
# #
# #
