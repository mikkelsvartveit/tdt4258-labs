CLOSE(2)                                                        Linux Programmer's Manual                                                       CLOSE(2)

NNAAMMEE
       close - close a file descriptor

SSYYNNOOPPSSIISS
       ##iinncclluuddee <<uunniissttdd..hh>>

       iinntt cclloossee((iinntt _f_d));;

DDEESSCCRRIIPPTTIIOONN
       cclloossee() closes a file descriptor, so that it no longer refers to any file and may be reused.  Any record locks (see ffccnnttll(2)) held on the file it
       was associated with, and owned by the process, are removed (regardless of the file descriptor that was used to obtain the lock).

       If _f_d is the last file descriptor referring to the underlying open file description (see ooppeenn(2)), the resources associated with  the  open  file
       description are freed; if the file descriptor was the last reference to a file which has been removed using uunnlliinnkk(2), the file is deleted.

RREETTUURRNN VVAALLUUEE
       cclloossee() returns zero on success.  On error, -1 is returned, and _e_r_r_n_o is set appropriately.

EERRRROORRSS
       EEBBAADDFF  _f_d isn't a valid open file descriptor.

       EEIINNTTRR  The cclloossee() call was interrupted by a signal; see ssiiggnnaall(7).

       EEIIOO    An I/O error occurred.

       EENNOOSSPPCC, EEDDQQUUOOTT
              On  NFS,  these  errors are not normally reported against the first write which exceeds the available storage space, but instead against a
              subsequent wwrriittee(2), ffssyynncc(2), or cclloossee(2).

       See NOTES for a discussion of why cclloossee() should not be retried after an error.

CCOONNFFOORRMMIINNGG TTOO
       POSIX.1-2001, POSIX.1-2008, SVr4, 4.3BSD.

NNOOTTEESS
       A successful close does not guarantee that the data has been successfully saved to disk, as the kernel uses the buffer  cache  to  defer  writes.
       Typically,  filesystems  do not flush buffers when a file is closed.  If you need to be sure that the data is physically stored on the underlying
       disk, use ffssyynncc(2).  (It will depend on the disk hardware at this point.)

       The close-on-exec file descriptor flag can be used to ensure that a file descriptor is automatically closed upon a successful eexxeeccvvee(2); see  ffcc--
       nnttll(2) for details.

       It  is probably unwise to close file descriptors while they may be in use by system calls in other threads in the same process.  Since a file de-
       scriptor may be reused, there are some obscure race conditions that may cause unintended side effects.

   DDeeaalliinngg wwiitthh eerrrroorr rreettuurrnnss ffrroomm cclloossee(())
       A careful programmer will check the return value of cclloossee(), since it is quite possible that errors on a previous wwrriittee(2) operation are reported
       only on the final cclloossee() that releases the open file description.  Failing to check the return value when closing a file may lead to _s_i_l_e_n_t loss
       of data.  This can especially be observed with NFS and with disk quota.

       Note, however, that a failure return should be used only for diagnostic purposes (i.e., a warning to the application that there may still be  I/O
       pending or there may have been failed I/O) or remedial purposes (e.g., writing the file once more or creating a backup).

       Retrying  the  cclloossee()  after  a failure return is the wrong thing to do, since this may cause a reused file descriptor from another thread to be
       closed.  This can occur because the Linux kernel _a_l_w_a_y_s releases the file descriptor early in the close operation,  freeing  it  for  reuse;  the
       steps that may return an error, such as flushing data to the filesystem or device, occur only later in the close operation.

       Many other implementations similarly always close the file descriptor (except in the case of EEBBAADDFF, meaning that the file descriptor was invalid)
       even if they subsequently report an error on return from cclloossee().  POSIX.1 is currently silent on this point, but there are plans to mandate this
       behavior in the next major release of the standard

       A careful programmer who wants to know about I/O errors may precede cclloossee() with a call to ffssyynncc(2).

       The EEIINNTTRR error is a somewhat special case.  Regarding the EEIINNTTRR error, POSIX.1-2013 says:

              If cclloossee() is interrupted by a signal that is to be caught, it shall return -1 with _e_r_r_n_o set to EEIINNTTRR and the state of _f_i_l_d_e_s is unspeci-
              fied.

       This permits the behavior that occurs on Linux and many other implementations, where, as with other errors that may be reported by  cclloossee(),  the
       file  descriptor  is  guaranteed  to be closed.  However, it also permits another possibility: that the implementation returns an EEIINNTTRR error and
       keeps the file descriptor open.  (According to its documentation, HP-UX's cclloossee() does this.)  The caller must then  once  more  use  cclloossee()  to
       close  the file descriptor, to avoid file descriptor leaks.  This divergence in implementation behaviors provides a difficult hurdle for portable
       applications, since on many implementations, cclloossee() must not be called again after an EEIINNTTRR error, and on at least one, cclloossee() must  be  called
       again.  There are plans to address this conundrum for the next major release of the POSIX.1 standard.

SSEEEE AALLSSOO
       ffccnnttll(2), ffssyynncc(2), ooppeenn(2), sshhuuttddoowwnn(2), uunnlliinnkk(2), ffcclloossee(3)

CCOOLLOOPPHHOONN
       This page is part of release 4.16 of the Linux _m_a_n_-_p_a_g_e_s project.  A description of the project, information about reporting bugs, and the latest
       version of this page, can be found at https://www.kernel.org/doc/man-pages/.

Linux                                                                  2017-09-15                                                               CLOSE(2)
