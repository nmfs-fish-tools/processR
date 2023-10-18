
find.package('processR')
library(processR)
library(Rcpp)
library(RInside)
#library(rslurm)
list.files(getwd(), 'fims')




rnorm_wrapper<-function(){
  set.seed(processR.rank)
  return(rnorm(n=10, mean = 50, sd = processR.rank))
}

 


#create a pool child processes
pool <- processR::CreateProcessPool()#list()

for (i in 1:length(pool)){#processR::HardwareConcurrency()) {
  #creat a new child
  ###pool[[i]] <- new(processR::Process)
  #pass the entry function, environment, and child rank
  pool[[i]]$start(rnorm_wrapper, environment(), i)
  print(pool[[i]]$pid() )
}

results<-list()

print("waiting...")
for (j in 1:length(pool)) {
  #wait for the children to finish
  pool[[j]]$wait()
  
  #print the child output stream
  cat(pool[[j]]$get_message())
  
  #get the child's environment
  env<-pool[[j]]$get_environment()
  
  #access the process "results" list created by the test function
  #print(env[["processR.return"]])
  results[[j]]<-env[["processR.return"]]
}
print(results)
print("done.")

env<-pool[[1]]$get_environment()
