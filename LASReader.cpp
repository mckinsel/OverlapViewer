#include "LASReader.h"
#include <cstring>
#include "dalign/DB.h"
#include "dalign/align.h"

void create_id_to_name(std::string db_name, std::map<int, std::string>* id_to_name)
{
  char* c_db_name = (char*)malloc(db_name.size() * sizeof(char));
  strcpy(c_db_name, db_name.c_str());

  HITS_DB    _db, *db = &_db;
  FILE* dbfile;
  Open_DB(c_db_name, db);
  dbfile = Fopen(c_db_name, "r");
  
  int nfiles, last;
  char  prolog[1000], fname[1000];
  fscanf(dbfile,DB_NFILE,&nfiles);
  fscanf(dbfile,DB_FDATA,&last,fname,prolog);

  HITS_READ  *reads;
  int         i, fcount, nblock, ireads, breads;
  int64       size, totlen;

  size = 400*1000000ll;
  reads = db->reads;

  ireads = 0;
  breads = 0;
  totlen = 0;
  nblock = 1;
  fcount = 0;

  for (i = 0; i < db->nreads; i++) {   
    int        len, flags;
    HITS_READ *r;
    
    r     = reads + i;
    len   = r->end - r->beg;
    flags = r->flags;

    ireads += 1;
    breads += 1;
    totlen += len;
    
    if (totlen >= size || ireads >= READMAX) {
      ireads = 0;
      totlen = 0;
      nblock += 1;
    }
    
    char full_name[1000];
    sprintf(full_name, "%s/%d/%d_%d", prolog, r->origin, r->beg, r->end); 
    (*id_to_name)[breads] = full_name;

    if (i+1 >= last && ++fcount < nfiles) {
      fscanf(dbfile,DB_FDATA,&last,fname,prolog);
    }
  } 
  
  fclose(dbfile);
  Close_DB(db);
  free(c_db_name);
}

void LASReader_create_ovl_list(std::string las_name, std::string db_name,
                               std::vector<Overlap_T>* ovl_list)
{

  std::map<int, std::string> id_to_name;
  create_id_to_name(db_name, &id_to_name);

  FILE   *input;
  int64   novl;
  int     tspace, tbytes, small;
  
  char* c_las_name = (char*)malloc(las_name.size() * sizeof(char));
  strcpy(c_las_name, las_name.c_str());
  input = Fopen(c_las_name, "r");
  free(c_las_name);

  fread(&novl,sizeof(int64),1,input);
  fread(&tspace,sizeof(int),1,input);

  if (tspace <= TRACE_XOVR) { small  = 1;
    tbytes = sizeof(uint8);
  } else {
    small  = 0;
    tbytes = sizeof(uint16);
  }

  int tmax  = 5000;
  uint16 *trace = (uint16 *) Malloc(sizeof(uint16)*tmax,"Allocating trace vector");

  Overlap   _ovl, *ovl = &_ovl;
  int j = 0;
  for (j = 0; j < novl; j++) {
    Read_Overlap(input,ovl);
    if (ovl->path.tlen > tmax) {
      tmax = ((int) 1.2*ovl->path.tlen) + 100;
      trace = (uint16 *) Realloc(trace,sizeof(uint16)*tmax,"Allocating trace vector");
    }
    ovl->path.trace = (void *) trace;
    Read_Trace(input,ovl,tbytes);

    Overlap_T over = {"","",0,0,0,0,0,0,false};

    over.name_a = id_to_name[ovl->aread];
    over.name_b = id_to_name[ovl->bread];
    over.start_a = ovl->path.abpos;
    over.start_b = ovl->path.bbpos;
    over.end_a = ovl->path.aepos;
    over.end_b = ovl->path.bepos;
    over.length_a = ovl->alen;
    over.length_b = ovl->blen;
    over.forward = COMP(ovl->flags);
    
    ovl_list->push_back(over);
  }
  
  fclose(input);
  free(trace);
}
