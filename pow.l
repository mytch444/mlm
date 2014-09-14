(define pow
  (\ (a b)
   (cond ((= b 0) 1)
	 (1 (* a
	       (pow a (- b 1)))))))

