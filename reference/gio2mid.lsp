;; gio2mid.lsp -- convert gio file to standard midi file
;;
;; Roger B. Dannenberg
;; 13 Jun 2022

(princ "Adagio file name to convert (without .gio): ")
(setf filename (read-line))
;; in case there was a left-over, unread newline, try again:
(cond ((or (null filename) (equal filename ""))
       (setf filename (read-line))))
(setf gro-name (strcat filename ".gio"))
(setf gio-file (open gio-name))
(cond ((null gio-file)
       (format t "Failed to open ~A~%" gio-name))
      (t
       (setf a-seq (seq-create))
       (seq-read a-seq  gio-file)
       (close gio-file)
       (setf smf-name (strcat filename "_gio.mid"))
       (setf smf-file (open-binary smf-name :direction :output))
       (seq-write-smf a-seq smf-file)
       (format t "Wrote ~A~%" smf-name)))
(exit)



