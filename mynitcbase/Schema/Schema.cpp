#include "Schema.h"

#include <cmath>
#include <cstring>

int Schema::openRel(char relName[ATTR_SIZE]) {
  int ret = OpenRelTable::openRel(relName);

  // the OpenRelTable::openRel() function returns the rel-id if successful
  // a valid rel-id will be within the range 0 <= relId < MAX_OPEN and any
  // error codes will be negative

  if(ret >= 0 && ret<MAX_OPEN){
    return SUCCESS;
  }

  //otherwise it returns an error message
  return ret;
}

int Schema::renameRel(char oldRelName[ATTR_SIZE], char newRelName[ATTR_SIZE]) {
    // if the oldRelName or newRelName is either Relation Catalog or Attribute Catalog,
    char oldrelName[ATTR_SIZE];
    char newrelName[ATTR_SIZE];
    strcpy(oldrelName,RELCAT_RELNAME);
    strcpy(newrelName,ATTRCAT_RELNAME);
    if((!strcmp(oldrelName,oldRelName)) || (!strcmp(oldrelName,newRelName)) || (!strcmp(newrelName,oldRelName)) || (!strcmp(newrelName,newRelName)))
    {
      return E_NOTPERMITTED;
    }
        // return E_NOTPERMITTED
        // (check if the relation names are either "RELATIONCAT" and "ATTRIBUTECAT".
        // you may use the following constants: RELCAT_NAME and ATTRCAT_NAME)
    int i = OpenRelTable::getRelId(oldRelName);
    if(i != E_RELNOTOPEN)
    {
      return E_RELOPEN;
    }
    // if the relation is open
    //    (check if OpenRelTable::getRelId() returns E_RELNOTOPEN)
    //    return E_RELOPEN
    int retVal = BlockAccess::renameRelation(oldRelName,newRelName);
    return retVal;
    // retVal = BlockAccess::renameRelation(oldRelName, newRelName);
    // return retVal
}

int Schema::renameAttr(char *relName, char *oldAttrName, char *newAttrName) {
    // if the relName is either Relation Catalog or Attribute Catalog,
    if((!strcmp(RELCAT_RELNAME,relName)) || (!strcmp(ATTRCAT_RELNAME,relName)))
    {
      return E_NOTPERMITTED;
    }
        // return E_NOTPERMITTED
        // (check if the relation names are either "RELATIONCAT" and "ATTRIBUTECAT".
        // you may use the following constants: RELCAT_NAME and ATTRCAT_NAME)
    int i = OpenRelTable::getRelId(relName);
    if(i != E_RELNOTOPEN)
    {
      return E_RELOPEN;
    }
    // if the relation is open
        //    (check if OpenRelTable::getRelId() returns E_RELNOTOPEN)
        //    return E_RELOPEN
    int retVal = BlockAccess::renameAttribute(relName,oldAttrName,newAttrName);
    // Call BlockAccess::renameAttribute with appropriate arguments.

    // return the value returned by the above renameAttribute() call
    return retVal;
}

int Schema::closeRel(char relName[ATTR_SIZE]) {
  if (/* relation is relation catalog or attribute catalog */(!strcmp(relName,RELCAT_RELNAME) ) || !strcmp(relName,ATTRCAT_RELNAME) ){
    return E_NOTPERMITTED;
  }

  // this function returns the rel-id of a relation if it is open or
  // E_RELNOTOPEN if it is not. we will implement this later.
  int relId = OpenRelTable::getRelId(relName);

  if (/* relation is not open */relId == E_RELNOTOPEN ) {
    return E_RELNOTOPEN;
  }

  return OpenRelTable::closeRel(relId);
}
int Schema::createRel(char relName[],int nAttrs, char attrs[][ATTR_SIZE],int attrtype[]){

    // declare variable relNameAsAttribute of type Attribute
    // copy the relName into relNameAsAttribute.sVal
    Attribute relNameAsAttribute;
    strcpy(relNameAsAttribute.sVal,relName);

    // declare a variable targetRelId of type RecId
    RecId targetRelId;
    RelCacheTable::resetSearchIndex(RELCAT_RELID);
    char relcatattrname[ATTR_SIZE];
    strcpy(relcatattrname,RELCAT_ATTR_RELNAME);
    targetRelId = BlockAccess::linearSearch(RELCAT_RELID,relcatattrname,relNameAsAttribute,EQ);

    // Reset the searchIndex using RelCacheTable::resetSearhIndex()
    // Search the relation catalog (relId given by the constant RELCAT_RELID)
    // for attribute value attribute "RelName" = relNameAsAttribute using
    // BlockAccess::linearSearch() with OP = EQ

    // if a relation with name `relName` already exists  ( linearSearch() does
    //                                                     not return {-1,-1} )
    //     return E_RELEXIST;
    if(targetRelId.block != -1 && targetRelId.slot != -1)return E_RELEXIST;
    for(int i = 0;i<nAttrs;i++)
    {
      for(int j = i+1;j<nAttrs;j++)
      {
        if(!strcmp(attrs[i],attrs[j]))return E_DUPLICATEATTR;
      }
    }
    // compare every pair of attributes of attrNames[] array
    // if any attribute names have same string value,
    //     return E_DUPLICATEATTR (i.e 2 attributes have same value)

    /* declare relCatRecord of type Attribute which will be used to store the
       record corresponding to the new relation which will be inserted
       into relation catalog */
    Attribute relCatRecord[RELCAT_NO_ATTRS];
    // fill relCatRecord fields as given below
    // offset RELCAT_REL_NAME_INDEX: relName
    // offset RELCAT_NO_ATTRIBUTES_INDEX: numOfAttributes
    // offset RELCAT_NO_RECORDS_INDEX: 0
    // offset RELCAT_FIRST_BLOCK_INDEX: -1
    // offset RELCAT_LAST_BLOCK_INDEX: -1
    // offset RELCAT_NO_SLOTS_PER_BLOCK_INDEX: floor((2016 / (16 * nAttrs + 1)))
    // (number of slots is calculated as specified in the physical layer docs)
    strcpy(relCatRecord[RELCAT_REL_NAME_INDEX].sVal,relName);
    relCatRecord[RELCAT_NO_ATTRIBUTES_INDEX].nVal = nAttrs;
    relCatRecord[RELCAT_NO_RECORDS_INDEX].nVal = 0;
    relCatRecord[RELCAT_FIRST_BLOCK_INDEX].nVal = -1;
    relCatRecord[RELCAT_LAST_BLOCK_INDEX].nVal = -1;
    relCatRecord[RELCAT_NO_SLOTS_PER_BLOCK_INDEX].nVal = (2016/(16*nAttrs + 1));
    int retVal = BlockAccess::insert(RELCAT_RELID,relCatRecord);
    if(retVal != SUCCESS)return retVal;

    // retVal = BlockAccess::insert(RELCAT_RELID(=0), relCatRecord);
    // if BlockAccess::insert fails return retVal
    // (this call could fail if there is no more space in the relation catalog)

    for(int i = 0;i<nAttrs;i++)// iterate through 0 to numOfAttributes - 1 :
    {
        /* declare Attribute attrCatRecord[6] to store the attribute catalog
           record corresponding to i'th attribute of the argument passed*/
        Attribute attrCatRecord[6];
        // (where i is the iterator of the loop)
        // fill attrCatRecord fields as given below
        strcpy(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal,relName);
        strcpy(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,attrs[i]);
        attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal = attrtype[i];
        attrCatRecord[ATTRCAT_PRIMARY_FLAG_INDEX].nVal = -1;
        attrCatRecord[ATTRCAT_ROOT_BLOCK_INDEX].nVal = -1;
        attrCatRecord[ATTRCAT_OFFSET_INDEX].nVal = i;
        // offset ATTRCAT_REL_NAME_INDEX: relName
        // offset ATTRCAT_ATTR_NAME_INDEX: attrNames[i]
        // offset ATTRCAT_ATTR_TYPE_INDEX: attrTypes[i]
        // offset ATTRCAT_PRIMARY_FLAG_INDEX: -1
        // offset ATTRCAT_ROOT_BLOCK_INDEX: -1
        // offset ATTRCAT_OFFSET_INDEX: i
        int reVal = BlockAccess::insert(ATTRCAT_RELID,attrCatRecord);
        // retVal = BlockAccess::insert(ATTRCAT_RELID(=1), attrCatRecord);
        if(reVal != SUCCESS)
        {
          deleteRel(relName);
          return E_DISKFULL;
        }
        /* if attribute catalog insert fails:
             delete the relation by calling deleteRel(targetrel) of schema layer
             return E_DISKFULL
             // (this is necessary because we had already created the
             //  relation catalog entry which needs to be removed)
        */
    }

    return SUCCESS;
}
int Schema::deleteRel(char *relName) {
  char relcatname[ATTR_SIZE];
  char attrcatname[ATTR_SIZE];
  strcpy(relcatname,RELCAT_RELNAME);
  strcpy(attrcatname,ATTRCAT_RELNAME);
  if((!strcmp(relName,relcatname)) || (!strcmp(relName,attrcatname)))
  {
    return E_NOTPERMITTED;
  }
    // if the relation to delete is either Relation Catalog or Attribute Catalog,
    //     return E_NOTPERMITTED
        // (check if the relation names are either "RELATIONCAT" and "ATTRIBUTECAT".
        // you may use the following constants: RELCAT_NAME and ATTRCAT_NAME)

    // get the rel-id using appropriate method of OpenRelTable class by
    // passing relation name as argument
    int ret = OpenRelTable::getRelId(relName);
    if(ret != E_RELNOTOPEN)return E_RELOPEN;
    // if relation is opened in open relation table, return E_RELOPEN
    return BlockAccess::deleteRelation(relName);
    // Call BlockAccess::deleteRelation() with appropriate argument.

    // return the value returned by the above deleteRelation() call

    /* the only that should be returned from deleteRelation() is E_RELNOTEXIST.
       The deleteRelation call may return E_OUTOFBOUND from the call to
       loadBlockAndGetBufferPtr, but if your implementation so far has been
       correct, it should not reach that point. That error could only occur
       if the BlockBuffer was initialized with an invalid block number.
    */
}