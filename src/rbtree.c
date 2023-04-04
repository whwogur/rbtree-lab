#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *NIL = (node_t *)calloc(1, sizeof(node_t));
  NIL->color = RBTREE_BLACK;
  p->nil = NIL;
  p->root = NIL;
  return p;
}
void delete_node(rbtree* t, node_t* n) {
  if (n == t->nil) return;

  delete_node(t, n->left);
  delete_node(t, n->right);
  free(n);
}
void delete_rbtree(rbtree *t) {
  delete_node(t, t->root);
  free(t->nil);
  free(t);
}

void left_rotate(rbtree *t, node_t *x) {
  node_t *y = x->right;
  x->right = y->left;
  if (y->left != t->nil)
    y->left->parent = x;

  y->parent = x->parent;
  if (x->parent == t->nil)
    t->root = y;
  else if (x == y->parent->left)
    x->parent->left = y;
  else
    x->parent->right = y;
  y->left = x;
  x->parent = y;
}

void right_rotate(rbtree *t, node_t *x) {
  node_t *y = x->left;
  x->left = y->right;
  if (y->right != t->nil)
    y->right->parent = x;
  
  y->parent = x->parent;
  if (x->parent == t->nil)
    t->root = y;
  else if (x == x->parent->right)
    x->parent->right = y;
  else
    x->parent->left = y;
  y->right = x;
  x->parent = y;
}
/*
Insert - T.C : O(logN)
(max height of the tree logN + fixing violations O(logN) + constant)
1. Insert Z and color it RED
2. Recolor and rotate nodes to fix violation
  -> by coloring the node red, we might violate property #2: The root and leaves are black, #3 If a node is RED, then its children are BLACK
cases:
case1) Z is the root -> color Z BLACK
case2) Z's uncle is RED -> recolor Z's p, p.p, uncle
case3) Z's uncle is BLACK
        ㄴ \ / shaped formation (p is a left/ right child of p.p and so is Z)
            -> rotate Z's grandparent in the opposite direction of Z - Z's p takes p.p's place && swaps color
        ㄴ > < shaped formation (p is a left/ right child of p.p and Z is the 'opposite-side' child of its p)
            -> rotate Z's parent in the opposite direction of Z - Z takes its p's place
*/

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t *z = (node_t *)calloc(1, sizeof(node_t));
  node_t *tmp_parent = t->nil;
  node_t *tmp_child = t->root;
  while (tmp_child != t->nil)
  {
    tmp_parent = tmp_child;
    if (key < tmp_child->key)
      tmp_child = tmp_child->left;
    else
      tmp_child = tmp_child->right;
  }
  z->parent = tmp_parent;
  if (tmp_parent == t->nil)
    t->root = z;
  else if (key < tmp_parent->key)
    tmp_parent->left = z;
  else
    tmp_parent->right = z;
  
  z->key = key;
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;
  rbtree_insert_fixup(t, z);
  return t->root;
}

void rb_tree_insert_fixup (rbtree *t, node_t *z) {
  while (z->parent->color == RBTREE_RED)
  {
    if (z->parent == z->parent->parent->left)
    {
      node_t *uncle = z->parent->parent->right;
      if (uncle->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else
      {
        if (z == z->parent->right)
        {
          z = z->parent;
          left_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
    }
    else
    {
      node_t *uncle = z->parent->parent->left;
      if (uncle->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else
      {
        if (z == z->parent->left)
        {
          z = z->parent;
          right_rotate(t, z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t* temp = t->root;
  while (temp != t->nil)
  {
    if (temp->key > key)
      temp = temp->left;
    else if (temp->key < key)
      temp = temp->right;
    else
      return temp;
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  return t->root;
}
/*
Delete
cases:
case1) Node to be deleted is RED -> just delete it
case2) DB(DoubleBlack)node is the root node -> remove DB
case3) DB's sibling is BLACK and both its children are BLACK
        -> 1. give DB to its parent & remove from itself
           2. color the sibling RED
case4) DB's sibling is RED
        -> 1. parent <-> sibling swap colors
           2. rotate parent in the direction of DB node
*/
void rb_delete_fixup(rbtree* t, node_t* z) {
  node_t* w;
  while (z!= t->nil && z != t->root && z->color == RBTREE_BLACK)
  {
    if (z == z->parent->left)
    {
      w = z->parent->right;
      if (w->color == RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        z->parent->color = RBTREE_RED;
        left_rotate(t, z->parent);
        w = z->parent->right;
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        z = z->parent;
      }
      else
      {
        if (w->right->color == RBTREE_BLACK)
        {
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w);
          w = z->parent->right;
        }
        w->color = z->parent->color;
        z->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, z->parent);
        z = t->root;
      }
    }
    else
    {
      w = z->parent->right;
      if (w->color == RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        z->parent->color = RBTREE_RED;
        left_rotate(t, z->parent);
        w = z->parent->right;
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        z = z->parent;
      }
      else
      {
        if (w->right->color == RBTREE_BLACK)
        {
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w);
          w = z->parent->right;
        }
        w->color = z->parent->color;
        z->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, z->parent);
        z = t->root;
      }
    }
    z->color = RBTREE_BLACK;
  }
}

int rbtree_erase(rbtree *t, node_t *z) {
  node_t* y = z;
  node_t* x;
  color_t y_og_color = y->color;

  if (z->left == t->nil)
  {
    x = z->right;
    rb_transplant(t, z, z->right);
  }
  else if (z->right == t->nil)
  {
    x = z->left;
    rb_transplant(t, z, z->left);
  }
  else
  {
    y = tree_minimum(t, z->right);
    y_og_color = y->color;
    x = y->right;
    if (y->parent == z)
      x->parent = y;
    else
    {
      rb_transplant(t, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }
    rb_transplant(t, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }
  if (y_og_color == RBTREE_BLACK);
    rb_delete_fixup(t, x);
  free(z);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  node_to_array(t, t->root, arr, 0);
  return 0;
}
