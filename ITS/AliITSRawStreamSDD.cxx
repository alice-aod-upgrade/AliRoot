/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

///////////////////////////////////////////////////////////////////////////////
//
// This is a class for reading ITS SDD raw data files and providing
// information about digits
//
///////////////////////////////////////////////////////////////////////////////

#include "AliITSRawStreamSDD.h"

ClassImp(AliITSRawStreamSDD)


const Int_t AliITSRawStreamSDD::kDDLsNumber;
const Int_t AliITSRawStreamSDD::kModulesPerDDL;
const Int_t AliITSRawStreamSDD::kDDLModuleMap[kDDLsNumber][kModulesPerDDL] = {
  {240,241,242,246,247,248,252,253,254,258,259,260,264,265,266,270,271,272,276,277,278,-1},
  {243,244,245,249,250,251,255,256,257,261,262,263,267,268,269,273,274,275,279,280,281,-1},
  {282,283,284,288,289,290,294,295,296,300,301,302,306,307,308,312,313,314,318,319,320,-1},
  {285,286,287,291,292,293,297,298,299,303,304,305,309,310,311,315,316,317,321,322,323,-1},
  {324,325,326,327,332,333,334,335,340,341,342,343,348,349,350,351,356,357,358,359,364,365},
  {328,329,330,331,336,337,338,339,344,345,346,347,352,353,354,355,360,361,362,363,368,369},
  {366,367,372,373,374,375,380,381,382,383,388,389,390,391,396,397,398,399,404,405,406,407},
  {370,371,376,377,378,379,384,385,386,387,392,393,394,395,400,401,402,403,408,409,410,411},
  {412,413,414,415,420,421,422,423,428,429,430,431,436,437,438,439,444,445,446,447,452,453},
  {416,417,418,419,424,425,426,427,432,433,434,435,440,441,442,443,448,449,450,451,456,457},
  {454,455,460,461,462,463,468,469,470,471,476,477,478,479,484,485,486,487,492,493,494,495},
  {458,459,464,465,466,467,472,473,474,475,480,481,482,483,488,489,490,491,496,497,498,499}};


AliITSRawStreamSDD::AliITSRawStreamSDD(AliRawReader* rawReader) :
  AliITSRawStream(rawReader)
{
// create an object to read ITS SDD raw digits

  fRawReader->Select(2);
}


Bool_t AliITSRawStreamSDD::Next()
{
// read the next raw digit
// returns kFALSE if there is no digit left

  fPrevModuleID = fModuleID;
  if (!fRawReader->ReadNextInt(fData)) return kFALSE;
  
  UInt_t relModuleID = (fData >> 25) & 0x0000007F;
  fModuleID = kDDLModuleMap[fRawReader->GetDDLID()][relModuleID];
  fCoord1 = (fData >> 16) & 0x000001FF;
  fCoord2 = (fData >> 8) & 0x000000FF;
  fSignal = fData & 0x000000FF;

  return kTRUE;
}
