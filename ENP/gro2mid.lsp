;; gro2mid.lsp -- convert gro file to standard midi file
;;
;; Roger B. Dannenberg
;; 13 Jun 2022

(princ "Allegro file name to convert (without .gro): ")
(setf filename (read-line))
;; in case there was a left-over, unread newline, try again:
(cond ((or (null filename) (equal filename ""))
       (setf filename (read-line))))
(setf gro-name (strcat filename ".gro"))
(setf gio-file (open gro-name))
(cond ((null gio-file)
       (format t "Failed to open ~A~%" gro-name))
      (t
       (setf a-seq (seq-create))
       (seq-read a-seq  gio-file)
       (close gio-file)
       (setf smf-name (strcat filename ".mid"))
       (setf smf-file (open-binary smf-name :direction :output))
       (seq-write-smf a-seq smf-file)
       (format t "Wrote ~A~%" smf-name)))
(exit)



