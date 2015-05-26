# Overlayfs


Address some non-standard behaviour of overlayfs. 

Specifically, quoting from https://lkml.org/lkml/2014/5/23/155 :

```
+Non-standard behavior
+---------------------
+
+The copy_up operation essentially creates a new, identical file and
+moves it over to the old name.  The new file may be on a different
+filesystem, so both st_dev and st_ino of the file may change.
+
+Any open files referring to this inode will access the old data and
+metadata.  Similarly any file locks obtained before copy_up will not
+apply to the copied up file.

```
This problem is demonstrated by the test program:
https://github.com/portworx/overlayfs/blob/master/test/ovl_rw_bug.c

The general approach taken in this fix is:
 1. Attach file struct coming from VFS to ovlfs's inode.
 2. Implement read and readpage operations and redirect them to a cached file struct opened on upper/lower fs
 3. i_size is set to s_maxbytes for ovlfs's inode to allow passthrough of read calls from VFS. 
  * lseek with SEEK_END is not an issue as we use generic_file_llseek_size() which takes eof offset as an argument.


Known issues:

* This change does not address overlay fs non-standard behavior with respect to hardlinks and locking.
* Page cache duplication in readpage path:
If a file, that is not yet copied up, is mmap'd and read, page cache for both lower fs and ovl fs is populated. 
This does not happen for normal reads, as ovl can avoid page cache while redirecting the read call to lower fs. If we try to redirect readpage call coming from page fault handler for the mmap'd region to a directIO call on lower fs, it results in dio to a kernel page. At present http://www.spinics.net/lists/linux-fsdevel/msg80902.html does not allow it.



Overlayfs and Docker
--------------------

Fixes the following two issues:

* https://github.com/docker/docker/issues/10180
* https://github.com/coreos/coreos-overlay/issues/1153
