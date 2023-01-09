Objectives: access a serial printing service:

a. [problem] incorrect printing on simultaneous requests
a.1. use the printing service from 2 different tasks simultaniously
- task1: print 000
- task2: print 111


b. [solution] treat the printing service as a critical section (shared resource) 
b.1 lock access to the service while it is being used:
2.1 use a mutex to protect the access
https://wokwi.com/projects/353380516849783809

2.2 use a queue to protect the access
 

