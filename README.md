# Overlayfs


Address limitations of overlayfs. 

Specifically quoting from https://lkml.org/lkml/2014/5/23/155 :

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
