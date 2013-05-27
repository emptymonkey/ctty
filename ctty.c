
/**********************************************************************
 *
 *	ctty : 2013-03-20
 *
 *		emptymonkey's tool for mapping sessions by their controlling tty.
 *              
 *
 *	Example use:
 *
 *		empty@monkey:~$ ctty /dev/pts/2
 *		--------------------------------
 *		TTY: /dev/pts/2
 *		USER: empty
 *		
 *		SID  PGID  PID  FDs
 *		---  ----  ---  ---
 *		1263
 *		     1263
 *		           1263
 *		                0
 *		                1
 *		                2
 *		                255
 *		     1790
 *		           1790
 *		                0
 *		                1
 *		                2
 *		--------------------------------
 *	
 *
 *	If run w/out an argument, it will attempt to return results for all ttys.
 *	(This will probably fail for most ttys unless you are root.)
 *
 **********************************************************************/


#include "libctty.h"

void ctty_print_session(struct sid_node *session_list);

#define MAX_INT_LEN 10


int main(int argc, char **argv){
	int i, retval;
	struct sid_node *session_head = NULL, *session_tail = NULL, *session_tmp;	
	glob_t pglob;


	if(argc > 2){
		error(-1, 0, "usage: %s [TTY_NAME]", program_invocation_short_name);
	}

	if(argc == 2){	
		if((session_head = ctty_get_session(argv[1])) == NULL){
			error(-1, errno, "ctty_get_session(%s)", argv[1]);
		}
	}else{
		if((retval = glob("/dev/tty*", 0, NULL, &pglob))){
			error(-1, errno, "glob(%s, %d, %p, %lx)", \
					"/dev/tty*", 0, NULL, (unsigned long) &pglob);
		}

		if((retval = glob("/dev/pts/[0-9]*", GLOB_APPEND, NULL, &pglob))){
			error(-1, errno, "glob(%s, %d, %p, %lx)", \
					"/dev/pts/[0-9]*", 0, NULL, (unsigned long) &pglob);
		}
		for(i = 0; i < (int) pglob.gl_pathc; i++){
			if(((session_tmp = ctty_get_session(pglob.gl_pathv[i])) == NULL) && (errno)){
				error(-1, errno, "ctty_get_session(%s)", pglob.gl_pathv[i]);
			}else if(session_tmp){

				if(!session_head){
					session_head = session_tmp;
					session_tail = session_tmp;
				}else{
					session_tail->next = session_tmp;
					session_tail = session_tmp;
				}
			}
		}
		globfree(&pglob);
	}
	ctty_print_session(session_head);

	return(0);
}


void ctty_print_session(struct sid_node *session_list){
	int i;

	char uid[MAX_INT_LEN + 1];

	struct sid_node *session;
	struct pgid_node *pgroup;
	struct pid_node *proc;

	session = session_list;
	while(session){

		memset(uid, 0, MAX_INT_LEN + 1);
		snprintf(uid, MAX_INT_LEN, "%d", session->uid);

		printf("--------------------------------\n");
		printf("TTY: %s\n", session->ctty);
		printf("USER: %s\n\n", cuserid(uid));
		printf("SID\tPGID\tPID\tFDs\n");
		printf("---\t----\t---\t---\n");

		printf("%d\n", session->sid);
		pgroup = session->pgid_head;
		while(pgroup){
			printf("\t%d\n", pgroup->pgid);

			proc = pgroup->pid_head;
			while(proc){
				printf("\t\t%d\n", proc->pid);

				for(i = 0; i < proc->fd_count; i++){
					printf("\t\t\t%d\n", proc->fds[i]);
				}
				proc = proc->next;
			}

			pgroup = pgroup->next;
		}
		session = session->next;
	}
	printf("--------------------------------\n");
}
