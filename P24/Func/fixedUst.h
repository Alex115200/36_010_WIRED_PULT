
//

#ifndef FIXED_UST_H
#define FIXED_UST_H

//

#define MakeAdrPrm(Ngrp, Nprm) ((Nprm & 0x7f) | ((Ngrp & 0x7f) << 7))

//
 
#define CONTRAST_GRPPRM          MakeAdrPrm(56, 0)

#define SAVE_MPFILE1_GRPPRM      MakeAdrPrm(38, 1)
#define SAVE_MPFILE2_GRPPRM      MakeAdrPrm(38, 2)
#define RESTORE_MPFILE1_GRPPRM   MakeAdrPrm(38, 3)
#define RESTORE_MPFILE2_GRPPRM   MakeAdrPrm(38, 4)

#define STATUS_EDIT_GRPPRM       MakeAdrPrm(48, 0)

#define DEFAULT_CONTRAST         30

#endif // FIXED_UST_H