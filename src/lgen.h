#pragma once

static int next_num = 0;

void get_next_label_name(char* str)
{
  sprintf(str, "label%d", next_num++);
}