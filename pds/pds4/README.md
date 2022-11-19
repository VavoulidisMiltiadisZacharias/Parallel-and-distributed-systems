# pds4
A C based implementation of some parallel programming techniques

## **Table of contents**
- [General info](#general-info)
- [Setup](#setup)
- [Complete stuff](#complete-stuff)
- [Contributors](#contributors)
- [Contact](#contact)

<br/>

### **General info:**
This is an implementation of [vantage point tree](https://en.wikipedia.org/wiki/Vantage-point_tree) using C programming language. Feel free to download and edit the code. 

### **Setup:**
*This section will be filled with more useful info soon*.
<br/>
<br/>

### **Complete stuff:**
The following is a short explanation of each version.<br/>

- 0_Knn:<br/>Contains the source code for the (only) sequential implementation. This implementation takes place just for reference reasons.<br/><br/>
- 1_B_Knn_PThreads:<br/>Contains the source code for the parallel implementation using pthreads. The "inner" and "outer" set is set in parallel.<br/><br/>
- 1_KnnPThreads:<br/>Contains the source code for the parallel implementation using pthreads. The distance of each point from vantage point are calculated in parallel.<br/><br/>
- 2_Threshold:<br/>Contains the source code for the parallel implementation using p threads. A threshold on the number of threads used and on each thread's workload is set for optimization reasons.<br/><br/>
- 3_All_kNN:<br/>Contains the source code for the parallel implementation using pthreads. All kNNs from all points (choose different vantage point eithin each iteration).<br/><br/>
- 4_MPI:<br/>Contains the source code for the parallel implementation using MPI.
<br/>

### **Contributors:**
Vavoulidis Miltiadis - Zacharias.
<br/>
<br/>

### **Contact:**
mvavoulidis@gmail.com

In case of any contact me attempt your e-mail has to fulfil the following requirements:
1. Set e-mail subject as "pds4 filename", where "filename" is the filename for which you sent the email. Furthermore, add any info you consider as useful.
2. Write your name.

