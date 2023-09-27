loadModule(module = "processR", TRUE)

CreateProcessPool<-function(size = ProcessR::HardwareConcurrency()){
  pool<-list()
  
  for(i in 1:size){
    pool[[i]]<-new(processR::Process)
    pool[[i]]$rank<-i
  }
  return(pool)
}