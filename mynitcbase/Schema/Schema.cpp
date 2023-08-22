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
