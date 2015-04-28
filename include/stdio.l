(def stdin 0)
(def stdout 1)

(def print (\ (x)
	(write (car x))
	(cond 	((nil? (cdr x)) 1)
		(else (print (cdr x))))))

(def readln-h (\ (a) (cond
		((= a '\n') ())
		(else (cons a (readln-h (read)))))))

(def readln (\ () 
	(readln-h (read))))

(def printf (\ (x l)
	(cond ((= (car x) '~') (printf-h 