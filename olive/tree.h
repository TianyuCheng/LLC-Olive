
/*
   FILE: x_array.gen
  
   Copyright (c) 1997 Princeton University

   All rights reserved.

   This software is to be used for non-commercial purposes only,
   unless authorized permission to do otherwise is obtained.  
   For more information, contact spam@ee.princeton.edu
*/



/*
  generalized for arbitray sized element.
  Implemented as a macro.
  Beware: User is responsible for instantiating implementation
  */



/*
   FILE: hash.gen
  
   Copyright (c) 1997 Princeton University

   All rights reserved.

   This software is to be used for non-commercial purposes only,
   unless authorized permission to do otherwise is obtained.  
   For more information, contact spam@ee.princeton.edu
*/












/*
   FILE: tree.int
  
   Copyright (c) 1997 Princeton University

   All rights reserved.

   This software is to be used for non-commercial purposes only,
   unless authorized permission to do otherwise is obtained.  
   For more information, contact spam@ee.princeton.edu
*/

#define MAX_KIDS 20
typedef struct tree *Tree;
struct tree {		/* tree patterns: */
	void *op;		/* a term or non-terminal */
	int nkids;
	Tree kids[MAX_KIDS];
};



/*

Arguments are (Type_name = Tree_list,Pointer_Type = Tree)

*/

typedef X_arrayp Tree_list;

#define Tree_list_create(sz) ((Tree_list *)X_arrayp_create(sz))
#define Tree_list_destroy(tl) X_arrayp_destroy(tl)
#define Tree_list_fetch(tl,i) ((Tree)X_arrayp_fetch(tl,i))
#define Tree_list_set(tl,i,t) X_arrayp_set(tl,i,t)
#define Tree_list_grow(tl,i) X_arrayp_grow(tl,i)
#define Tree_list_expand(tl,i) X_arrayp_expand(tl,i)
#define Tree_list_extend(tl,t) X_arrayp_extend(tl,t)
#define Tree_list_ub(tl) X_arrayp_ub(tl)
#define Tree_list_grab_from(d,s) X_arrayp_grab_from(d,s)
#define Tree_list_freeze(tl) X_arrayp_freeze(tl)


extern Tree tree(char *op, Tree_list *kids);
