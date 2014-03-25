#include "cache.h"

static int do_serve(const char *me, char *arg)
{
  size_t arg_count=0;
  int i;
  char *targ;
  char **args;
  char *help_args[] = { (char *)me, NULL }; 

  if(!arg)
    return execvp(me,(char* const*)help_args);
  if (strlen(arg) > 0){
    arg_count++;
  }
  for (i=0;i<strlen(arg);i++){
    if (*(arg+i) == ' ')
       arg_count++;
  }
  arg_count+=2;
  if(!(args = (char**)malloc(arg_count)))
    die("Unable to allocate additional memory!");
  args[0]=(char *)me;
  args[arg_count-1]=NULL;
  arg_count=1;
  targ = strtok(arg," ");
  while (targ != NULL){
    if (!strcmp(targ,"-d")||!strcmp(targ,"-i"))
      die("'-d' and '-i' switches not allowed, no daemonizing!");
    args[arg_count++] = targ;
    targ = strtok (NULL," ");
  }
  return execvp(me,(char* const*)args);
}

static struct commands {
        const char *name;
        int (*exec)(const char *me, char *arg);
} cmd_list[] = {
        { "svnserve", do_serve },
        { NULL }, 
};

int main(int argc, char **argv)
{
	char *prog;
	struct commands *cmd;
	int devnull_fd;

	/*
	 * Always open file descriptors 0/1/2 to avoid clobbering files
	 * in die().
	 */
	devnull_fd = open("/dev/null", O_RDWR);
	while (devnull_fd >= 0 && devnull_fd <= 2)
		devnull_fd = dup(devnull_fd);
	if (devnull_fd == -1)
		die_errno("opening /dev/null failed");
	close (devnull_fd);

	if (argc != 3 || strcmp(argv[1], "-c")) {
		usage("What, you think I'm a shell? Run me with the -c switch and a single command!");
	}

	if (!(prog = strdup(argv[2])))
    die("Unable to allocate additional memory!");
	for (cmd = cmd_list ; cmd->name ; cmd++) {
		int len = strlen(cmd->name);
		char *arg;
		if (strncmp(cmd->name, prog, len))
			continue;
		arg = NULL;
		switch (prog[len]) {
		case '\0':
			arg = NULL;
			break;
		case ' ':
			arg = prog + len + 1;
			break;
		default:
			continue;
		}
		exit(cmd->exec(cmd->name,arg));
	}
	die("Invalid command '%s'", argv[2]);
}
