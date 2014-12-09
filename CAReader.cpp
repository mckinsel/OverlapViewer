#include "AS_global.H"
#include "CAReader.h"
#include <stdint.h>
#include <zlib.h>

#include <string>
#include <vector>

#include "kseq.h"

#include "AS_OVS_overlapStore.H"
#include "AS_OBT_overlaps.H"


KSEQ_INIT(gzFile, gzread)

void read_fastq(std::string fastq_filename, std::vector<std::string>* fastq_names,
                std::vector<size_t>* fastq_lengths)
{
  gzFile fastq_file = gzopen(fastq_filename.c_str(), "r");
  kseq_t* seq;
  int l;
  seq = kseq_init(fastq_file);
  while((l = kseq_read(seq)) >= 0) {
    fastq_names->push_back((std::string) seq->name.s);
    fastq_lengths->push_back(seq->seq.l);
  }
  kseq_destroy(seq);
  gzclose(fastq_file);
}

void CAReader_create_ovl_list(std::string fastq_filename, std::string gkp_store_name,
                              std::string ovl_store_name, std::vector<Overlap_T>* ovl_list)
{
  std::vector<std::string> fastq_names;
  std::vector<size_t> fastq_lengths;
  read_fastq(fastq_filename, &fastq_names, &fastq_lengths);

  gkStore* gkp_store = new gkStore(gkp_store_name.c_str(), false, false);
  OverlapStore *ovl_store = AS_OVS_openOverlapStore(ovl_store_name.c_str());

  uint32_t iidMin = 0;
  uint32_t iidMax = gkp_store->gkStore_getNumFragments();

  uint32_t ovl_len = 0;
  uint32_t ovl_max = 25000;

  OVSoverlap* overlaps = new OVSoverlap [ovl_max];

  for(uint32_t iid=iidMin; iid < iidMax; iid++) {
    loadOverlaps(iid, overlaps, ovl_len, ovl_max, ovl_store, NULL);

    for(uint32_t i=0; i<ovl_len; i++) {
      Overlap_T ovl = {"","",0,0,0,0,0,0,false};

      ovl.name_a = fastq_names[overlaps[i].a_iid - 1];
      ovl.name_b = fastq_names[overlaps[i].b_iid - 1];
      
      ovl.start_a = overlaps[i].dat.obt.a_beg;
      ovl.start_b = overlaps[i].dat.obt.b_beg;

      ovl.end_a = overlaps[i].dat.obt.a_end;
      ovl.end_b = overlaps[i].dat.obt.b_end_lo | (overlaps[i].dat.obt.b_end_hi << 9);
      
      if(ovl.end_b < ovl.start_b) {
        int tmp = ovl.start_b;
        ovl.start_b = ovl.end_b;
        ovl.end_b = tmp;
      }

      ovl.length_a = fastq_lengths[overlaps[i].a_iid - 1];
      ovl.length_b = fastq_lengths[overlaps[i].b_iid - 1];
      
      ovl.forward = overlaps[i].dat.obt.fwd; 

      ovl_list->push_back(ovl);
    }
  }

  delete[] overlaps;
  AS_OVS_closeOverlapStore(ovl_store);
  delete gkp_store;

}

