# ConsumerProducerProblem
For OS assignment 1

To compile and run:

  Will only work on systems that support POSIX library compalation, such as Linux based machines. 
  
  To compile each file with a g++ compiler:
  
    g++ producer.cpp -pthread -lrt -o producer.exe
    
    g++ consumer.cpp -pthread -lrt -o consumer.exe
    
  Then the files must be run at the same time, so for a Linux machine:
  
    ./producer.exe & ./consumer.exe &

 
  
  

Documentation Folder:


  documentation.pdf- A description of how the programs operate and the key concepts and ideas implemented. 
  
  Examples&Results.pdf - a screanshot showing what an example output would be if programs are compled and run correctly

  

Programs Folder:


  consumer.cpp - A C++ file that recived information from the shared memory and prints it out to the terminal when it has collected all of the information. 
  
  producer.cpp - A C++ file that pushes information into shared memory from a string that is stored in the file. 
  
