#pragma once

#include <stdio.h>
#include <stdlib.h>
#define COLOR_RED 0
#define COLOR_BLACK 1

int nnodes = 0;
int ndeleted_nodes = 0;

const char* gc_red = "RED";
const char* gc_black = "BLACK";

struct rbtree *rbtree_add(struct rbtree *root, int key, char *value);
struct rbtree *rbtree_fixup_add(struct rbtree *root, struct rbtree *node);
struct rbtree *rbtree_left_rotate( struct rbtree *root, struct rbtree *node);
struct rbtree *rbtree_right_rotate(struct rbtree *root, struct rbtree *node);

struct rbtree
{
    int key;
    char *value;
    int color;
    int deleted;
    struct rbtree *parent;
    struct rbtree *left;
    struct rbtree *right;
};

struct rbtree EmptyNode = {-1, 0, COLOR_BLACK,
    NULL, NULL, NULL};
struct rbtree *NullNode = &EmptyNode;

struct rbtree *rbtree_add(struct rbtree *root,
int key, char *value)
{
    struct rbtree *node, *parent = NullNode;
    /* Search leaf for new element */
    for (node = root; node != NullNode && node != NULL; )
    {
        parent = node;
        if(key == node->key)
        {
            node->deleted = 0;
            node->value = value;
            ndeleted_nodes--;
            return node;
        }
        if (key < node->key)
            node = node->left;
        else if (key > node->key)
            node = node->right;
        else
            return root;
    }

    nnodes++;
    node = malloc(sizeof(*node));
    if (node == NULL)
        return NULL;
    node->key = key;
    node->value = value;
    node->deleted = 0;
    node->color = COLOR_RED;
    node->parent = parent;
    node->left = NullNode;
    node->right = NullNode;
    if (parent != NullNode) {
        if (key < parent->key)
            parent->left = node;
        else
            parent->right = node;
    } else {
        root = node;
    }
    return rbtree_fixup_add(root, node);
}

struct rbtree *rbtree_fixup_add(struct rbtree *root,
struct rbtree *node)
{
    struct rbtree *uncle;
    /* Current node is RED */
    while (node != root && node->parent->color == COLOR_RED)
    {
        if (node->parent == node->parent->parent->left)
        {
            /* node in left tree of grandfather */
            uncle = node->parent->parent->right;
                if (uncle->color == COLOR_RED) {
                /* Case 1 - uncle is RED */
                    node->parent->color = COLOR_BLACK;
                    uncle->color = COLOR_BLACK;
                    node->parent->parent->color = COLOR_RED;
                    node = node->parent->parent;
                } else {
                /* Cases 2 & 3 - uncle is BLACK */
                    if (node == node->parent->right) {
                    /* Reduce case 2 to case 3 */
                        node = node->parent;
                        root = rbtree_left_rotate(root, node);
                    }

                    /* Case 3 */
                    node->parent->color = COLOR_BLACK;
                    node->parent->parent->color = COLOR_RED;
                    root = rbtree_right_rotate(root, node->parent->parent);
                }
        } else {
        /* Node in right tree of grandfather */
            uncle = node->parent->parent->left;
            if (uncle->color == COLOR_RED) {
            /* Uncle is RED */
                node->parent->color = COLOR_BLACK;
                uncle->color = COLOR_BLACK;
                node->parent->parent->color = COLOR_RED;
                node = node->parent->parent;
            } else {
            /* Uncle is BLACK */
                if (node == node->parent->left)
                {
                    node = node->parent;
                    root = rbtree_right_rotate(root, node);
                }
                node->parent->color = COLOR_BLACK;
                node->parent->parent->color = COLOR_RED;
                root = rbtree_left_rotate(root,
                node->parent->parent);
            }
        }
    }
    root->color = COLOR_BLACK;
    return root;
}

struct rbtree *rbtree_left_rotate( struct rbtree *root, struct rbtree *node)
{
    struct rbtree *right = node->right;
    /* Create node->right link */
    node->right = right->left;
    if (right->left != NullNode)
        right->left->parent = node;
        /* Create right->parent link */
    if (right != NullNode)
        right->parent = node->parent;
    if (node->parent != NullNode)
    {
        if (node == node->parent->left)
            node->parent->left = right;
        else
            node->parent->right = right;
    } else {
        root = right;
    }
    right->left = node;
    if (node != NullNode)
        node->parent = right;
    return root;
}

struct rbtree *rbtree_right_rotate(struct rbtree *root, struct rbtree *node)
{
    struct rbtree *left = node->left;
    /* Create node->left link */
    node->left = left->right;
    if (left->right != NullNode)
        left->right->parent = node;
    /* Create left->parent link */
    if (left != NullNode)
        left->parent = node->parent;
    if (node->parent != NullNode) {
        if (node == node->parent->right)
            node->parent->right = left;
        else
            node->parent->left = left;
    } else {
        root = left;
    }
    left->right = node;
    if (node != NullNode)
        node->parent = left;
    return root;
}

void rbtree_print(struct rbtree *tree, int level)
{
    int i;
    if (tree == NULL)
        return;
    if(tree->deleted != 1 && tree != &EmptyNode)
    {
        for (i = 0; i < level; i++)
            printf("\t");
		if (tree->color == COLOR_RED)
		{
			printf("%d : %s\n", tree->key, gc_red);
		}
		else
		{
			printf("%d : %s\n", tree->key, gc_black);
		}
    }

    rbtree_print(tree->left, level + 1);
    rbtree_print(tree->right, level + 1);
}

void rbtree_free(struct rbtree *tree)
{
    if (tree == NULL)
        return;

    rbtree_free(tree->left);
    rbtree_free(tree->right);
    free(tree);
}

struct rbtree *rbtree_lookup(
struct rbtree *tree, int key)
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

struct rbtree *rbtree_traverse(struct rbtree *tree, struct rbtree *copy)
{
    if(tree == NULL)
        return copy;

    if(tree->deleted == 0)
    {
        copy = rbtree_add(copy, tree->key, tree->value);
        tree->deleted = 1;
    }

	copy = rbtree_traverse(tree->left, copy);
    copy = rbtree_traverse(tree->right, copy);

    return copy;
}

struct rbtree* rbtree_delete(struct rbtree *tree, int key, char *value)
{
    struct rbtree *t = NULL;

    t = rbtree_lookup(tree, key);

    if(t != NULL)
    {
        t->deleted = 1;
        ndeleted_nodes++;
    }

    if(nnodes/ndeleted_nodes <= 2)
    {
        //start = NULL;
		nnodes = 0;
        t = NULL;
        t = rbtree_traverse(tree, t);
        rbtree_free(tree);
        ndeleted_nodes = 0;
        tree = t;
        //rbtree_free(t);
    }

    return tree;
}
