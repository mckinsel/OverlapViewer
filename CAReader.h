#ifndef _CAREADER_H_
#define _CAREADER_H_

#include <string>
#include <vector>

#include "Overlap.h"

void CAReader_create_ovl_list(std::string fastq_filename, std::string gkp_store_name,
                              std::string ovl_store_name, std::vector<Overlap_T>* ovl_list);
    
#endif
