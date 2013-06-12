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

**This is all pretty crazy? How did it end up this way?**

Pretend you are a computer engineer in the 1960s. Computers are big and slow and users interact with them by feeding them [punch cards](http://en.wikipedia.org/wiki/Punch_cards). However, computers are finally getting fast enough to interact with users in real time. How are you going to design the interface that will allow them to all work on the machine at the same time??

If you're any good, I'll tell you how you'll do it... [You're going to be lazy!](http://threevirtues.com/)

Back in that era, the [old teletype terminals](http://en.wikipedia.org/wiki/Teleprinter) were broadly used in the telecommunications industry. The engineers of the day simply re-purposed this existing technology to fit their needs. This was the birth of the command line.

Make sure you read [The TTY demystified](http://www.linusakesson.net/programming/tty/) by [Linus Åkesson](http://www.linusakesson.net/). His page is the most enlightening for this topic anywhere on the internet. Many thanks to Linus for putting it together!

**How can you tell what the controlling tty is for any given process?**

The [stat](http://linux.die.net/man/5/proc) file contains that information. 

**How can you tell which session is controlled by any given tty?**

Traditionally, there is no easy way to see this information programmatically. (The "[ps j](http://linux.die.net/man/1/ps)" command can help you perform this discovery manually.) I wrote _ctty_ to fill this gap. It does the needed detective work, and reports back to the user. The library gives you a C interface to this functionality.


## _ctty_ Usage ##

To see the session information for a particular tty:
<pre><code>empty@monkey:~$ ctty /dev/pts/1
&#45;-------------------------------
TTY: /dev/pts/1
USER: empty

SID&#09;PGID&#09;PID&#09;FDs
---&#09;----&#09;---&#09;---
1480
&#09;1480
&#09;&#09;1480
&#09;&#09;&#09;0
&#09;&#09;&#09;1
&#09;&#09;&#09;2
&#09;&#09;&#09;255
&#09;2082
&#09;&#09;2082
&#09;&#09;&#09;0
&#09;&#09;&#09;1
&#09;&#09;&#09;2
&#45;-------------------------------
</code></pre>

The output fields above are:

 * SID:	Session ID
 * PGID:	Process group ID
 * PID:	Process ID
 * FDs:	File descriptors *which are open to this controlling tty.*

To see the session information for all tty's:

    empty@monkey:~$ sudo ctty

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


