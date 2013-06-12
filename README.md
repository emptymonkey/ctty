# ctty #

_ctty_ is a controlling [tty](http://www.linusakesson.net/programming/tty/) discovery tool (and library).

**What's a tty?**

In Unix and Linux, users can issue commands to the operating system through a [command line](http://en.wikipedia.org/wiki/Command_line). In the modern era, a command line is implemented by a [shell](http://en.wikipedia.org/wiki/Shell_%28computing%29) that the user interacts with through a [pseudo-terminal](http://linux.die.net/man/7/pty). The pseudo-terminal itself is a type of tty and leverages the [tty subsystem](http://lxr.linux.no/#linux+v3.9.5/drivers/tty) of the [Linux kernel](https://www.kernel.org/).

**What's a _controlling_ tty?**

A controlling tty is a tty that has a special relationship with a [process session](http://www.win.tue.nl/~aeb/linux/lk/lk-10.html). When a tty is controlling for a session, it will send the session leader, and other members of that session, [signals](http://en.wikipedia.org/wiki/Unix_signal) to help control the user experience. 

**What's a session?**
Processes are grouped into process groups. Process groups are grouped together into sessions. A session is that grouping of processes that cooperatively conspire to share a controlling tty. A very good reference on this topic is the [man page](http://en.wikipedia.org/wiki/Man_page) for [credentials](http://linux.die.net/man/7/credentials).

**What part of the operating system keeps track of all this?**

The tty driver in the kernel will know what session id a tty is controlling for. Also, any process knows which tty is controlling for it. Honestly, it seems to be a bit of a conspiracy between the two, as neither is really authoritative on the subject. All in all, it seems to work pretty well. 

**How can you tell what the controlling tty is for any given process?**

The [stat](http://linux.die.net/man/5/proc) file contains that information. 

**How can you tell which process session is controlled by a given tty?**

Traditionally, there is no easy way to see this information. I wrote _ctty_ to fill this gap. It does the needed detective work, and reports it back to the user. 

**This is all pretty crazy? How did it end up this way?**

***Short Version***

Pretend you are a computer engineer in the 1960s. Computers are big and slow. Users are used to interacting with them by feeding them [punch cards](http://en.wikipedia.org/wiki/Punch_cards). Around this time, however, the computers finally get fast enough to interact with users in real time. What interface would you invent to allow multiple users to talk to a machine all at the same time??

If you're any good, I'll tell you how you'd do it... [You'd be lazy!](http://c2.com/cgi/wiki?LazinessImpatienceHubris)

Back in that era, the old [teletype terminals](http://en.wikipedia.org/wiki/Teleprinter) were broadly used for communication. The engineers of the day simply re-purposed this technology to fit their needs. This was the birth of the command line.

***Long Version***

Go read [The TTY demystified](http://www.linusakesson.net/programming/tty/) by [Linus Åkesson](http://www.linusakesson.net/)! On this topic, his page is the most enlightening on the internet. Many thanks to Linus for putting this together!

**And there's a _ctty_ library too?**

I needed code to do ctty discovery for [another project](https://github.com/emptymonkey/shelljack). The library was the main code-base. The _ctty_ tool was simply my debug driver code. I then decided it may be useful enough to be promoted to "tool".

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

This is best documented inside the source code. However, as a quick overview, _libctty_ provides the following interfaces:
```
/************************************************************************
 * Functions for working with controlling ttys: 
 ************************************************************************/

/* ctty_get_name() is used to discover the controlling tty for a process. */
char *ctty_get_name(int pid);

/* ctty_get_session() is used to map out the entire process session. */
struct sid_node *ctty_get_session(char *tty_name);

/* ctty_free_session() is used to release the session data structure. */
void ctty_free_session(struct sid_node *session);

/************************************************************************
 * Functions for working with any ttys:
 ************************************************************************/

/* tty_stat_parse() will parse the processes stat file in /proc. */
int tty_stat_parse(int pid, struct proc_stat *stat_info);

/* tty_get_fds() returns the list of file descriptors open to the tty you're interested in. */
int tty_get_fds(int pid, char *tty, int **fds);
```

## Installation ##

```
git clone git@github.com:emptymonkey/ctty.git
cd ctty
make
```


