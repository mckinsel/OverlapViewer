#ifndef _LASREADER_H_
#define _LASREADER_H_

#include <string>
#include <vector>

#include "Overlap.h"

void LASReader_create_ovl_list(std::vector<Overlap_T>* ovl_list, std::string las_name,
                               std::string db_name);

#endif
