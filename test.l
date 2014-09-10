
(defun twice (a) (* 2 a))

(defun pow (a b) (cond ((= b 0) 1) (1 (* a (pow a (- b 1))))))

(defun map (f l) (cond ((nil? l) ()) (1 (cons (f (car l)) (map f (cdr l))))))
