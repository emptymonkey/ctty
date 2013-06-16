# ctty #

_ctty_ is a controlling tty discovery tool (and library).

**What is a tty?**

In Unix and Linux, users can issue commands to the operating system through a [command line](http://en.wikipedia.org/wiki/Command_line). In the modern era, a command line is implemented as a [shell](http://en.wikipedia.org/wiki/Shell_%28computing%29) attached to a [pseudo-terminal](http://linux.die.net/man/7/pty). The pseudo-terminal itself is a type of tty and leverages the [tty driver](http://lxr.linux.no/#linux+v3.9.5/drivers/tty) section of the [Linux kernel](https://www.kernel.org/).

**What is a _controlling_ tty?**

A controlling tty is a tty that has a special relationship with a [process session](http://www.win.tue.nl/~aeb/linux/lk/lk-10.html). When a tty is controlling for a session, it will send the session leader, and other members of that session, [signals](http://en.wikipedia.org/wiki/Unix_signal) to help control the user experience. 

**What is a session?**

Processes are grouped into process groups for [job control](http://en.wikipedia.org/wiki/Job_control_%28Unix%29). Process groups themselves are grouped together into a session for resource sharing of a tty. A very good reference on this topic is the [man page](http://en.wikipedia.org/wiki/Man_page) for [credentials](http://linux.die.net/man/7/credentials).

**What part of the operating system keeps track of all this?**

The tty driver in the kernel will know what session ID a tty is controlling for. Likewise, every process in a session will know which tty is controlling for it. Honestly, it seems to be a bit of a conspiracy between the driver and all of the processes in the session. No single piece acts authoritatively, and this relationship requires that all the participating members play well together. In the end, it seems to work out pretty well. 

**This sounds totally crazy? How did it end up this way?**

Back in late 1960s, computers were finally fast enough to interact with users in real time. Coincidentally, the [old teletype terminals](http://en.wikipedia.org/wiki/Teleprinter) were broadly used in throughout the telecommunications industry. The engineers of the day, being appropriately [lazy](http://threevirtues.com/), simply re-purposed this existing technology to fit their needs. This was the birth of the command line.

Make sure you read [The TTY demystified](http://www.linusakesson.net/programming/tty/) by [Linus Åkesson](http://www.linusakesson.net/). His page is the most enlightening for this topic anywhere on the internet. Many thanks to Linus for putting it together!

**How can you tell what the controlling tty is for any given process?**

The [stat](http://linux.die.net/man/5/proc) file for a process will contain that information. 

**How can you tell which session is controlled by any given tty?**

Traditionally, there is no easy way to see this information programmatically. (The "[ps j](http://linux.die.net/man/1/ps)" command can help you perform this discovery manually.) I wrote _ctty_ to fill this gap. It does the needed detective work, and reports back to the user. _libctty_ gives you a C interface to this functionality.


## _ctty_ Usage ##

To see the session information for a particular tty:
```
empty@monkey:~$ ctty /dev/pts/3
/dev/pts/3:empty:3099:3099:3099:0,1,2,255
/dev/pts/3:empty:3099:3158:3158:0,1,2
/dev/pts/3:empty:3099:3158:3170:1,2
/dev/pts/3:empty:3099:3176:3176:15,16,17,18,19
/dev/pts/3:empty:3099:3184:3184:0,1,2,5,6,7
```

The format is:

	TTY_NAME:USER:SID:PGID:PID:FD1,FD2,...,FDn

The fields are:

 * TTY_NAME: tty name
 * USER: user name (or uid if no match in /etc/passwd)
 * SID:	session ID
 * PGID:	process group ID
 * PID:	process ID
 * FDs:	file descriptors *which are open to this controlling tty.*

Notes:

 * Only the file descriptors that are pointing to the processes controlling tty are listed.
 * If you run _ctty_ without any arguments, it will attempt to return the results for all ttys. (This will probably fail for most ttys unless you are root.)
 * The -v switch will give a different output format that is a bit easier to read, though much longer and not fit for scripting.

## _libctty_ Usage ##

This is best documented inside the source code. However, as a quick overview, _libctty.h_ defines the following interfaces:

```
/* ctty_get_name() is used to discover the controlling tty for a process. */
char *ctty_get_name(int pid);

/* ctty_get_session() is used to map out the entire process session. */
struct sid_node *ctty_get_session(char *tty_name);

/* ctty_free_session() is used to release the session data structure. */
void ctty_free_session(struct sid_node *session);

/* ctty_stat_parse() will pull ctty and session related info from the processes stat file. */
int ctty_stat_parse(int pid, struct proc_stat *stat_info);

/* ctty_get_fds() returns the list of file descriptors open to the tty you're interested in. */
int ctty_get_fds(int pid, char *tty, int **fds);
```

## Installation ##

```
git clone git@github.com:emptymonkey/ctty.git
cd ctty
make
```


