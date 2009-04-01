int xdim = 60;                                                                                                                                                                                                  
int ydim = 40;                                                                                                                                                                                                  
real p = .25;                                                                                                                                                                                                    
                                                                                                                                                                                                                
import stats;                                                                                                                                                                                                   
                                                                                                                                                                                                                
unitsize (5);                                                                                                                                                                                                   
                                                                                                                                                                                                                
for (int i=0; i<=xdim; ++i)                                                                                                                                                                                     
  for (int j=0; j<=ydim; ++j)                                                                                                                                                                                   
    dot ((i,j), linewidth(.8));                                                                                                                                                                                 
                                                                                                                                                                                                                
for (int i=0; i<xdim; ++i)                                                                                                                                                                                      
  for (int j=0; j<=ydim; ++j)                                                                                                                                                                                   
    if (unitrand() < p) draw ((i,j)--(i+1,j));                                                                                                                                                                  
                                                                                                                                                                                                                
for (int i=0; i<=xdim; ++i)                                                                                                                                                                                     
  for (int j=0; j<ydim; ++j)                                                                                                                                                                                    
    if (unitrand() < p) draw ((i,j)--(i,j+1));                                                                                                                                                                  

