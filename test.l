(defun else ()
  1)

(defun twice (a)
  (* 2 a))

(defun pow (a b)
  (cond ((= b 0) 1)
	(else (* a
	      (pow a (- b 1))))))

(defun map (f l)
  (cond ((nil? l) ())
	(else (cons
	    (f (car l))
	    (map f (cdr l))))))

(defun n (i l)
  (cond ((= 0 i) (car l))
	(else (n (- i 1) (cdr l)))))
