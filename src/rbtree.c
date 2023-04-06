#include "rbtree.h"
#include <stdlib.h>

// 트리 초기화
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
  // 현재 노드가 트리의 nil노드면 return
  if (node == t->nil) {
    return;
  }

  delete_node(node->left, t);   // 왼쪽으로 재귀
  delete_node(node->right, t);  // 오른쪽으로 재귀
  free(node);                   // 현재 노드가 가리키는 메모리 할당 해제
  node = NULL;                  // 할당 해제 후 현재 노드값을 NULL로 초기화
  return;
}

// 트리, 트리의 nil이 가리키는 공간 해제
void delete_rbtree(rbtree *t) {
  // 트리가 없으면 return
  if (t == NULL) {
    return;
  }

  delete_node(t->root, t);  // 생성된 노드들이 가리키는 메모리 해제
  free(t->nil);             // 트리의 nil노드가 가리키는 메모리 해제
  t->nil = NULL;            // 메모리 해제 후 트리의 nil노드값을 NULL로 초기화
  free(t);                  // 트리가 가리키는 메모리 할당 해제
  t = NULL;                 // 메모리 해제 후 트리의 값을 NULL로 초기화
  return;
}

// 좌회전
void left_rotation(rbtree* t, node_t* x) {
  node_t* y = x->right;               // y = 현재 노드의 오른쪽
  x->right = y->left;                 // x의 오른쪽 자식을 y의 왼쪽 자식으로 변경
  if (y->left != t->nil) {            // y의 왼쪽이 nil이 아니면
    y->left->parent = x;              // y의 왼쪽 부모을 x로 변경             
  }
  y->parent = x->parent;              // y의 부모를 x의 부모로 변경
  if (x->parent == t->nil) {          // x의 부모가 nil이라면 루트라는 뜻
    t->root = y;                      // 트리의 루트를 y로 변경
  } else if (x == x->parent->left) {  // x가 x의 부모의 왼쪽 자식이라면
    x->parent->left = y;              // x의 부모의 왼쪽자식을 y로 변경
  } else {                            // x가 x의 부모의 오른쪽 자식이라면
    x->parent->right = y;             // x의 부모의 오른쪽자식을 y로 변경
  }
  y->left = x;                        // y의 왼쪽자식을 x로 변경
  x->parent = y;                      // x의부모 = y
  return;
}


// 우회전(좌회전과 대칭, 반대로)
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

// 색 변경
void rbtree_insert_fixup(rbtree *t, node_t *z) {
  while (z->parent->color == RBTREE_RED) {      // z의 부모가 RED일 경우
    if (z->parent == z->parent->parent->left) { // z의 부모가 z의 조부모의 왼쪽 자식이면
      node_t *y = z->parent->parent->right;     // y는 z의 조부모의 오른쪽 자식(삼촌)
      if (y->color == RBTREE_RED) {             // case 1) z의 삼촌이 빨강일 경우
        z->parent->color = RBTREE_BLACK;          // z의 부모 BLACK으로 변경
        y->color = RBTREE_BLACK;                  // z의 삼촌 BLACK으로 변경 
        z->parent->parent->color = RBTREE_RED;    // z의 조부모 RED로 변경 
        z = z->parent->parent;                    // z = 조부모
      } else {                                  // case 2) z의 삼촌이 검정일 경우
          if (z == z->parent->right) {            // z가 오른쪽 자식일 경우
            z = z->parent;                        // z를 z의 부모로 변경
            left_rotation(t, z);                  // z를 기준으로 좌회전
        }                                       // case 3) z의 삼촌이 검정이고 z가 왼쪽 자식일 때
        z->parent->color = RBTREE_BLACK;          // z의 부모 검정으로 변경
        z->parent->parent->color = RBTREE_RED;    // z의 조부모 빨강으로 변경
        right_rotation(t, z->parent->parent);     // z의 조부모 기준으로 우회전
      }
    } else {                                    // z의 부모가 z의 조부모의 오른쪽이라면 왼쪽case의 반대로
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
  t->root->color = RBTREE_BLACK;                // 특성 2을 유지하기 위해 root BLACK으로 설정
  return;
}

// 삽입
node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t* y = t->nil;     // y는 트리의 nil노드
  node_t* x = t->root;    // x는 트리의 root노드
  while (x != t->nil) {   // 서브트리 탐색
    y = x;
    if (key < x->key) {
      x = x->left;
    } else {
      x = x->right;
    }
  }
  node_t* z = (node_t *)calloc(1, sizeof(node_t));  // z(노드) 생성
  z->parent = y;              // z의 부모 = y
  if (y == t->nil) {          // y가 트리의 nil일 때(첫 노드 삽입)
    t->root = z;              // 트리의 root = z
  } else if (key < y->key) {  // y의 key값이 삽입할 키 값보다 작을 때
    y->left = z;              // 왼쪽 서브트리 탐색
  } else {                    // y의 key값이 삽입할 키 값보다 크거나 같을 때
    y->right = z;             // 오른쪽 서브트리 탐색
  }
  z->key = key;               // z의 key값은 현재 key
  z->color = RBTREE_RED;      // z의 color값은 RED, 삽입할 때는 무조건 RED
  z->left = t->nil;           // 좌 / 우 자식 NIL 연결
  z->right = t->nil;           
  rbtree_insert_fixup(t, z);  // fixup 호출
  return t->root;             // 트리의 root값 반환
}

// 트리에서 원하는 값 찾기
node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *x = t->root;                    // x는 트리의 루트
  while (x != t->nil && key != x->key) {  // 서브트리 탐색
    if (x->key < key) {
      x = x->right;
    } else {
      x = x->left;
    }
  }
  if (x == t->nil) {                      // x가 nil노드면, 즉 key를 찾지 못했을 때
    return NULL;                          // NULL 반환
  }
  return x;                               // x가 nil노드가 아니면 x 반환, 즉 key를 찾았을 때
}
// 트리의 최솟값
node_t *rbtree_min(const rbtree *t) {
  node_t *x = t->root;        // 트리에서 가장 작은 값은 트리의 가장 왼쪽에 위치한다
  while (x->left != t->nil) { 
    x = x->left;              
  }
  return x;                   // x 반환
}

// 트리의 최댓값
node_t *rbtree_max(const rbtree *t) {
  node_t *x = t->root;          // 트리에서 가장 작은 값은 트리의 가장 오른쪽에 위치한다
  while (x->right != t->nil) {
    x = x->right;
  }
  return x;                     // x 반환
}

// u의 부모와 v와 연결
void rbtree_transplant(rbtree *t, node_t *u, node_t * v) {
  if (u->parent == t->nil) {          // u의 부모가 nil일 때, 즉, 삭제할 노드가 트리의 root면
    t->root = v;                      // 트리의 root는 v
  } else if (u == u->parent->left) {  // u가 u의 부모의 왼쪽 자식이면 
    u->parent->left = v;              // u의 부모의 왼쪽 자식 v
  } else {                            // u가 u의 부모의 오른쪽 자식이면
    u->parent->right = v;             // u의 부모의 오른쪽 자식은 v
  }
  v->parent = u->parent;              // v의 부모는 u의 부모
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

// successor찾기, 삭제할 노드의 오른쪽 자식을 루트로 하는 서브트리의 최솟값
node_t *rbtree_successor(rbtree *t, node_t *x) {
  node_t *y = x;
  while (y->left != t->nil) {
    y = y->left;
  }
  return y;
}

int rbtree_erase(rbtree *t, node_t *p) {
  node_t *x;                              // 노드 x
  node_t *y = p;                          // y = 삭제할 노드
  color_t y_color = y->color;             // y_color는 y의 색
  if (p->left == t->nil) {                // 삭제할 노드의 왼쪽이 nil일 경우
    x = p->right;                         // x는 삭제할 노드의 오른쪽
    rbtree_transplant(t, p, p->right);    // 삭제할 노드의 부모와 삭제할 노드의 오른쪽을 연결 
  } else if (p->right == t->nil) {        // 삭제할 노드의 오른쪽이 nil일 경우
    x = p->left;                          // x = 삭제할 노드의 왼쪽자식
    rbtree_transplant(t, p, p->left);     // 삭제할 노드의 부모와 삭제할 노드의 왼쪽을 연결
  } else {                                // 삭제할 노드의 왼쪽, 오른쪽 자식이 둘다 있을 때
    y = rbtree_successor(t, p->right);    // y = successor
    y_color = y->color;                   // y_color는 직후 원소의 색
    x = y->right;                         // x = y의 오른쪽 자식
    if (y->parent == p) {                 // y의 부모가 삭제할 노드일 때
      x->parent = y;                      // x의 부모 = y
    } else {                              // y의 부모가 삭제할 노드가 아닐 때
      rbtree_transplant(t, y, y->right);  // y의 부모와 y의 오른쪽 자식을 연결
      y->right = p->right;                // y의 오른쪽 자식 = 삭제할 노드의 오른쪽 자식
      y->right->parent = y;               // y의 오른쪽 자식의 부모 = y
    }
    rbtree_transplant(t, p, y);           // 삭제할 노드 부모와 y를 연결
    y->left = p->left;                    // y의 왼쪽 자식 = 삭제할 노드의 왼쪽 자식
    y->left->parent = y;                  // y의 왼쪽 자식의 부모 = y
    y->color = p->color;                  // y의 색 = 삭제할 노드의 색
  }
  free(p);                                // 삭제한 노드가 가리키는 공간 삭제
  p = NULL;                               // 할당 해제 후 삭제한 노드값을 NULL로 초기화
  if (y_color == RBTREE_BLACK) {          // y_color가 BLACK이면(삭제한 노드의 색이 BLACK이면(특성 5 위반))
    rb_delete_fixup(t, x);                // fixup 호출
  }
  return 0;
}

// 트리의 중위 순회
// root 노드는 계속 변화
int rbtree_inorder(node_t *nil, node_t *root, key_t *arr, const size_t n, int index) {
  if (root == nil) {                                        // root노드가 nil노드면
    return index;                                           // index 반환
  }
  if (index == n) {                                         // index가 n하고 같을 때 더 이상 재귀 돌지 않게 종료 조건
    return index;                                           // index반환
  } 
  index = rbtree_inorder(nil, root->left, arr, n, index);   // 왼쪽으로 재귀
  if(index < n) {                                           // idenx가 n보다 작을 때만 arr에 추가
    arr[index++] = root->key;                               // arr[현재 index] = 현재 노드의 키 값 
  }
  index = rbtree_inorder(nil, root->right, arr, n, index);  // 오른쪽으로 재귀
  return index;                                             // index 반환
}

// 트리를 이용하여 오름차순 구현
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // 트리의 루트가 nil노드일 때 반환
  if (t->root == t->nil) {
    return 0;
  }
  rbtree_inorder(t->nil, t->root, arr, n, 0); // 중위 순회
  return 0;
}