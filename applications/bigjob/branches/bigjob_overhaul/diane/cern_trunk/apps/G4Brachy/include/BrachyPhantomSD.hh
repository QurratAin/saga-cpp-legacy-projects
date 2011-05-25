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
// $Id: BrachyPhantomSD.hh,v 1.1 2008/10/03 16:39:43 moscicki Exp $
// GEANT4 tag $Name:  $
//
//    ********************************
//    *                              *  
//    *     BrachyPhantomSD.hh      *
//    *                              *
//    ********************************

#ifndef BrachyPhantomSD_h
#define BrachyPhantomSD_h 1

#include "G4VSensitiveDetector.hh"
#include "BrachyPhantomHit.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class BrachyPhantomSD : public G4VSensitiveDetector
{
public:
  BrachyPhantomSD(G4String name);
  ~BrachyPhantomSD();

  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist);
  void EndOfEvent(G4HCofThisEvent*HCE);
  void clear();
  void DrawAll();
  void PrintAll();
};
#endif


