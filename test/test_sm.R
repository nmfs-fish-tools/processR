library(processR) 
library(Rcpp)
library(RInside)

# To run this example, type "Rscript test_sm.R -parent" from the command line

#default values
parent <- FALSE
rank_ <- 0
sm_name <- "my_shared_memory"

#get arguments
args <- commandArgs()


for (i in 1:length(args)) {
  
  if (args[i] == "-parent") {
    parent <<- TRUE
  }
  
  if (args[i] == "-sm" && (i + 1) <= length(args)) {
    sm_name <<- args[i + 1]
    cat("sm_name = ")
    cat(sm_name)
    cat("\n")
  }
  
  if (args[i] == "-rank" && (i + 1) <= length(args)) {
    rank_ <<- as.integer(args[i + 1])
    cat("rank = ")
    cat(rank_)
    cat("\n")
  }
  
}


#number of child processes
hc <- processR::HardwareConcurrency() - 1

#child segments
begin <- seq(from = 0, to = ((hc - 1) * 5), by = 5)
end <- seq(from = 5, to = ((hc) * 5), by = 5)


if (parent == TRUE) {#parent process
  
  #create a shared vector
  SMV <- new(processR::SharedVector)
  SMV$create(sm_name)
  SMV$resize((hc) * 5)
  
  
  cat(SMV$size())
  cat("\ninitial values:\n")
  for (i in 0:(SMV$size() - 1)) {
    SMV$set(i, 3.1459)
    cat(SMV$get(i))
    cat(" ")
  }
  cat("\n")
  
  #create a pool of child processes
  pool <- list()
  for (i in 1:hc) {
    cmd <- "Rscript test_sm.R -child -rank "
    cmd <- paste(cmd, i)
    
    pool[[i]] <-
      new(processR::Process,
          cmd)
    
  }
  
  #wait for children to complete
  for (i in 1:hc) {
    pool[[i]]$wait()
    pool[[i]]$get_message()
  }
 
  
  cat("\nparent after concurrent modification:\n")
  #print results
  for (i in 0:(SMV$size() - 1)) {
    cat(SMV$get(i))
    cat(" ")
  }
  cat("\n")
  
  #destroy shared memory
  SMV$destroy(sm_name)
  
} else{#child process
  #open shared vector
  SMV <- new(processR::SharedVector)
  SMV$open(sm_name)
  
  #modify segment of the shared vector
  for (i in begin[rank_]:(end[rank_] - 1)) {
    SMV$set(i, SMV$get(i) * (rank_ * 2))
    cat(SMV$get(i))
    cat(" ")
  }
  cat("\n")
}
