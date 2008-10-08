/*
!-------------------------------------------------------------------------!
!                                                                         !
!                   N A S   G R I D   B E N C H M A R K S                 !  
!                                                                         !
!			 J A V A	 V E R S I O N  		  !
!                                                                         !
!                               A R C H E A D                             !
!                                                                         !
!-------------------------------------------------------------------------!
!                                                                         !
!    ArcHead implements on server side a data source object transmitting  !
!    data along a DFG arc.                                                !
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
package tasks.DGraph;

import java.io.*;

public class ArcHead implements Serializable{
  public int	id=-1;
  public String	name=null;
  public SparseA spa=null;
  public BMRequest req=null;
	 
  public ArcHead(){
  }
  public ArcHead(int pid){
    id=pid;
  }
  public ArcHead(BMRequest rq){
    req=new BMRequest(rq);
    req.tmSent=rq.tmSent;
    req.tmReceived=rq.tmReceived;
    id=req.pid;
    spa=rq.spa;
  }
  public ArcHead(ArcHead arh){
    req=new BMRequest(arh.req);
    req.tmSent=arh.req.tmSent;
    req.tmReceived=arh.req.tmReceived;
    id=req.pid;
    spa=new SparseA(arh.spa);
  }
  public void Show(){
    System.out.println("ArcHead: "+id+"."+name);
  }
  public void ShowData(int DetailsLevel){spa.Show();}
  public void finalize() throws Throwable{
    spa=null;
    req.spa=null;
    req=null;
    super.finalize();
  }
}
