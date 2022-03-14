#include "bt_soldier.h"


void bt_soldier::Init()
{
CreateRootNode("soldier", SELECTOR);
AddDecoratorNode("soldier","dec_escape", (bttask)&bt_soldier::DecoratorEscape);
AddCompositeNode("dec_escape", "escape", SEQUENCE);
AddTaskNode("escape", "part1", (bttask)&bt_soldier::TaskPart1);
AddTaskNode("escape", "part2", (bttask)&bt_soldier::TaskPart2);
AddCompositeNode("soldier", "idle",PARALLEL);
AddTaskNode("idle", "scream", (bttask)&bt_soldier::TaskScream);
AddTaskNode("idle", "shout", (bttask)&bt_soldier::TaskShout);
}

bool enter = true;

int bt_soldier::DecoratorEscape()
{
  if (enter)
  {
    printf("Gate open!\n");
    //enter = false;
    return SUCCESS;
  }
  else
  {
    printf("Gate closed...\n");
    return FAIL;
  }
}



int bt_soldier::TaskIdle()
{
printf("SOLDIER: Idle\n");
return SUCCESS;
}


int bt_soldier::TaskPart1()
{
  printf("SOLDIER: Part1\n");
  if (rand() % 4 == 0) return SUCCESS;
  if (rand() % 4 == 0) return FAIL;

  else return IN_PROGRESS;
}


int bt_soldier::TaskPart2()
{
printf("SOLDIER: Part2\n");
return SUCCESS;
}


int bt_soldier::TaskScream()
{
  printf("SOLDIER: Scream\n");
  if (rand() % 4 == 0)
  {
      printf("Scream returns SUCCESS, I should cancel Part2\n");
      return SUCCESS;
    }
  else
  {
      printf("Scream returns IN PROGRESS, I should carry on Part2\n");
      return IN_PROGRESS;
  }
}


int bt_soldier::TaskShout()
{
  printf("SOLDIER: Shout\n");
  return SUCCESS;
}



