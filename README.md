# im

----------------2016-07-02 update log----------------------

dir structure 
  conf dir
  core dir
  im.c file
  test dir

dir structure specification
  conf dir: place makefile、compile file 、logfile
  
  core dir: place core source code, include log dir、net dir、other file
      log dir: place log source code
      net dir: place net relevant source code
      other file: basic source code  
      
  im.c file: main() source code
  
  test dir: place test source code
  
instructions
  start program
    1.cd XX/XX/conf dir
    2.make run
    
  stop program
     1.cd XX/XX/conf dir
     2.make stop
     
  clear compile file
     1.cd XX/XX/conf dir
     2.make clear
     
  view log 
     1.cd XX/XX/conf/logfile dir
     2.log type
        debug
        error
        dump
  
----------------2016-07-16 update log----------------------
add new dir
  pyutil: place python file
  
    
                  
