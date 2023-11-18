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
  # SMV2 <- new(processR::SharedVector)
  # SMV2$create(sm_name)
  # SMV2$resize((hc) * 5)


  #create a shared vector
  SMV <- new(processR::SharedVector)
  SMV$create(sm_name)
  SMV$resize((hc) * 5)
  
  show(SMV)
  
  SML <- new(processR::SharedList)
  SML$create("SML")
  SML$set(sm_name, SMV)

  
  # SML2 <- new(processR::SharedList)
  # SML2$open("SML")
  # SMV2<-SML2$get(sm_name)
  # pi<- 3.1549
  # print(typeof(pi))
  # SML2$set("PI",3.1549)
  # print(SML2$get("PI"))
  # q()
  # SML2$set("double", 3,1459)
  # PI_ <-SML$get("double")
  # print(PI_)
  # print(SMV2$size())
  # print(typeof(SMV2))
 # print(SMV2$size())
 

  
  print("still parent")
  cat(SMV$size())
  cat("\ninitial values:\n")
  for (i in 0:(SMV$size() - 1)) {
    SMV$set(i, 3.1459)
    cat(SMV$get(i))
    cat(" ")
  }
  cat("\n")

  print("launching children")
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
   cat(pool[[i]]$get_message())
    cat("\n")
  }

  cat("\nparent after concurrent modification:\n")
  #print results
  for (i in 0:(SMV$size() - 1)) {
    cat(SMV$get(i))
    cat(" ")
  }
  cat("\n\n")
  cat(SML$get("PI"))
  cat("\n")
  #destroy shared memory
  SMV$destroy(sm_name)
  
} else{#child process

  
  print("in child")
  
  # open shared vector
  # SMV <- new(processR::SharedVector)
  # SMV$open(sm_name)
  # 
  SML <- new(processR::SharedList)
  SML$open("SML")

  SMV<-SML$get(sm_name)
  SML$set("PI",3.1549)
  # print(SMV2$size())
  show(SMV)
  
  print("still child")
  #modify segment of the shared vector
  for (i in begin[rank_]:(end[rank_] - 1)) {
    SMV$set(i, SML$get("PI")*(2*rank_))
    cat(SMV$get(i))
    cat(" ")
  }
  cat("\n")
  print("still still child")
  # SL<- new(processR::SharedList)
  # SL$open("shared_list")
  # 
  # print(SL$get("env"))
  
}
