/*******************************************************************
* Copyright (C) 2011 Rensselaer Polytechnic Institute              *
* This work is licensed under the FreeBSD License.                 *
* Please see the included LICENSE file in the top level directory. *
*******************************************************************/
#ifndef _RPIMS_FEATURES_H
#define _RPIMS_FEATURES_H

#include "AnalogStream.h"
#include "FuncGen.h"
#include "DigitalStream.h"
#include "DigitalIO.h"

struct Features {
  struct AnalogStreamDevOps *analogStreamDevOps;
  struct FuncGenDevOps *funcGenDevOps;
  struct DigitalStreamDevOps *digitalStreamDevOps;
  struct DigitalIODevOps *digitalIODevOps;
};

#endif /* _RPIMS_FEATURES_H */
