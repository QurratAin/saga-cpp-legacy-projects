//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: BrachyEventAction.hh,v 1.1 2008/10/03 16:39:43 moscicki Exp $
// GEANT4 tag $Name:  $
//
// Code developed by:
//  S.Guatelli
//
//    **********************************
//    *                                *
//    *      BrachyEventAction.hh      *
//    *                                *
//    **********************************

#ifndef BrachyEventAction_h
#define BrachyEventAction_h 1

//#include <time.h>
#include "G4Timer.hh"
#include "G4UserEventAction.hh"
#include "globals.hh"

class BrachyEventAction : public G4UserEventAction
{
public:
  BrachyEventAction();
  ~BrachyEventAction();

public:
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);

private:
  G4String drawFlag; //Visualisation flag
  G4Timer  eventTimer;
  double   eventUserTimeAcc;
  double   taskNmb;
  int      eventNmb;
  //clock_t start, end;
  //double cpu_time_used;
  //double cpu_time_used_acc;

};

#endif



