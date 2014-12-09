#ifndef _GAPFINDER_H_
#define _GAPFINDER_H_

#include <string>
#include <vector>

#include "Overlap.h"

bool GapFinder_find_gaps(const std::vector<Overlap_T>& overlaps,
                         const QnameToOvlsMap_T& qname_to_ovls,
                         const std::string& query_name);

#endif
