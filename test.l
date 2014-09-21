(define pow
  (\ (a b)
   (cond ((= b 0) 1)
	 (1 (* a
	       (pow a (- b 1)))))))
(print "2 to the 9 is %" (pow 2 9))
