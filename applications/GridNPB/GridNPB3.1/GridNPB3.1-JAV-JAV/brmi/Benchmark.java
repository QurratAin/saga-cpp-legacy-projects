/*
!-------------------------------------------------------------------------!
!                                                                         !
!                   N A S   G R I D   B E N C H M A R K S                 !  
!                                                                         !
!			 J A V A	 V E R S I O N  		  !
!                                                                         !
!                            B E N C H M A R K                            !
!                                                                         !
!-------------------------------------------------------------------------!
!                                                                         !
!    Benchmark declares interface to benchmark server.                    !
!                                                                         !
!    Permission to use, copy, distribute and modify this software         !
!    for any purpose with or without fee is hereby granted.               !
!    We request, however, that all derived work reference the             !
!    NAS Grid Benchmarks 3.0 or GridNPB3.0. This software is provided     !
!    "as is" without expressed or implied warranty.                       ! 
!                                                                         !
!    Information on GridNPB3.0, including the concept of                  !
!    the NAS Grid Benchmarks, the specifications, source code,            !
!    results and information on how to submit new results,                !
!    is available at:                                                     !
!                                                                         !
!         http://www.nas.nasa.gov/Software/NPB                            !
!                                                                         !
!    Send comments or suggestions to  ngb@nas.nasa.gov                    !
!    Send bug reports to              ngb@nas.nasa.gov                    !
!                                                                         !
!          E-mail:  ngb@nas.nasa.gov                                      !
!          Fax:     (650) 604-3957                                        !
!                                                                         !
!-------------------------------------------------------------------------!
! GridNPB3.0 Java version                   				  !
!	   M. Frumkin							  !
!-------------------------------------------------------------------------!
*/
package brmi;

import tasks.DGraph.*;

import java.rmi.Remote; 
import java.rmi.RemoteException; 

public interface Benchmark extends Remote{ 
    BMRequest req=null;
    BMResults res=null;
    String benchtype=null;
    public void startBenchmark(BMRequest req,BMResults res) 
                              throws RemoteException;
    public BMResults getResults(BMRequest req) 
                               throws RemoteException;   
    public int SendData(BMRequest req,BMResults res) 
                        throws RemoteException;
}
