#include "rbtree.h"
#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));			  
  node_t *nil_node = (node_t *)calloc(1, sizeof(node_t));	
  nil_node->color = RBTREE_BLACK;							            
  t->nil = nil_node;										                  
  t->root = nil_node;										                  
  return t;													                     
}

// 각각의 노드들이 가리키는 공간들 해제
void delete_node(node_t *node, rbtree *t) {
  if (node == t->nil) {
    return;
  } // 재귀적으로 삭제
  delete_node(node->left, t);
  delete_node(node->right, t);
  free(node);
  node = NULL;
  return;
}

// 트리, 트리의 nil이 가리키는 메모리 해제 (rbtree_erase != delete_rbtree)
void delete_rbtree(rbtree *t) {
  // 트리가 없으면 return
  if (t == NULL) {
    return;
  }
  delete_node(t->root, t);
  free(t->nil);             
  t->nil = NULL;            
  free(t);                  
  t = NULL;                 
  return;
}

// 좌회전
void left_rotation(rbtree* t, node_t* x) {
  node_t* y = x->right;             
  x->right = y->left;               
  if (y->left != t->nil) {          
    y->left->parent = x;              
  }
  y->parent = x->parent;
  if (x->parent == t->nil) {        
    t->root = y;                    
  } else if (x == x->parent->left) {
    x->parent->left = y; 
  } else {               
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
  return;
}
// 우회전
void right_rotation(rbtree* t, node_t* x) {
  node_t* y = x->left;
  x->left = y->right;
  if (y->right != t->nil) {
    y->right->parent = x;
  }
  y->parent = x->parent;
  if (x->parent == t->nil) {
    t->root = y;
  } else if (x == x->parent->right) {
    x->parent->right = y;
  } else {
    x->parent->left = y;
  }
  y->right = x;
  x->parent = y;
  return;
}

void rbtree_insert_fixup(rbtree *t, node_t *z) {
  while (z->parent->color == RBTREE_RED) {      // z의 부모가 RED일때
    if (z->parent == z->parent->parent->left) { 
      node_t *y = z->parent->parent->right;     
      if (y->color == RBTREE_RED) {             // case 1. z의 삼촌이 적색일때
        z->parent->color = RBTREE_BLACK;          
        y->color = RBTREE_BLACK;                  
        z->parent->parent->color = RBTREE_RED;    
        z = z->parent->parent;                    
      } else {                                  // case 2. z의 삼촌이 흑색일때
          if (z == z->parent->right) {            
            z = z->parent;                        
            left_rotation(t, z);                  
        }                                       // case 3. z의 삼촌이 흑색이고 z가 부모의 왼쪽 자식일 때
        z->parent->color = RBTREE_BLACK;          
        z->parent->parent->color = RBTREE_RED;    
        right_rotation(t, z->parent->parent);     
      }
    } else {                                    // z의 부모가 조부모의 오른쪽자식이라면 반대로
      node_t *y = z->parent->parent->left;
      if (y->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } else {
          if (z == z->parent->left) {
            z = z->parent;
            right_rotation(t, z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotation(t, z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;                // 특성 2 유지
  return;
}

// 삽입
node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t* y = t->nil;     
  node_t* x = t->root;    
  while (x != t->nil) {   // 서브트리 탐색
    y = x;                
    if (key < x->key) {   
      x = x->left;        
    } else {              
      x = x->right;       
    }
  }
  node_t* z = (node_t *)calloc(1, sizeof(node_t));
  z->parent = y;              
  if (y == t->nil) {          
    t->root = z;              
  } else if (key < y->key) {  
    y->left = z;              
  } else {                    
    y->right = z;             
  }
  z->key = key;               
  z->color = RBTREE_RED;      
  z->left = t->nil;           
  z->right = t->nil;          
  rbtree_insert_fixup(t, z);  
  return t->root;             
}

// 트리에서 특정 값 찾기
node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *x = t->root;                    
  while (x != t->nil && key != x->key) {  
    if (x->key < key) {                   
      x = x->right;                       
    } else {                              
      x = x->left;                        
    }
  }
  if (x == t->nil) {
    return NULL;
  }
  return x;
}

// 트리의 최솟값
node_t *rbtree_min(const rbtree *t) {
  node_t *x = t->root;
  while (x->left != t->nil) {
    x = x->left;             
  }
  return x;                  
}

// 트리의 최댓값
node_t *rbtree_max(const rbtree *t) {
  node_t *x = t->root;
  while (x->right != t->nil) {
    x = x->right;             
  }
  return x;                   
}

// u의 부모와 v와 연결
void rbtree_transplant(rbtree *t, node_t *u, node_t * v) {
  if (u->parent == t->nil) {
    t->root = v;         
  } else if (u == u->parent->left) {
    u->parent->left = v;            
  } else {                          
    u->parent->right = v;           
  }
  v->parent = u->parent;            
  return;
}

void rb_delete_fixup(rbtree *t, node_t *x){
    node_t *w;
    while ((x != t->root) && (x->color == RBTREE_BLACK)) {
        if (x == x->parent->left) {
            w = x->parent->right;
            if (w->color == RBTREE_RED) {
                w->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                left_rotation(t, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
                w->color = RBTREE_RED;
                x = x->parent;
            }  else {
                if (w->right->color == RBTREE_BLACK) {
                    w->left->color = RBTREE_BLACK;
                    w->color = RBTREE_RED;
                    right_rotation(t, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = RBTREE_BLACK;
                w->right->color = RBTREE_BLACK;
                left_rotation(t, x->parent);
                x = t->root;
            }
        } else {
            w = x->parent->left;
            if (w->color == RBTREE_RED) {
                w->color = RBTREE_BLACK;
                x->parent->color = RBTREE_RED;
                right_rotation(t, x->parent);
                w = x ->parent->left;
            }
            if (w->right ->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
                w->color = RBTREE_RED;
                x = x->parent;
            } else {
                if (w->left->color == RBTREE_BLACK) {
                    w->right->color = RBTREE_BLACK;
                    w->color = RBTREE_RED;
                    left_rotation(t, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = RBTREE_BLACK;
                w->left -> color = RBTREE_BLACK;
                right_rotation(t, x->parent);
                x = t->root;
            }
        }
    }
    x->color = RBTREE_BLACK;
    return;
}

// 계승 원소 찾기, x는 삭제할 노드의 오른쪽 자식
node_t *rbtree_successor(rbtree *t, node_t *x) {
  node_t *y = x;              
  while (y->left != t->nil) { 
    y = y->left;              
  }
  return y;                   
}

int rbtree_erase(rbtree *t, node_t *p) {
  node_t *x;                              
  node_t *y = p;                          
  color_t y_color = y->color;             
  if (p->left == t->nil) {                
    x = p->right;                         
    rbtree_transplant(t, p, p->right);    
  } else if (p->right == t->nil) {        
    x = p->left;                          
    rbtree_transplant(t, p, p->left);     
  } else {                                
    y = rbtree_successor(t, p->right);    
    y_color = y->color;                   
    x = y->right;                         
    if (y->parent == p) {                 
      x->parent = y;                      
    } else {                              
      rbtree_transplant(t, y, y->right);  
      y->right = p->right;                
      y->right->parent = y;               
    }
    rbtree_transplant(t, p, y);           
    y->left = p->left;                    
    y->left->parent = y;                  
    y->color = p->color;                  
  }
  free(p);                                
  p = NULL;                               
  if (y_color == RBTREE_BLACK) {          
    rb_delete_fixup(t, x);                
  }
  return 0;
}

// 트리의 중위 순회
int rbtree_inorder(node_t *nil, node_t *root, key_t *arr, const size_t n, int index) {
  if (root == nil) {                                        
    return index;                                           
  }
  if (index == n) {                                         
    return index;                                           
  } 
  index = rbtree_inorder(nil, root->left, arr, n, index);   
  if(index < n) {                                           
    arr[index++] = root->key;                               
  }
  index = rbtree_inorder(nil, root->right, arr, n, index);
  return index;                                             
}

// 트리를 이용하여 오름차순 구현
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  if (t->root == t->nil) {
    return 0;
  }
  rbtree_inorder(t->nil, t->root, arr, n, 0);
  return 0;
}