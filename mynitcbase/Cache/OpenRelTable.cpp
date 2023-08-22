
#include "OpenRelTable.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
// int OpenRelTable::getRelId(char relName[ATTR_SIZE]) {

//   // if relname is RELCAT_RELNAME, return RELCAT_RELID
//   if(!strcmp(relName,RELCAT_RELNAME))return RELCAT_RELID;
//   if(!strcmp(relName, ATTRCAT_RELNAME))return ATTRCAT_RELID;

//   // if relname is ATTRCAT_RELNAME, return ATTRCAT_RELID

//   return E_RELNOTOPEN;
// }

OpenRelTableMetaInfo OpenRelTable::tableMetaInfo[MAX_OPEN];

OpenRelTable::OpenRelTable() {

  // initialise all values in relCache and attrCache to be nullptr and all entries
  // in tableMetaInfo to be free
  for(int i = 0;i<MAX_OPEN;i++)
  {
    RelCacheTable::relCache[i] = nullptr;
    AttrCacheTable::attrCache[i] = nullptr;
    tableMetaInfo[i].free = true;
  }


  // load the relation and attribute catalog into the relation cache (we did this already)
  RecBuffer relCatBlock(RELCAT_BLOCK);

  Attribute relCatRecord[RELCAT_NO_ATTRS];
  relCatBlock.getRecord(relCatRecord, RELCAT_SLOTNUM_FOR_RELCAT);

  struct RelCacheEntry relCacheEntry;
  RelCacheTable::recordToRelCatEntry(relCatRecord, &relCacheEntry.relCatEntry);
  relCacheEntry.recId.block = RELCAT_BLOCK;
  relCacheEntry.recId.slot = RELCAT_SLOTNUM_FOR_RELCAT;

  // allocate this on the heap because we want it to persist outside this function
  RelCacheTable::relCache[RELCAT_RELID] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[RELCAT_RELID]) = relCacheEntry;

  /**** setting up Attribute Catalog relation in the Relation Cache Table ****/
  relCatBlock.getRecord(relCatRecord,RELCAT_SLOTNUM_FOR_ATTRCAT);
  RelCacheTable::recordToRelCatEntry(relCatRecord,&relCacheEntry.relCatEntry);
  relCacheEntry.recId.block = RELCAT_BLOCK;
  relCacheEntry.recId.slot = RELCAT_SLOTNUM_FOR_ATTRCAT;
  RelCacheTable::relCache[ATTRCAT_RELID] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[ATTRCAT_RELID]) = relCacheEntry;

  // load the relation and attribute catalog into the attribute cache (we did this already)
  RecBuffer attrCatBlock(ATTRCAT_BLOCK);

  Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
  AttrCacheEntry *head,*temp;
  head = temp = (AttrCacheEntry*) malloc (sizeof(AttrCacheEntry));
  for(int i = 0;i<5;i++)
  {
    temp->next = (AttrCacheEntry*) malloc (sizeof(AttrCacheEntry));
    temp = temp->next;
  }
  temp->next = NULL;
  temp = head;
  for(int i = 0;i<6;i++)
  {
    attrCatBlock.getRecord(attrCatRecord,i);
    AttrCacheTable::recordToAttrCatEntry(attrCatRecord,&temp->attrCatEntry);
    temp->recId.block = ATTRCAT_BLOCK;
    temp->recId.slot = i;
    temp = temp->next;
  }
  AttrCacheTable::attrCache[RELCAT_RELID] = /* head of the linked list */head;
  head = NULL;

  /**** setting up Attribute Catalog relation in the Attribute Cache Table ****/
  head = temp = (AttrCacheEntry*) malloc (sizeof(AttrCacheEntry));
  for(int i = 0;i<5;i++)
  {
    temp->next = (AttrCacheEntry*) malloc (sizeof(AttrCacheEntry));
    temp = temp->next;
  }
  temp->next = NULL;
  temp = head;
  for(int i = 6;i<12;i++)
  {
    attrCatBlock.getRecord(attrCatRecord,i);
    AttrCacheTable::recordToAttrCatEntry(attrCatRecord,&temp->attrCatEntry);
    temp->recId.block = ATTRCAT_BLOCK;
    temp->recId.slot = i;
    temp = temp->next;
  }
  AttrCacheTable::attrCache[ATTRCAT_RELID] = /* head of the linked list */head;

  /************ Setting up tableMetaInfo entries ************/

  // in the tableMetaInfo array
  //   set free = false for RELCAT_RELID and ATTRCAT_RELID
  //   set relname for RELCAT_RELID and ATTRCAT_RELID
  tableMetaInfo[RELCAT_RELID].free = false;
  tableMetaInfo[ATTRCAT_RELID].free = false;
  strcpy(tableMetaInfo[RELCAT_RELID].relName,RELCAT_RELNAME);
  strcpy(tableMetaInfo[ATTRCAT_RELID].relName,ATTRCAT_RELNAME);
}
int OpenRelTable::getFreeOpenRelTableEntry() {
  for(int i = 0;i<MAX_OPEN;i++)
  {
    if(tableMetaInfo[i].free)return i;
  }
  return E_CACHEFULL;
  /* traverse through the tableMetaInfo array,
    find a free entry in the Open Relation Table.*/

  // if found return the relation id, else return E_CACHEFULL.
}
int OpenRelTable::getRelId(char relName[ATTR_SIZE]) {

  /* traverse through the tableMetaInfo array,
    find the entry in the Open Relation Table corresponding to relName.*/
  for(int i = 0;i<MAX_OPEN;i++)
  {
    if(!tableMetaInfo[i].free &&(!strcmp(tableMetaInfo[i].relName,relName)))return i;
  }
  return E_RELNOTOPEN;

  // if found return the relation id, else indicate that the relation do not
  // have an entry in the Open Relation Table.
}
int OpenRelTable::openRel(char relName[ATTR_SIZE]) {
  int id = getRelId(relName);
  if(/* the relation `relName` already has an entry in the Open Relation Table */id>=0){
    // (checked using OpenRelTable::getRelId())
    return id;
    // return that relation id;
  }

  /* find a free slot in the Open Relation Table
     using OpenRelTable::getFreeOpenRelTableEntry(). */
     id = getFreeOpenRelTableEntry();

  if (/* free slot not available */id==E_CACHEFULL){
    return E_CACHEFULL;
  }

  // let relId be used to store the free slot.
  int relId = id;
  /****** Setting up Relation Cache entry for the relation ******/

  /* search for the entry with relation name, relName, in the Relation Catalog using
      BlockAccess::linearSearch().
      Care should be taken to reset the searchIndex of the relation RELCAT_RELID
      before calling linearSearch().*/

  // relcatRecId stores the rec-id of the relation `relName` in the Relation Catalog.
  RelCacheTable::resetSearchIndex(RELCAT_RELID);
  Attribute val;
  strcpy(val.sVal,relName);
  char Name[ATTR_SIZE];
  strcpy(Name,ATTRCAT_ATTR_RELNAME);
  RecId relcatRecId = BlockAccess::linearSearch(RELCAT_RELID,Name,val,EQ);

  if (/* relcatRecId == {-1, -1} */relcatRecId.block == -1 && relcatRecId.slot == -1) {
    // (the relation is not found in the Relation Catalog.)
    return E_RELNOTEXIST;
  }

  /* read the record entry corresponding to relcatRecId and create a relCacheEntry
      on it using RecBuffer::getRecord() and RelCacheTable::recordToRelCatEntry().
      update the recId field of this Relation Cache entry to relcatRecId.
      use the Relation Cache entry to set the relId-th entry of the RelCacheTable.
    NOTE: make sure to allocate memory for the RelCacheEntry using malloc()
  */
  RecBuffer block(relcatRecId.block);
  Attribute record[RELCAT_NO_ATTRS];
  block.getRecord(record,relcatRecId.slot);
  RelCacheEntry relCacheEntry;
  RelCacheTable::recordToRelCatEntry(record,&relCacheEntry.relCatEntry);
  relCacheEntry.recId = relcatRecId;
  RelCacheTable::relCache[relId] = (RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[relId]) = relCacheEntry;
  /****** Setting up Attribute Cache entry for the relation ******/

  // let listHead be used to hold the head of the linked list of attrCache entries.
  AttrCacheEntry* listHead, *temp;
  listHead = temp = (AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
  int numAttrs = relCacheEntry.relCatEntry.numAttrs;
  for(int i = 0;i<numAttrs-1;i++)
  {
    temp->next = (AttrCacheEntry*) malloc (sizeof(AttrCacheEntry));
    temp = temp->next;
  }
  temp->next = nullptr;
  RelCacheTable::resetSearchIndex(ATTRCAT_RELID);

  /*iterate over all the entries in the Attribute Catalog corresponding to each
  attribute of the relation relName by multiple calls of BlockAccess::linearSearch()
  care should be taken to reset the searchIndex of the relation, ATTRCAT_RELID,
  corresponding to Attribute Catalog before the first call to linearSearch().*/
  temp = listHead;
  for(int i = 0;i<numAttrs;i++)
  {
      /* let attrcatRecId store a valid record id an entry of the relation, relName,
      in the Attribute Catalog.*/
      RecId attrcatRecId = BlockAccess::linearSearch(ATTRCAT_RELID,Name,val,EQ);

      /* read the record entry corresponding to attrcatRecId and create an
      Attribute Cache entry on it using RecBuffer::getRecord() and
      AttrCacheTable::recordToAttrCatEntry().
      update the recId field of this Attribute Cache entry to attrcatRecId.
      add the Attribute Cache entry to the linked list of listHead .*/
      RecBuffer rec(attrcatRecId.block);
      Attribute reco[ATTRCAT_NO_ATTRS];
      rec.getRecord(reco,attrcatRecId.slot);
      AttrCacheTable::recordToAttrCatEntry(reco,&temp->attrCatEntry);
      temp->recId = attrcatRecId;

      // NOTE: make sure to allocate memory for the AttrCacheEntry using malloc()
      temp = temp->next;
  }

  // set the relIdth entry of the AttrCacheTable to listHead.
  AttrCacheTable::attrCache[relId] = listHead;

  /****** Setting up metadata in the Open Relation Table for the relation******/

  // update the relIdth entry of the tableMetaInfo with free as false and
  tableMetaInfo[relId].free = false;
  strcpy(tableMetaInfo[relId].relName,relName);
  // relName as the input.

  return relId;
}

int OpenRelTable::closeRel(int relId) {
  if (/* rel-id corresponds to relation catalog or attribute catalog*/relId == RELCAT_RELID || relId == ATTRCAT_RELID) {
    return E_NOTPERMITTED;
  }

  if (/* 0 <= relId < MAX_OPEN */relId<0 || relId>=MAX_OPEN) {
    return E_OUTOFBOUND;
  }

  if (/* rel-id corresponds to a free slot*/tableMetaInfo[relId].free) {
    return E_RELNOTOPEN;
  }

  // free the memory allocated in the relation and attribute caches which was
  // allocated in the OpenRelTable::openRel() function
  free(RelCacheTable::relCache[relId]);
  free(AttrCacheTable::attrCache[relId]);
  // update `tableMetaInfo` to set `relId` as a free slot
  tableMetaInfo[relId].free = true;
  RelCacheTable::relCache[relId] = nullptr;
  AttrCacheTable::attrCache[relId] = nullptr;

  // update `relCache` and `attrCache` to set the entry at `relId` to nullptr

  return SUCCESS;
}


OpenRelTable::~OpenRelTable() {

  // close all open relations (from rel-id = 2 onwards. Why?)
  for (int i = 2; i < MAX_OPEN; ++i) {
    if (!tableMetaInfo[i].free) {
      OpenRelTable::closeRel(i); // we will implement this function later
    }
  }

  // free the memory allocated for rel-id 0 and 1 in the caches
  free(RelCacheTable::relCache[0]);
  free(RelCacheTable::relCache[1]);
  free(AttrCacheTable::attrCache[0]);
  free(AttrCacheTable::attrCache[1]);
}
