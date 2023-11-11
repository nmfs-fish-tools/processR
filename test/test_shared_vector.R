

library(processR)
library(Rcpp)
library(RInside)


args <- commandArgs()

rank<-0
for(i in 1:length(args)){
  if(args[i] == "-sm" && (i+1) <= length(args)){
    print(args[i+1])
    vector_name<<-args[i+1]
  }
  
  if(args[i] == "-rank" && (i+1) <= length(args)){
    print(args[i+1])
    rank_<<- as.integer(args[i+1])
  }
  
  if(args[i] == "-child"){
    print(args[i+1])
    child<<-TRUE
  }
  
}

test<-function(){
  cat("i'm a child\n")
  cat("looping through shared_vector at index ")
  cat(rank_)
  cat("\n")
}

if(child == TRUE){
test()
}else{
  
  cat("i'm a parent\n")
}

