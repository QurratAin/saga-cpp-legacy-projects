/*
!-------------------------------------------------------------------------!
!                                                                         !
!                   N A S   G R I D   B E N C H M A R K S                 !  
!                                                                         !
!			 J A V A	 V E R S I O N  		  !
!                                                                         !
!                            B M R E S U L T S                            !
!                                                                         !
!-------------------------------------------------------------------------!
!                                                                         !
!    BMResults implements result class used to report results of an NGB   !
!    task.                                                                !
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
import java.text.*;
import java.util.*;

public class BMResults extends NPB_JAV.BMInOut.BMResults{
  public Date date;
  public BMResults(){
    date=new Date(System.currentTimeMillis());
  }
  public BMResults(int bid){
    super(bid);
    date=new Date(System.currentTimeMillis());
  }

  public BMResults(String str) {
//No error checking since str suposed to be generated by toString()
//System.out.println("BMResults: str="+str);
    String tmpstr;
    int idx,fromidx,count;
    fromidx=0;
    idx=str.indexOf('.',fromidx);
    name=str.substring(fromidx,idx-fromidx);
//System.out.println("name="+name+" "+fromidx+" "+idx);
    fromidx=idx+1;
    idx=str.indexOf(';',fromidx);
    char tmpclss[]=new char[10];
    str.getChars(fromidx,fromidx+1,tmpclss,0);
    clss=tmpclss[0];
//System.out.println("clss="+clss+" "+fromidx+" "+idx);
    fromidx=idx+1;
    idx=str.indexOf(';',fromidx);   
    MachineName=str.substring(fromidx,idx);
//System.out.println("MachineName="+MachineName+" "+fromidx+" "+idx);
    fromidx=idx+1;
    idx=str.indexOf(';',fromidx);   
    PrLang=str.substring(fromidx,idx);
//System.out.println("PrLang="+PrLang+" "+fromidx+" "+idx);
    fromidx=idx+1;
    idx=str.indexOf(';',fromidx);   
    optype=str.substring(fromidx,idx);
//System.out.println("optype="+optype+" "+fromidx+" "+idx);
    fromidx=idx+1;
    idx=str.indexOf(';',fromidx);   
    tmpstr=str.substring(fromidx,idx);
    n1=Integer.parseInt(tmpstr);
    fromidx=idx+1;
    idx=str.indexOf(';',fromidx);   
    tmpstr=str.substring(fromidx,idx);
    n2=Integer.parseInt(tmpstr);
    fromidx=idx+1;
    idx=str.indexOf(';',fromidx);   
    tmpstr=str.substring(fromidx,idx);
    n3=Integer.parseInt(tmpstr);
//System.out.println("n1,n2,n3="+n1+" "+n2+" "+n3+" "+fromidx+" "+idx);
    fromidx=idx+1;
    idx=str.indexOf(';',fromidx);   
    tmpstr=str.substring(fromidx,idx);
//System.out.println("tmpstr="+tmpstr+" "+fromidx+" "+idx);
    niter=Integer.parseInt(tmpstr);
    fromidx=idx+1;
    idx=str.indexOf(';',fromidx);   
    tmpstr=str.substring(fromidx,idx);
//System.out.println("tmpstr="+tmpstr+" "+fromidx+" "+idx);
    Double d=Double.valueOf(tmpstr);
    time=d.doubleValue();
    fromidx=idx+1;
    idx=str.indexOf(';',fromidx);   
    tmpstr=str.substring(fromidx,idx);
//System.out.println("tmpstr="+tmpstr+" "+fromidx+" "+idx);
    d=Double.valueOf(tmpstr);
    mops=d.doubleValue();
    fromidx=idx+1;
    idx=str.indexOf(';',fromidx);   
    tmpstr=str.substring(fromidx,idx);
//System.out.println("tmpstr="+tmpstr+" "+fromidx+" "+idx);
    numthreads=Integer.parseInt(tmpstr);
    fromidx=idx+1;
    idx=str.indexOf(';',fromidx);   
    tmpstr=str.substring(fromidx,idx);
//System.out.println("tmpstr="+tmpstr+" "+fromidx+" "+idx);
    if(tmpstr.equals("true")) serial=true;
    else serial=false;
    fromidx=idx+1;
    idx=str.indexOf(';',fromidx);   
    tmpstr=str.substring(fromidx,idx);
//System.out.println("tmpstr="+tmpstr+" "+fromidx+" "+idx);
    pid=Integer.parseInt(tmpstr);
    fromidx=idx+1;
    tmpstr=str.substring(fromidx);
//System.out.println("tmpstr="+tmpstr+" "+fromidx+" "+idx);
    if(tmpstr.equals("true")) verified=1;
    else verified=0;
//    print();    
    date=new Date(System.currentTimeMillis());
  }
  public BMResults(String bname,
		   char   CLASS,
		   int    bn1, 
		   int    bn2,
		   int    bn3,
		   int    bniter,
		   double btime,
		   double bmops,
		   String boptype,
		   int    passed_verification,
		   boolean bserial,
	           int num_threads,
	           int bid){ 
      pid=bid;
      name=bname;
      clss=CLASS;
      n1=bn1;
      n2=bn2;
      n3=bn3;
      niter=bniter;
      time=btime;
      mops=bmops;
      optype=boptype;
      verified=passed_verification;
      serial=bserial;
      numthreads=num_threads;
    date=new Date(System.currentTimeMillis());
  }
}
