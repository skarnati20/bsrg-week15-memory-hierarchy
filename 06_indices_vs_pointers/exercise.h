#ifndef EXERCISE_06_H
#define EXERCISE_06_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    OP_ADD = 0,
    OP_SUB = 1,
    OP_MUL = 2,
    OP_CONST = 3
} OpType;

typedef struct PointerNode {
    OpType op;
    int32_t val;
    struct PointerNode *left;
    struct PointerNode *right;
} PointerNode;

// Compact Data-Oriented AST node (8 bytes, no 64-bit pointers!)
typedef struct {
    uint8_t op;
    int32_t val;
} CompactNode;

void flatten_tree(const PointerNode *root, CompactNode *out_arena, size_t *out_count);
int64_t eval_compact_ast(const CompactNode *nodes, size_t count);

#endif // EXERCISE_06_H
