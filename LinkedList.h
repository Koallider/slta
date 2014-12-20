#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ListNode 
{
	/* Данные */
	char *data;
	int value;
	/* Указатель на следующий элемент */
	struct ListNode *next;
	struct ListNode *prev;
};

struct ListNode *ListCreateNode(char *data, int value)
{
	struct ListNode *p;
	p = (struct ListNode *)malloc(sizeof(*p));
	if (p != NULL) {
		p->data = data;
		p->value = value;
		p->next = NULL;
		p->prev = NULL;
	}
	return p;
}

struct ListNode *ListAddFront(struct ListNode *list, char *data, int value)
{
	/* Создаем новый элемент */
	struct ListNode *newnode;
	newnode = ListCreateNode(data, value);
	if (newnode != NULL) {
		newnode->next = list;
		list->prev = newnode;
		return newnode;
	}
	return list;
}

struct ListNode *ListLookup(struct ListNode *list, char *data, int value)
{
	for (; list != NULL; list = list->next) {
		if (strcmp(list->data, data) == 0 && list->value == value)
		{
			return list;
		}
	}
	return NULL;
}

struct ListNode *ListDelete(struct ListNode *list, char *data, int value)
{
	struct ListNode *p;
	for (p = list; p != NULL; p = p->next) {
		if (strcmp(p->data, data) == 0 && p->value == value)
		{
			if (p->prev == NULL) {
				list = p->next;
			}
			else {
				p->prev->next = p->next;
			}
			free(p);
			return list;
		}
	}
	return NULL;
}