library(processR)
# library(Rcpp)
# library(RInside)


P<-new(Process, "r --slave --no-save -e cat(R.home())")
P$wait()
cat(P$get_message())

v <- 10

test <- function() {
  #declare a list globally
  results<<-list()
  
  for (i in 1:2) {
    #modify a global variable
    v <- v + 1
    #multiply i by the processR.rank
    results[[i]]<<-i * processR.rank
  }
  
  for (i in 1:2) {
    #print to standard out (to be captured after completion)
    print(paste("rank", paste(processR.rank, " -> hi from the test function")))
  }
  
}



#create a pool children
pool <- list()

for (i in 1:processR::HardwareConcurrency()) {
  #creat a new child
  
  pool[[i]] <- new(processR::Process)
  #pass the entry function, environment, and child rank
  pool[[i]]$start(test, environment(), i)
  print(pool[[i]]$pid() )
}


print("waiting...")
for (j in 1:10) {
  #wait for the children to finish
  pool[[j]]$wait()
  
  #print the child output stream
  cat(pool[[j]]$get_message())
  
  #get the child's environment
  env<-pool[[j]]$get_environment()
  
  #access the global "results" list created by the test function
  str <- paste0(unlist(env[["results"]]), collapse = "\n")
  cat(str)
  cat("\n");
}

print("done.")

