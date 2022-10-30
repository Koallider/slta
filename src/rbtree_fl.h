#pragma once

#include <stdio.h>
#include <stdlib.h>
#define COLOR_RED_FL 0
#define COLOR_BLACK_FL 1

int nnodes_fl = 0;
int ndeleted_nodes_fl = 0;

/*const char* gc_red = "RED";
const char* gc_black = "BLACK";*/

struct rbtree_fl *rbtree_add_fl(struct rbtree_fl *root, float key, char *value);
struct rbtree_fl *rbtree_fixup_add_fl(struct rbtree_fl *root, struct rbtree_fl *node);
struct rbtree_fl *rbtree_left_rotate_fl( struct rbtree_fl *root, struct rbtree_fl *node);
struct rbtree_fl *rbtree_right_rotate_fl(struct rbtree_fl *root, struct rbtree_fl *node);

struct rbtree_fl
{
    float key;
    char *value;
    int color;
    int deleted;
    struct rbtree_fl *parent;
    struct rbtree_fl *left;
    struct rbtree_fl *right;
};

struct rbtree_fl EmptyNode_fl = {-1, 0, COLOR_BLACK_FL,
    NULL, NULL, NULL};
struct rbtree_fl *NullNode_fl = &EmptyNode_fl;

struct rbtree_fl *rbtree_add_fl(struct rbtree_fl *root,
float key, char *value)
{
    struct rbtree_fl *node, *parent = NullNode_fl;
    /* Search leaf for new element */
    for (node = root; node != NullNode_fl && node != NULL; )
    {
        parent = node;
        if(key == node->key)
        {
            node->deleted = 0;
            node->value = value;
            ndeleted_nodes_fl--;
            return node;
        }
        if (key < node->key)
            node = node->left;
        else if (key > node->key)
            node = node->right;
        else
            return root;
    }

    nnodes_fl++;
    node = malloc(sizeof(*node));
    if (node == NULL)
        return NULL;
    node->key = key;
    node->value = value;
    node->deleted = 0;
    node->color = COLOR_RED_FL;
    node->parent = parent;
    node->left = NullNode_fl;
    node->right = NullNode_fl;
    if (parent != NullNode_fl) {
        if (key < parent->key)
            parent->left = node;
        else
            parent->right = node;
    } else {
        root = node;
    }
    return rbtree_fixup_add_fl(root, node);
}

struct rbtree_fl *rbtree_fixup_add_fl(struct rbtree_fl *root,
struct rbtree_fl *node)
{
    struct rbtree_fl *uncle;
    /* Current node is RED */
    while (node != root && node->parent->color == COLOR_RED_FL)
    {
        if (node->parent == node->parent->parent->left)
        {
            /* node in left tree of grandfather */
            uncle = node->parent->parent->right;
                if (uncle->color == COLOR_RED_FL) {
                /* Case 1 - uncle is RED */
                    node->parent->color = COLOR_BLACK_FL;
                    uncle->color = COLOR_BLACK_FL;
                    node->parent->parent->color = COLOR_RED_FL;
                    node = node->parent->parent;
                } else {
                /* Cases 2 & 3 - uncle is BLACK */
                    if (node == node->parent->right) {
                    /* Reduce case 2 to case 3 */
                        node = node->parent;
                        root = rbtree_left_rotate_fl(root, node);
                    }

                    /* Case 3 */
                    node->parent->color = COLOR_BLACK_FL;
                    node->parent->parent->color = COLOR_RED_FL;
                    root = rbtree_right_rotate_fl(root, node->parent->parent);
                }
        } else {
        /* Node in right tree of grandfather */
            uncle = node->parent->parent->left;
            if (uncle->color == COLOR_RED_FL) {
            /* Uncle is RED */
                node->parent->color = COLOR_BLACK_FL;
                uncle->color = COLOR_BLACK_FL;
                node->parent->parent->color = COLOR_RED_FL;
                node = node->parent->parent;
            } else {
            /* Uncle is BLACK */
                if (node == node->parent->left)
                {
                    node = node->parent;
                    root = rbtree_right_rotate_fl(root, node);
                }
                node->parent->color = COLOR_BLACK_FL;
                node->parent->parent->color = COLOR_RED_FL;
                root = rbtree_left_rotate_fl(root,
                node->parent->parent);
            }
        }
    }
    root->color = COLOR_BLACK_FL;
    return root;
}

struct rbtree_fl *rbtree_left_rotate_fl( struct rbtree_fl *root, struct rbtree_fl *node)
{
    struct rbtree_fl *right = node->right;
    /* Create node->right link */
    node->right = right->left;
    if (right->left != NullNode_fl)
        right->left->parent = node;
        /* Create right->parent link */
    if (right != NullNode_fl)
        right->parent = node->parent;
    if (node->parent != NullNode_fl)
    {
        if (node == node->parent->left)
            node->parent->left = right;
        else
            node->parent->right = right;
    } else {
        root = right;
    }
    right->left = node;
    if (node != NullNode_fl)
        node->parent = right;
    return root;
}

struct rbtree_fl *rbtree_right_rotate_fl(struct rbtree_fl *root, struct rbtree_fl *node)
{
    struct rbtree_fl *left = node->left;
    /* Create node->left link */
    node->left = left->right;
    if (left->right != NullNode_fl)
        left->right->parent = node;
    /* Create left->parent link */
    if (left != NullNode_fl)
        left->parent = node->parent;
    if (node->parent != NullNode_fl) {
        if (node == node->parent->right)
            node->parent->right = left;
        else
            node->parent->left = left;
    } else {
        root = left;
    }
    left->right = node;
    if (node != NullNode_fl)
        node->parent = left;
    return root;
}

void rbtree_print_fl(struct rbtree_fl *tree, int level)
{
    int i;
    if (tree == NULL)
        return;
    if(tree->deleted != 1 && tree != &EmptyNode_fl)
    {
        for (i = 0; i < level; i++)
            printf("\t");
		if (tree->color == COLOR_RED_FL)
		{
			printf("%d : %s\n", tree->key, gc_red);
		}
		else
		{
			printf("%d : %s\n", tree->key, gc_black);
		}
    }

    rbtree_print_fl(tree->left, level + 1);
    rbtree_print_fl(tree->right, level + 1);
}

void rbtree_free_fl(struct rbtree_fl *tree)
{
    if (tree == NULL)
        return;

    rbtree_free_fl(tree->left);
    rbtree_free_fl(tree->right);
    free(tree);
}

struct rbtree_fl *rbtree_lookup_fl(
struct rbtree_fl *tree, float key)
{
    while (tree != NULL)
    {
        if (key == tree->key) {
            return tree;
        } else if (key < tree->key) {
            tree = tree->left;
        } else {
            tree = tree->right;
        }
    }
    return tree;
}

struct rbtree_fl *rbtree_traverse_fl(struct rbtree_fl *tree, struct rbtree_fl *copy)
{
    if(tree == NULL)
        return copy;

    if(tree->deleted == 0)
    {
        copy = rbtree_add_fl(copy, tree->key, tree->value);
        tree->deleted = 1;
    }

	copy = rbtree_traverse_fl(tree->left, copy);
    copy = rbtree_traverse_fl(tree->right, copy);

    return copy;
}

struct rbtree_fl* rbtree_delete_fl(struct rbtree_fl *tree, float key, char *value)
{
    struct rbtree_fl *t = NULL;

    t = rbtree_lookup_fl(tree, key);

    if(t != NULL)
    {
        t->deleted = 1;
        ndeleted_nodes_fl++;
    }

    if(nnodes_fl/ndeleted_nodes_fl <= 2)
    {
        //start = NULL;
		nnodes_fl = 0;
        t = NULL;
        t = rbtree_traverse_fl(tree, t);
        rbtree_free_fl(tree);
        ndeleted_nodes_fl = 0;
        tree = t;
        //rbtree_free(t);
    }

    return tree;
}
